//
// Created by Filip Stamenkovic on 11/16/2017.
//

#ifndef P4_LINEARPROBING_H
#define P4_LINEARPROBING_H

#include "DefragRunner.h"
#include "mynew.h"
#include <iostream>

using namespace std;

struct Cell
{
  int formerIndex; //represents the index where something used to be
  int newIndex;
}; //Cell if it's NULL

class LinearProbing
{
public:
  //Constructors
  LinearProbing() {};
  LinearProbing(int size) : capacity(size)
       {array = new Cell[capacity];
       for(int i = 0; i < capacity; i++)
         array[i].formerIndex = -1;
       }; //initalize the capacity of the hash table, initialize the array to store that data, set all former indexes to -1
  // -1 DiskBlockID means that it is empty
  
  //methods to use in defragmenter
  void insert(int storageIndex, unsigned currentIndex, char location);
  int find(unsigned DiskBlockID);
  void updatelocation(int oldLocation, int newLocation, char RAMorDISK);
  void remove(unsigned DiskBlockID);
  int findOld(int currentIndex);
 // void print(); //to test the insert, remove
  
  
private:
  Cell *array;
  int capacity;
  int currSize;
  int hash (int key) {return key % capacity;}; //this is our hash function
  int findIndex(int key);
  
}; //this is our linear probing class


#endif //P4_LINEARPROBING_H
