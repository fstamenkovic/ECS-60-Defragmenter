//
// Created by Filip Stamenkovic on 11/16/2017.
//

#include "linearprobing.h"
#include <iostream>


using namespace std;

int LinearProbing::findIndex(int key)
{
  int index = hash(key);
  
  for(int i = 0; array[index].formerIndex != -1; i++)
    index = (index + 1) % capacity;
  
  return index;
} //find the index in which the DiskBlock should be placed

void LinearProbing::insert(int storageIndex, unsigned currentIndex, char location)
{
  int key = findIndex(currentIndex);
  if(location == 'r')
    storageIndex = -storageIndex; //make it negative, this is our encoding
  array[key].newIndex = storageIndex; //new index where it has been moved
  array[key].formerIndex = currentIndex; //record it's position before the move
  if(location == 'd') //make another entry if we are moving temp DiskBlock it to a different place on the disk
  { //this is not done for RAM as we will never move things in the RAM
    array[storageIndex].formerIndex = key; //to be able to update. Make an entry at the current location of temp that it has been moved.
  } //using the former index we will be able to track it back to the initial entry in the hash table and change the newIndex
  
//TODO run the debugger here again
  currSize++;
} //inserts into the Cell array, hash function based on the DiskBlockID

int LinearProbing::find(unsigned DiskBlockID) //tells us whether it's on the disk or RAM
{
  int index = hash(DiskBlockID);
  int count = 0;
  /*for(; array[index].formerIndex != DiskBlockID; count++)
  {
    index = (index + 1) % capacity;
  }*/
  
  return array[index].newIndex;
} //returns the DiskBlock pointer based on it's ID. returns NULL if it has not been found

void LinearProbing::remove(unsigned DiskBlockID)
{
  array[DiskBlockID].formerIndex = -1;
  currSize--;
}//removes the DiskBlock with this ID by setting the appropriate Cell to NULL

void LinearProbing::updatelocation(int oldLocation, int newLocation, char RAMorDISK)
{
  if(array[oldLocation].formerIndex == oldLocation) //check
    array[oldLocation].newIndex = newLocation; //update that the diskBlock has been moved to a new place on the disk
  
  if(RAMorDISK == 'd') //we are placing it on the disk. we need to make an entry at it's new location in order to be able to track it.
  {
    array[newLocation].formerIndex = oldLocation; //make it point to it's very first location so info does not get lost.
    //do this directly, no reason to point to the intermediate location anymore, in other words access array[currentIndex]
  }
}

int LinearProbing::findOld(int currentIndex)
{
  return array[currentIndex].formerIndex; //see if temp has been moved to this location
}


/*
void LinearProbing::print()
{
  int index = 0;
  
  for(; index < capacity; index++)
  {
    if(array[index].DiskBlockID == -1)
      cout << index << " NULL" << endl;
    else
      cout << "Index: " << index << ", DiskBlockID: " << array[index].DiskBlockID << ", FileBlockNum: " <<
           array[index].block->getFileBlockNum() << ", Next Index: " << array[index].block->getNext() << endl;
  }
  cout << endl;
}*/
