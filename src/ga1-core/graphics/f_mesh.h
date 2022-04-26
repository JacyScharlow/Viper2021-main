#ifndef MESH_H
#define MESH_H

#include <assert.h> 
#include <vector>
#include <string>
#include "f_hash.h"

#include "math/ga_mat4f.h"
#include "math/ga_vec2f.h"
#include "math/ga_vec3f.h"

class ArgParser;
class Vertex;
class Edge;
class Triangle;
class BoundingBox;
class Camera;

/*
** RPI Game Architecture Engine
** FINAL
**
** By: Jacy Scharlow
*/
// **********************************************************************

class Mesh {

public:

  // **********************************************************************
  // CONSTRUCTOR & DESTRUCTOR & LOAD
  Mesh() {}
  virtual ~Mesh();
  void ComputeGouraudNormals();

  // **********************************************************************
  // VERTICES
  int numVertices() const { return vertices.size(); }
  Vertex* addVertex(const ga_vec3f &pos);
  // look up vertex by index from original .obj file
  Vertex* getVertex(int i) const {
    assert (i >= 0 && i < numVertices());
    return vertices[i]; }

  // **********************************************************************
  // EDGES
  int numEdges() const { return edges.size(); }
  // this efficiently looks for an edge with the given vertices, using a hash table
  Edge* getMeshEdge(Vertex *a, Vertex *b) const;

  // **********************************************************************
  // TRIANGLES
  int numTriangles() const { return triangles.size(); }
  void addTriangle(Vertex *a, Vertex *b, Vertex *c);
  void removeTriangle(Triangle *t);

  // **********************************************************************
  // OTHER ACCESSORS
  ga_vec3f LightPosition() const;

  void packShadowVolumes(std::vector<ga_vec3f> *quads);
  
private:

  // **********************************************************************
  // REPRESENTATION
  std::vector<Vertex*> vertices;
  edgeshashtype edges;
  triangleshashtype triangles;

public:
  ga_vec3f _color = { 1, 1, 0 };
};

// **********************************************************************
// **********************************************************************

#endif




