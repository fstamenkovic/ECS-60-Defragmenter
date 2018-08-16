// Author Sean Davis
#ifndef defragmenterH
  #define defragmenterH

#include "mynew.h"
#include "DefragRunner.h"
#include "BinaryHeap.h"

class Defragmenter
{
  int diskSize;
public:
  Defragmenter(DiskDrive *diskDrive);
  void buildHeap(DiskDrive *diskDrive, BinaryHeap<int> *heap, int limit);
}; // class Defragmenter

#endif
