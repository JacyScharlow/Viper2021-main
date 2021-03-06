#ifndef _HASH_H_
#define _HASH_H_

#include <unordered_map>

class Edge;
class Triangle;
#include "f_vertex.h"
//#include <xhash>

#define LARGE_PRIME_A 10007
#define LARGE_PRIME_B 11003


// ******************************************************************
// edges are stored in a hash table - hash
// function based on the indices of the start and end vertices
// ******************************************************************

inline unsigned int ordered_two_int_hash(unsigned int a, unsigned int b) {
  return LARGE_PRIME_A * a + LARGE_PRIME_B * b;
}

struct orderedvertexpairhash {
  size_t operator()(std::pair<Vertex*,Vertex*> p) const {
    return ordered_two_int_hash(p.first->getIndex(),p.second->getIndex());
  }
};

struct orderedsamevertexpair {
  bool operator()(std::pair<Vertex*,Vertex*> p1, std::pair<Vertex*,Vertex*>p2) const {
    if (p1.first->getIndex() == p2.first->getIndex() && p1.second->getIndex() == p2.second->getIndex())
      return true;
    return false;
  }
};



// ******************************************************************
// vertex relationships
// ******************************************************************

inline unsigned int unordered_two_int_hash(unsigned int a, unsigned int b) {
  assert (a != b);
  if (b < a) {
    return ordered_two_int_hash(b,a);
  } else {
    assert (a < b);
    return ordered_two_int_hash(a,b);
  }
}

struct unorderedvertexpairhash {
  size_t operator()(std::pair<Vertex*,Vertex*> p) const {
    return unordered_two_int_hash(p.first->getIndex(),p.second->getIndex());
  }
};

struct unorderedsamevertexpair {
  bool operator()(std::pair<Vertex*,Vertex*> p1, std::pair<Vertex*,Vertex*>p2) const {
    if ((p1.first->getIndex() == p2.first->getIndex() && p1.second->getIndex() == p2.second->getIndex()) ||
	(p1.first->getIndex() == p2.second->getIndex() && p1.second->getIndex() == p2.first->getIndex())) return true;
    return false;
  }
};



// ******************************************************************
// triangle hashed with index
// ******************************************************************

struct idhash {
  size_t operator()(unsigned int id) const {
    return LARGE_PRIME_A * id;
  }
};

struct sameid {
  bool operator()(unsigned int a, unsigned int b) const {
    if (a == b)
      return true;
    return false;
  }
};



// to handle different platforms with different variants of a developing standard
// NOTE: You may need to adjust these depending on your installation
typedef std::unordered_map<std::pair<Vertex*,Vertex*>,Vertex*,unorderedvertexpairhash,unorderedsamevertexpair> vphashtype;
typedef std::unordered_map<std::pair<Vertex*,Vertex*>,Edge*,orderedvertexpairhash,orderedsamevertexpair> edgeshashtype;
typedef std::unordered_map<unsigned int,Triangle*,idhash,sameid> triangleshashtype;


#endif // _HASH_H_
