#ifndef _VERTEX_H
#define _VERTEX_H

#include "math/ga_mat4f.h"
#include "math/ga_vec2f.h"
#include "math/ga_vec3f.h"

/*
** RPI Game Architecture Engine
** FINAL
**
** By: Jacy Scharlow
*/
// ******************************************************************

class Vertex {

public:

  // ******************************************************************
  // CONSTRUCTOR & DESTRUCTOR
  Vertex(int i, const ga_vec3f& pos) { position = pos; index = i; }
  
  // ******************************************************************
  // ACCESSORS
  int getIndex() const { return index; }
  double x() const { return position.x; }
  double y() const { return position.y; }
  double z() const { return position.z; }
  const ga_vec3f& getPos() const { return position; }
  const ga_vec3f& getGouraudNormal() const { return gouraud_normal; }

  // ******************************************************************
  // MODIFIERS
  void setPos(const ga_vec3f&v) { position = v; }
  void clearGouraudNormal() { gouraud_normal.zero_vector(); }
  void incrGouraudNormal(const ga_vec3f&v) { gouraud_normal += v; }
  void normalizeGouraudNormal() { gouraud_normal.normalize(); }



private:

  // ******************************************************************
  // REPRESENTATION
  ga_vec3f position;
  ga_vec3f gouraud_normal;

  int index;  //In order of being created
};

#endif

