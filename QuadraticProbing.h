#ifndef _QUADRATIC_PROBING_H_
#define _QUADRATIC_PROBING_H_

#include "vector.h"
#include "mystring.h"
#include "mynew.h"

// QuadraticProbing Hash table class
//
// CONSTRUCTION: an initialization for ITEM_NOT_FOUND
//               and an approximate initial size or default of 101
//
// ******************PUBLIC OPERATIONS*********************
// void insert( x )       --> Insert x
// void remove( x )       --> Remove x
// Hashable find( x )     --> Return item that matches x
// int hash( String str, int tableSize )
//                        --> Static method to hash strings

template <class HashedObj>
class QuadraticHashTable
{
public:
  explicit QuadraticHashTable( const HashedObj & notFound, int size = 101 );
  QuadraticHashTable( const QuadraticHashTable & rhs )
      : ITEM_NOT_FOUND( rhs.ITEM_NOT_FOUND ),
        array( rhs.array ), currentSize( rhs.currentSize ) { }

  const HashedObj & find(unsigned index) const;
  void insert(unsigned index, const HashedObj & x);
  void makeEmpty( );
  void remove( unsigned index );
  const QuadraticHashTable & operator=( const QuadraticHashTable & rhs );
  enum EntryType { ACTIVE, EMPTY, DELETED };
private:
  struct HashEntry
  {
    unsigned index;
    HashedObj element;
    EntryType info;
    HashEntry( unsigned j = 0, const HashedObj & e = HashedObj( ), EntryType i = EMPTY )
        : index(j), element( e ), info( i ) { }
  };
  vector<HashEntry> array;
  int currentSize;
  const HashedObj ITEM_NOT_FOUND;
  bool isPrime( int n ) const;
  int nextPrime( int n ) const;
  bool isActive( int currentPos ) const;
  int findPos( unsigned index ) const;
  int hash( const string & key, int tableSize ) const;
  int hash( int key, int tableSize ) const;
  void rehash( );
};

#include "QuadraticProbing.cpp"
#endif
