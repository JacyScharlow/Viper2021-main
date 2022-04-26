/*
** RPI Game Architecture Engine
** FINAL
**
** By: Jacy Scharlow
*/

#include <iostream>
#include <cstring>
#include <unordered_map>

#include "f_mesh.h"
#include "f_edge.h"
#include "f_vertex.h"
#include "f_triangle.h"

int Triangle::next_triangle_id = 0;

// ***********************************************************************
// MESH DESTRUCTOR 
// ***********************************************************************
// =======================================================================

Mesh::~Mesh() {
  // delete all the triangles
  std::vector<Triangle*> todo;
  for (triangleshashtype::iterator iter = triangles.begin();
       iter != triangles.end(); iter++) {
    Triangle *t = iter->second;
    todo.push_back(t);
  }
  int num_triangles = todo.size();
  for (int i = 0; i < num_triangles; i++) {
    removeTriangle(todo[i]);
  }
  // delete all the vertices
  int num_vertices = numVertices();
  for (int i = 0; i < num_vertices; i++) {
    delete vertices[i];
  }
}

// ***********************************************************************
// MODIFIERS:   ADD & REMOVE
// ***********************************************************************

Vertex* Mesh::addVertex(const ga_vec3f &position) {
  int index = numVertices();
  Vertex *v = new Vertex(index, position);
  vertices.push_back(v);
  return v;
}


void Mesh::addTriangle(Vertex *a, Vertex *b, Vertex *c) {
  // create the triangle
  Triangle *t = new Triangle();
  // create the edges
  Edge *ea = new Edge(a,b,t);
  Edge *eb = new Edge(b,c,t);
  Edge *ec = new Edge(c,a,t);
  // point the triangle to one of its edges
  t->setEdge(ea);
  // connect the edges to each other
  ea->setNext(eb);
  eb->setNext(ec);
  ec->setNext(ea);
  // verify these edges aren't already in the mesh 
  // (which would be a bug, or a non-manifold mesh)
  if (edges.find(std::make_pair(a, b)) != edges.end() ||
    edges.find(std::make_pair(b, c)) != edges.end() ||
    edges.find(std::make_pair(c, a)) != edges.end())
    return;
  //assert (edges.find(std::make_pair(a,b)) == edges.end());
  //assert (edges.find(std::make_pair(b,c)) == edges.end());
  //assert (edges.find(std::make_pair(c,a)) == edges.end());
  // add the edges to the master list
  edges[std::make_pair(a,b)] = ea;
  edges[std::make_pair(b,c)] = eb;
  edges[std::make_pair(c,a)] = ec;
  // connect up with opposite edges (if they exist)
  edgeshashtype::iterator ea_op = edges.find(std::make_pair(b,a)); 
  edgeshashtype::iterator eb_op = edges.find(std::make_pair(c,b)); 
  edgeshashtype::iterator ec_op = edges.find(std::make_pair(a,c)); 
  if (ea_op != edges.end()) { 
    ea_op->second->setOpposite(ea); 
  }
  if (eb_op != edges.end()) { eb_op->second->setOpposite(eb); }
  if (ec_op != edges.end()) { ec_op->second->setOpposite(ec); }
  // add the triangle to the master list
  assert (triangles.find(t->getID()) == triangles.end());
  triangles[t->getID()] = t;
}


void Mesh::removeTriangle(Triangle *t) {
  Edge *ea = t->getEdge();
  Edge *eb = ea->getNext();
  Edge *ec = eb->getNext();
  Vertex *a = ea->getStartVertex();
  Vertex *b = eb->getStartVertex();
  Vertex *c = ec->getStartVertex();
  // remove these elements from master lists
  edges.erase(std::make_pair(a,b)); 
  edges.erase(std::make_pair(b,c)); 
  edges.erase(std::make_pair(c,a)); 
  triangles.erase(t->getID());
  // clean up memory
  delete ea;
  delete eb;
  delete ec;
  delete t;
}


Edge* Mesh::getMeshEdge(Vertex *a, Vertex *b) const {
  edgeshashtype::const_iterator iter = edges.find(std::make_pair(a,b));
  if (iter == edges.end()) return NULL;
  return iter->second;
}

// ***********************************************************************

// compute the gouraud normals of all vertices
void Mesh::ComputeGouraudNormals() {
  int i;
  // clear the normals
  for (i = 0; i < numVertices(); i++) {
    getVertex(i)->clearGouraudNormal();
  }
  // loop through all the triangles incrementing the normal at each vertex
  for (triangleshashtype::iterator iter = triangles.begin();
       iter != triangles.end(); iter++) {
    Triangle *t = iter->second;
    ga_vec3f n = t->ComputeNormal();
    (*t)[0]->incrGouraudNormal(n);
    (*t)[1]->incrGouraudNormal(n);
    (*t)[2]->incrGouraudNormal(n);
  }
  for (i = 0; i < numVertices(); i++) {
    getVertex(i)->normalizeGouraudNormal();
  }
}

// ***********************************************************************
// ***********************************************************************

ga_vec3f Mesh::LightPosition() const {
  ga_vec3f tmp {0, 150, 0};
  return tmp;
}

// ***********************************************************************

void Mesh::packShadowVolumes(std::vector<ga_vec3f>* quads) {

  ga_vec3f light_position = LightPosition();

  for (edgeshashtype::iterator iter = edges.begin();
    iter != edges.end(); iter++) {
    Edge* e = iter->second;
    ga_vec3f a_pos = e->getStartVertex()->getPos();
    ga_vec3f a_normal = e->getStartVertex()->getGouraudNormal();
    ga_vec3f b_pos = e->getEndVertex()->getPos();
    ga_vec3f b_normal = e->getEndVertex()->getGouraudNormal();

    ga_vec3f color = _color;

    ga_vec3f na = e->getTriangle()->ComputeNormal();


    if (e->getOpposite() == NULL) {
      continue;
    }

    ga_vec3f nb = e->getOpposite()->getTriangle()->ComputeNormal();

    ga_vec3f toLight = LightPosition() - e->getStartVertex()->getPos();
    toLight.normalize();
    float dota = na.dot(toLight);
    float dotb = nb.dot(toLight);


    if (dota > 0 && dotb < 0) {
      ga_vec3f da = a_pos - light_position;
      ga_vec3f db = b_pos - light_position;
      da.normalize();
      db.normalize();
      ga_vec3f cpos = db.scale_result(50) + b_pos;
      ga_vec3f dpos = da.scale_result(50) + a_pos;
      ga_vec3f normal = a_normal + b_normal;
      normal.normalize();

      quads->push_back(b_pos);
      quads->push_back(a_pos);
      quads->push_back(dpos);
      quads->push_back(cpos);

    }
  }
}