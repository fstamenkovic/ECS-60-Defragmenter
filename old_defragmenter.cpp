#include "defragmenter.h"
#include "DefragRunner.h"
#include "mynew.h"

/*
We have a hash table in which we store the positions of the diskBlocks that have been moved from their original position on the disk
 The hash table needs to somehow show whether the diskBlock is in RAM or at the back of the disk
 The locations are hashed by currentIndex, we store the formerIndex, the newIndex, and the char to say if it's in RAM or disk
 We need a data structure to contain the list of free locations in the RAM array, and the free locations of the disk(binaryheap)
 
 
*/

void Defragmenter::defragment()
{
  int count = 0;
  bool found = false;
  for(int i = diskSize - 1 ; i >= 2; i--)
  {
    if(!drive->FAT[i])
      heap->insert(i);
    
    count++;
  } //get the empty spaces on the disk inside the heap
  
  for(int currFile = 0; currFile < numFiles; currFile++) //this is the main loop, it goes through all files
  {
    found = false;
    int firstBlockID = drive->directory[currFile].getFirstBlockID();
    Cell *temp = locationStorage->find((unsigned)firstBlockID);
    currBlockID = drive->directory[currFile].getFirstBlockID();
    if(temp->formerIndex == -1) //it has not been moved, still in it's original position
      currBlock = drive->readDiskBlock(firstBlockID);
    else
    {
      if(temp->newIndex <= 0)
      {
        currBlock = &array[-(temp->newIndex)];
        arrayCount--; //decrease the count
        arrayOpenLocations->enqueue(-(temp->newIndex));
        found = true;
      }
      while(!found)
      {
        if(temp->newIndex > 0)
        {
          if(temp->newIndex <= currentIndex)
          {
            temp = locationStorage->find(temp->newIndex);
            found = false;
          }
    
          else
          {
            currBlock = drive->readDiskBlock(temp->newIndex);
            found = true;
          }
          currBlockID = temp->newIndex;
          // TODO: do this
        }
      }
      
      
    } //this will be rewritten too
    
    
    //currBlockID = drive->directory[currFile].getFirstBlockID();
    drive->directory[currFile].setFirstBlockID((unsigned)currentIndex); //had a nasty bug right there
    
    defragCurrFile();
  } //don't delete Cell temps
  
  cout << "ArrayCount at end : " << arrayCount << endl;
  
  //delete heap;
  delete arrayOpenLocations;
  delete array;
}

void Defragmenter::defragCurrFile()
{
  while(1) //index 40 something acts up
  {
    int nextID = currBlock->getNext();
    if(currentIndex == 10000)
      cout << "reached " << currentIndex << endl;
    
    if (nextID != 1)
      currBlock->setNext((unsigned) currentIndex + 1); //final adjustment to the current block
    
    if(needsDefrag(nextID))
    {
      if (!drive->FAT[currentIndex])
      {
        drive->writeDiskBlock(currBlock, currentIndex);
        drive->FAT[currentIndex] = true; //mark the current slot as free
        drive->FAT[currBlockID] = false; //mark the slot from where we took the diskblock as empty(false)
        if(currentIndex > currBlockID)
          heap->insert(currBlockID); //put the index of the one that freed up into the heap
      } //simplest version, the space is free, and we do not need to store any DiskBlock
  
      else
      {
        overwriteAndStore();
        //drive->FAT[currBlockID] = false;
      }
  
      if(currBlock->getNext() == 1)
      {
        //printDisk();
        currentIndex++;
        break;
      }
    }
    
  
    currBlockID = nextID; //move on the the next block, first we get it's ID(index)
    //printDisk();
    moveToNext();
    //cout << "arrayCount: " << arrayCount << endl;
    //cout << "Binary Heap max: " << heap->findMax() << endl;
  }
}

bool Defragmenter::needsDefrag(int nextID)
{
  if(currBlock->getNext() == nextID && nextID != 1 && drive->FAT[currentIndex] == true)
    return false;
  
  else
    return true;
}

void Defragmenter::overwriteAndStore()
{
  if(currBlockID > currentIndex)
  {
    heap->insert(currBlockID); //store that index into the heap
    drive->FAT[currBlockID] = false; //we just moved our currBlock to the currentIndex, we need to let FAT know space has freed up
  }
  
  DiskBlock* temp = drive->readDiskBlock(currentIndex); //get the diskBlock at our current index, now go on to store it
  
  
  if(isThereSpaceOnRAM()) //place it in RAM
  {
    int indexInArray = arrayOpenLocations->dequeue();
    DiskBlock tempSec = *currBlock; //make a deep copy!
    currBlock = &tempSec; //make a deep copy!
    array[indexInArray] = *temp;
    locationStorage->insert(indexInArray, currentIndex, 'r');
    arrayCount++;
    delete temp;
  }
  
  else //place it on the disk
  {
    int highestIndex = heap->findMax(); //get the highest index from the heap
    heap->deleteMax(); //remove it
    drive->writeDiskBlock(temp, highestIndex);
    drive->FAT[highestIndex] = true;
    locationStorage->insert(highestIndex, currentIndex, 'd');
    delete temp;
    //TODO Binary heap comes into play
  }
  
  drive->writeDiskBlock(currBlock, currentIndex); //write the current disk block in it's space
  drive->FAT[currentIndex] = true;
}


void Defragmenter::moveToNext()
{
  if(currBlockID <= currentIndex)  //this is to see how we access the next disk Block, from RAM storage or from disk storage
  {//access from RAMStorage, this means that we have already moved it out of the way from the Disk in the past
    Cell *temp = locationStorage->find((unsigned) currBlockID);
    bool found = false;
    
    while(!found)  //write an updateLocation method
    {
      if(temp->newIndex <= 0) //if it's in RAM, retrieve it from the array, and delete it.
      {
        currBlock = &array[-(temp->newIndex)];
        arrayCount--;
        arrayOpenLocations->enqueue(-(temp->newIndex)); //get the index that was used available for use again
        found = true;
      }
      if(temp->newIndex > 0) //if it's in the disk
      {
        if(temp->newIndex <= currentIndex)
        {
          temp = locationStorage->find(temp->newIndex);
          found = false;
        }
        
        else
        {
          currBlock = drive->readDiskBlock(temp->newIndex);
          currBlockID = temp->newIndex;
          found = true;
        }
        
        // TODO: do this
      }
    }
    
    
    
    locationStorage->remove(currBlockID);
  }
  
  else
  {
    currBlock = drive->readDiskBlock(currBlockID); //just get it from the hard drive, it has not been moved out
  }
  
  currentIndex++;
}

void Defragmenter::printDisk()
{
  for(int i = 2; i < diskSize; i++)
  {
    DiskBlock *itr = drive->readDiskBlock(i);
    cout << i << ". FileID: " << itr->getFileID() << ", FileBlockNum: " << itr->getFileBlockNum() << ", Next Block: " << itr->getNext() << endl;
    itr = drive->readDiskBlock(i+1);
  }
  
  cout << endl;
}