/*
** RPI Game Architecture Engine
** FINAL
**
** By: Jacy Scharlow
*/

#include "f_obj_parser.h"
#include "f_mesh.h"

#include "ga_animation.h"
#include "ga_geometry.h"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <istream>

#include <vector>

//**************************************************************
//Parses Obj files and constructs a Mesh object based off of the information.

void obj_to_model(const char* filename, ga_model* model, Mesh* mesh)
{
	extern char g_root_path[256];
	std::string fullpath = g_root_path;
	fullpath += filename;

	FILE* objfile = fopen(fullpath.c_str(), "r");
	if (objfile == NULL) {
		std::cout << "ERROR! CANNOT OPEN '" << fullpath << "'\n";
		return;
	}
	else {
		std::cout << "OPENED '" << fullpath << "'\n";
	}

	char line[200] = "";
	char token[100] = "";
	char atoken[100] = "";
	char btoken[100] = "";
	char ctoken[100] = "";
	float x, y, z;
	int a, b, c, d, e, f, g, h, i;

	int index = 0;
	int vert_count = 0;
	int vert_index = 1;

	std::vector<ga_vec3f> verts;
	std::vector<ga_vec3f> norms;
	std::vector<ga_vec2f> uvs;

	while (fgets(line, 200, objfile)) {
		int token_count = sscanf(line, "%s\n", token);
		if (token_count == -1) continue;
		a = b = c = d = e = f = g = h = i = -1;

		if (!strcmp(token, "v")) {

			int num = sscanf(line, "%s %f %f %f\n", token, &x, &y, &z);
			ga_vec3f temp;
			temp.x = x;
			temp.y = y;
			temp.z = z;
			verts.push_back(temp);

		}
		else if (!strcmp(token, "vt")) {
			int num = sscanf(line, "%s %f %f\n", token, &x, &y);
			ga_vec2f temp;
			temp.x = x;
			temp.y = y;
			uvs.push_back(temp);
		}
		else if (!strcmp(token, "vn")) {

			int num = sscanf(line, "%s %f %f %f\n", token, &x, &y, &z);
			ga_vec3f temp;
			temp.x = x;
			temp.y = y;
			temp.z = z;
			norms.push_back(temp);

		}
		else if (!strcmp(token, "f")) {
		
			int num = sscanf(line, "%s %d/%d/%d %d/%d/%d %d/%d/%d\n", token,
				&a, &b, &c, &d, &e, &f, &g, &h, &i);
			if (num < 10 || num == EOF) {
				num = sscanf(line, "%s %d %d %d\n", token,
					&a, &b, &c, &d, &e, &f, &g, &h, &i);

				ga_vertex v1, v2, v3;

				//DEBUG
				//std::cout << a << ":" << d << ":" << g << "\t" << verts.size();
				//std::cout << b << ":" << e << ":" << h << "\t" << uvs.size();
				//std::cout << c << ":" << f << ":" << i << "\t" << norms.size();

				if (model->_vertices.size() < verts.size()) {
					for (int i = 0; i < verts.size(); i++) {
						ga_vertex vt;
						vt._position.x = verts[i].x;
						vt._position.y = verts[i].y;
						vt._position.z = verts[i].z;
						model->_vertices.push_back(vt);

						mesh->addVertex(vt._position);
					}
				}

				model->_indices.push_back(a - 1);
				model->_indices.push_back(b - 1);
				model->_indices.push_back(c - 1);

				/*v1._position.x = verts[a - 1].x;
				v1._position.y = verts[a - 1].y;
				v1._position.z = verts[a - 1].z;

				v2._position.x = verts[b - 1].x;
				v2._position.y = verts[b - 1].y;
				v2._position.z = verts[b - 1].z;

				v3._position.x = verts[c - 1].x;
				v3._position.y = verts[c - 1].y;
				v3._position.z = verts[c - 1].z;

				model->_vertices.push_back(v1);
				model->_vertices.push_back(v2);
				model->_vertices.push_back(v3);

				int verts_size = model->_vertices.size();

				//DEBUG
				//std::cout << verts_size << std::endl;

				uint32_t indices[3];
				indices[0] = verts_size - 3;
				indices[1] = verts_size - 2;
				indices[2] = verts_size - 1;

				model->_indices.push_back(indices[0]);
				model->_indices.push_back(indices[1]);
				model->_indices.push_back(indices[2]);*/
			}
			else {
				ga_vertex v1, v2, v3;

				//DEBUG
				//std::cout << verts[a - 1].x << ":" << verts[a - 1].y << ":" << verts[a - 1].z << std::endl;
				//std::cout << verts[d - 1].x << ":" << verts[d - 1].y << ":" << verts[d - 1].z << std::endl;
				//std::cout << verts[g - 1].x << ":" << verts[g - 1].y << ":" << verts[g - 1].z << std::endl;
				//std::cout << a << ":" << d << ":" << g << "\t" << verts.size() << std::endl;
				//std::cout << b << ":" << e << ":" << h << "\t" << uvs.size();
				//std::cout << c << ":" << f << ":" << i << "\t" << norms.size();

				if (model->_vertices.size() < verts.size()) {
					for (int i = 0; i < verts.size(); i++) {
						ga_vertex vt;
						vt._position.x = verts[i].x;
						vt._position.y = verts[i].y;
						vt._position.z = verts[i].z;

						vt._normal.x = 0.0f;
						vt._normal.y = 0.0f;
						vt._normal.z = 0.0f;

						model->_vertices.push_back(vt);

						mesh->addVertex(vt._position);
					}
				}

				model->_vertex_format |= k_vertex_attribute_normal;

				model->_vertices[a - 1]._normal += norms[c - 1];
				model->_vertices[d - 1]._normal += norms[f - 1];
				model->_vertices[g - 1]._normal += norms[i - 1];

				model->_indices.push_back(a - 1);
				model->_indices.push_back(d - 1);
				model->_indices.push_back(g - 1);

				mesh->addTriangle(mesh->getVertex(a - 1),
					mesh->getVertex(d - 1),
					mesh->getVertex(g - 1));

				//v1._position.x = verts[a - 1].x;
				//v1._position.y = verts[a - 1].y;
				//v1._position.z = verts[a - 1].z;
				//
				//v2._position.x = verts[d - 1].x;
				//v2._position.y = verts[d - 1].y;
				//v2._position.z = verts[d - 1].z;
				//
				//v3._position.x = verts[g - 1].x;
				//v3._position.y = verts[g - 1].y;
				//v3._position.z = verts[g - 1].z;

				//model->_vertex_format |= k_vertex_attribute_uv;

				//v1._uv.x = uvs[b - 1].x;
				//v1._uv.y = uvs[b - 1].y;
				//
				//v2._uv.x = uvs[e - 1].x;
				//v2._uv.y = uvs[e - 1].y;
				//
				//v3._uv.x = uvs[h - 1].x;
				//v3._uv.y = uvs[h - 1].y;

				//model->_vertex_format |= k_vertex_attribute_normal;
				

				//v1._normal.x = norms[c - 1].x;
				//v1._normal.y = norms[c - 1].y;
				//v1._normal.z = norms[c - 1].z;
				//
				//v2._normal.x = norms[f - 1].x;
				//v2._normal.y = norms[f - 1].y;
				//v2._normal.z = norms[f - 1].z;
				//
				//v3._normal.x = norms[i - 1].x;
				//v3._normal.y = norms[i - 1].y;
				//v3._normal.z = norms[i - 1].z;

				//model->_vertex_format |= k_vertex_attribute_color;

				//v1._color.x = 0.5f;
				//v1._color.y = 0.5f;
				//v1._color.z = 0.5f;
				//v2._color.x = 0.5f;
				//v2._color.y = 0.5f;
				//v2._color.z = 0.5f;
				//v3._color.x = 0.5f;
				//v3._color.y = 0.5f;
				//v3._color.z = 0.5f;
				//
				//model->_vertices.push_back(v1);
				//model->_vertices.push_back(v2);
				//model->_vertices.push_back(v3);

				int verts_size = model->_vertices.size();

				//DEBUG
				//std::cout << verts_size << std::endl;

				//uint32_t indices[3];
				//indices[0] = verts_size - 3;
				//indices[1] = verts_size - 2;
				//indices[2] = verts_size - 1;
				//
				//model->_indices.push_back(indices[0]);
				//model->_indices.push_back(indices[1]);
				//model->_indices.push_back(indices[2]);
			}


		}
	}

	for (int i = 0; i < model->_vertices.size(); i++) {
		
		//std::cout << model->_vertices[i]._normal.x << "\t" << model->_vertices[i]._normal.y << "\t" << model->_vertices[i]._normal.z << std::endl;

		model->_vertices[i]._normal.normalize();
		
		//std::cout << model->_vertices[i]._normal.x << "\t" << model->_vertices[i]._normal.y << "\t" << model->_vertices[i]._normal.z << std::endl;
	}
	mesh->ComputeGouraudNormals();

	//DEBUG
	std::cout << "exiting" << std::endl;

}
