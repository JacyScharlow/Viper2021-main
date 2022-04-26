/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_cube_component.h"
#include "ga_material.h"

#include "entity/ga_entity.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <iostream>
ga_cube_component::ga_cube_component(ga_entity* ent, std::vector<ga_vec3f> quads) : ga_component(ent)
{
	_material = new ga_constant_color_material();
	_material->init();
	_material->set_color({ 1,1,0 });

	std::vector<GLushort> indices;
	for (int i = 0; i < quads.size(); i += 4) {
		indices.push_back(i); indices.push_back(i + 1); indices.push_back(i + 2);
		indices.push_back(i + 2); indices.push_back(i + 3); indices.push_back(i + 0);
	}
	std::cout << "Q: " << quads.size() << std::endl;
	std::cout << "I: " << indices.size() << std::endl;

	for (int i = 0; i < quads.size(); i++)
	{
		//std::cout << quads[i].x << " " << quads[i].y << " " << quads[i].z << std::endl;
	}

	std::vector<ga_vec3f> tempQ;
	ga_vec3f v1;

	_index_count = uint32_t(sizeof(indices)/sizeof(indices[0]));
	_index_count = indices.size();
	std::cout << "Index Count: " << _index_count << std::endl;
	GLsizei quad_size = sizeof(quads[0]);
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glGenBuffers(2, _vbos);
	
	glBindBuffer(GL_ARRAY_BUFFER, _vbos[0]);
	glBufferData(GL_ARRAY_BUFFER, quads.size() * quad_size, &quads[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, quad_size, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbos[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), &indices[0], GL_STATIC_DRAW);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbos[1]);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, model->_indices.size() * sizeof(model->_indices[0]), &model->_indices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
}

ga_cube_component::~ga_cube_component()
{
	glDeleteBuffers(4, _vbos);
	glDeleteVertexArrays(1, &_vao);

	delete _material;
}

void ga_cube_component::update(ga_frame_params* params)
{
	float dt = std::chrono::duration_cast<std::chrono::duration<float>>(params->_delta_time).count();
	//ga_quatf axis_angle;
	//axis_angle.make_axis_angle(ga_vec3f::y_vector(), ga_degrees_to_radians(60.0f) * dt);
	//get_entity()->rotate(axis_angle);

	ga_static_drawcall draw;
	draw._name = "ga_cube_component";
	draw._vao = _vao;
	draw._index_count = _index_count;
	draw._transform = get_entity()->get_transform();
	draw._draw_mode = GL_TRIANGLES;
	draw._material = _material;

	while (params->_shadow_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
	
	params->_shadow_drawcalls.push_back(draw);
	params->_shadow_drawcall_lock.clear(std::memory_order_release);
}
