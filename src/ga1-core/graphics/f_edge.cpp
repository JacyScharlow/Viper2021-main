/*
** RPI Game Architecture Engine
** FINAL
**
** By: Jacy Scharlow
*/

#include "f_vertex.h"
#include "f_edge.h"
#include "f_triangle.h"



// EDGE CONSTRUCTOR
Edge::Edge(Vertex *vs, Vertex *ve, Triangle *t) {
  start_vertex = vs;
  end_vertex = ve;
  triangle = t;
  next = NULL;
  opposite = NULL;
}

// EDGE DESTRUCTOR
Edge::~Edge() { 
  if (opposite != NULL)
    opposite->opposite = NULL;
}

float Edge::Length() const {
  ga_vec3f diff = start_vertex->getPos() - end_vertex->getPos();
  return diff.mag();
}

bool Edge::isSilhouette(const ga_vec3f&lightpos) const {
  ga_vec3f na = getTriangle()->ComputeNormal();
  ga_vec3f nb = getOpposite()->getTriangle()->ComputeNormal();


  ga_vec3f toLight = lightpos - getStartVertex()->getPos();
  toLight.normalize();

  float dota = na.dot(toLight);
  float dotb = nb.dot(toLight);

  if ((dota > 0 && dotb < 0) ||
      (dota < 0 && dotb > 0))
    return true;

  return false;
  
}
