#include "defragmenter.h"
#include "DefragRunner.h"
#include "mynew.h"
#include "QueueAr.h"
#include "linearprobing.h"

//TODO:: here's the problem. heap overflows. the reason is that when we find an empty slot, we insert currBlock into it.
//TODO:: but, we don't remove that slot from the heap. Possible solution: write a method to remove it from heap. It will take more CPU time.

Defragmenter::Defragmenter(DiskDrive *diskDrive) : diskSize(diskDrive->getCapacity())
{
  int arraySize = 6500;
  int numFiles = diskDrive->getNumFiles();
  int currBlockID = 0;
  int currentIndex = 2; //this is the first index that we can write on
  int fragments;
  DiskBlock *emptyBlock = diskDrive->readDiskBlock(0);
  DiskBlock* currBlock = NULL;
  DiskBlock *temp = NULL;
  BinaryHeap<int> *heap = new BinaryHeap<int>(100000); //heap to contain the free slots at the back
  LinearProbing *LocationHash = new LinearProbing(500000);
  DiskBlock **array = new DiskBlock*[arraySize];
  Queue<int> *RAMIndexes = new Queue<int>(arraySize);
  
  buildHeap(diskDrive, heap, 0);
  
  for(int i = 0; i < arraySize ; i++)
    RAMIndexes->enqueue(i); //queue contains open locations in the array
  
  
  for(int currFile = 0; currFile < numFiles; currFile++)
  {
    currBlockID = diskDrive->directory[currFile].getFirstBlockID(); //get the starting block in a file
    if(currentIndex != currBlockID)
      diskDrive->directory[currFile].setFirstBlockID((unsigned)currentIndex);
    fragments = diskDrive->directory[currFile].getSize();
    
    for(int currFragment = 0; currFragment < fragments; currFragment++)
    {
      if(heap->isFull() || heap->isEmpty())
      {
        heap->makeEmpty();
        buildHeap(diskDrive, heap, currentIndex);
      }
      
      //first find the currBlock
      if(currentIndex <= currBlockID)
      { //the block we're looking for has not been moved, it's further down the disk
        currBlock = diskDrive->readDiskBlock(currBlockID);
        diskDrive->FAT[currBlockID] = false; //mark it's position on the disk as empty
        if(currentIndex < currBlockID)
        { //we have to let heap know of new available index. does not make sense to do if currentIndex == currBlockID since we're writing to it right away
          heap->insert(currBlockID); //space has freed up further down the array
          //diskDrive->writeDiskBlock(emptyBlock,currBlockID); //make it physically empty, useful for debugging
        }
        
      }
      
      if(currentIndex > currBlockID)
      { //the block we're looking for has been moved
        int newLocation = LocationHash->find(currBlockID); //get it's updated location from hash
        
        if(newLocation <= 0)
        { //it's in RAM, get currBlock from there
          currBlock = array[-newLocation];
          RAMIndexes->enqueue(-newLocation); //let the queue know space has freed up
        }
        
        if(newLocation > 1)
        { //it has been moved further down on the disk
          currBlock = diskDrive->readDiskBlock(newLocation);
          diskDrive->FAT[newLocation] = false; //we have taken it from that location, mark it as free
          if(currentIndex < newLocation) //it does not make sense to insert to heap if currentIndex == newLocation
          { //newLocation is further down the disk, we have to let heap know we have freed space up
            heap->insert(newLocation);
            //diskDrive->writeDiskBlock(emptyBlock, newLocation);
          }
        }
      } //we are done fetching currFile
      
      if(currentIndex != currBlockID) //if they are the same, then we don't need to defragment
      {
        if(diskDrive->FAT[currentIndex]) //it's full, we need to move it's contents
        {
          temp  = diskDrive->readDiskBlock(currentIndex);
          int oldIndex = LocationHash->findOld(currentIndex); //we have to figure out if temp has been moved to this location already
          
          if(!RAMIndexes->isEmpty())
          { //there are free indexes in RAM, so there is space in RAM
            int RAMIndex = RAMIndexes->dequeue(); //get an available one
            array[RAMIndex] = temp;
            
            if(oldIndex == -1) //no, temp has not been moved to this location
            {
              LocationHash->insert(RAMIndex, currentIndex, 'r');
            }
            
            else
            {
              LocationHash->updatelocation(oldIndex, -RAMIndex, 'r');
              LocationHash->remove(currentIndex); //temp is out of this index, remove it from this index
            }
          }
          
          else
          { //we have to store temp on the disk
            int highestIndex = heap->findMax();
            heap->deleteMax();
            
            if(oldIndex == -1) //no, temp has not been moved to this location
            {
              LocationHash->insert(highestIndex, currentIndex, 'd');
            }
            
            else //yes, temp has previously been moved to this location, update out hash
            {
              LocationHash->updatelocation(oldIndex, highestIndex, 'd');
              LocationHash->remove(currentIndex); //temp is out of here, no more need to have it here
            }
            
            diskDrive->writeDiskBlock(temp, highestIndex);
            diskDrive->FAT[highestIndex] = true;
            delete temp;
          }
          
        }
        diskDrive->writeDiskBlock(currBlock, currentIndex); //finally, write currBlock right here
        diskDrive->FAT[currentIndex] = true; //make it full
      }
      currBlockID = currBlock->getNext(); //start over
      delete currBlock; //avoid memory leaks
      currentIndex++;
    }
    
  }
  
  delete heap;
  delete RAMIndexes;
  delete LocationHash;
  delete array;
  
  
  
  
  
} // Defragmenter()


void Defragmenter::buildHeap(DiskDrive *diskDrive, BinaryHeap<int> *heap, int limit)
{
  for(int i = diskSize - 1 ; i >= 2 ; i--)
  { //initialize the heap
    if(!diskDrive->FAT[i])
      heap->insert(i);
  } //get the empty spaces on the disk inside the heap
}