/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_output.h"

#include "ga_frame_params.h"

#include "graphics/ga_material.h"
#include "graphics/ga_program.h"
#include "math/ga_mat4f.h"
#include "math/ga_quatf.h"

#include <cassert>
#include <iostream>
#include <SDL.h>

#include <windows.h>

#define GLEW_STATIC
#include <GL/glew.h>

ga_output::ga_output(void* win) : _window(win)
{
	int width, height;
	SDL_GetWindowSize(static_cast<SDL_Window* >(_window), &width, &height);

	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	_default_material = new ga_constant_color_material();
	_default_material->init();
}

ga_output::~ga_output()
{
	delete _default_material;
}

void ga_output::update(ga_frame_params* params)
{
	// Update viewport in case window was resized:
	int width, height;
	SDL_GetWindowSize(static_cast<SDL_Window*>(_window), &width, &height);
	glViewport(0, 0, width, height);

	// Clear viewport:
	//glEnable(GL_LIGHTING);
	//glDepthMask(GL_TRUE);
	//glClearColor(0.0, 0.0, 0.0, 1.0);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Compute projection matrices:
	ga_mat4f perspective;
	perspective.make_perspective_rh(ga_degrees_to_radians(45.0f), (float)width / (float)height, 0.1f, 10000.0f);
	ga_mat4f view_perspective = params->_view * perspective;

	ga_mat4f ortho;
	ortho.make_orthographic(0.0f, (float)width, (float)height, 0.0f, 0.1f, 10000.0f);
	ga_mat4f view;
	view.make_lookat_rh(ga_vec3f::z_vector(), -ga_vec3f::z_vector(), ga_vec3f::y_vector());
	ga_mat4f view_ortho = view * ortho;

	// Draw all static geometry:

	glEnable(GL_DEPTH_TEST); // standard depth testing
	glDepthFunc(GL_LEQUAL);
	glDepthMask(1);
	glDisable(GL_STENCIL_TEST); // no stencil testing (this pass)
	glColorMask(1, 1, 1, 1); // update color buffer
	glClearStencil(0); // clear stencil to zero
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	for (auto& d : params->_static_drawcalls)
	{	
		d._material->set_lit(true); // FINAL : Jacy scharlow

		d._material->bind(view_perspective, d._transform);
		glBindVertexArray(d._vao);
		glDrawElements(d._draw_mode, d._index_count, GL_UNSIGNED_SHORT, 0);
	}

	//***FINAL draw shadow volumes 

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(1); // do not disturb depth buffer
	glColorMask(0, 0, 0, 0); // do not disturb color buffer
	glStencilMask(~0u);
	glEnable(GL_CULL_FACE); // use face culling
	glCullFace(GL_BACK); // increment for front facing fragments
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR); // that pass the depth test
	for (auto& d : params->_shadow_drawcalls)
	{
		d._material->bind(view_perspective, d._transform);
		glBindVertexArray(d._vao);
		glDrawElements(d._draw_mode, d._index_count, GL_UNSIGNED_SHORT, 0);
	}
	glCullFace(GL_FRONT); // decrement for back facing fragments
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR); // that pass the depth test
	for (auto& d : params->_shadow_drawcalls)
	{
		d._material->bind(view_perspective, d._transform);
		glBindVertexArray(d._vao);
		glDrawElements(d._draw_mode, d._index_count, GL_UNSIGNED_SHORT, 0);
	}

	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_EQUAL); // must match depth from 1st step
	glDepthMask(0);
	glEnable(GL_STENCIL_TEST); // and use stencil to update only
	glStencilFunc(GL_EQUAL, 0x1, 0x1); // pixels tagged as
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); // “in the shadow volume”
	glColorMask(1, 1, 1, 1);

	for (auto& d : params->_static_drawcalls)
	{
		d._material->set_lit(false); // FINAL : Jacy scharlow

		d._material->bind(view_perspective, d._transform);
		glBindVertexArray(d._vao);
		glDrawElements(d._draw_mode, d._index_count, GL_UNSIGNED_SHORT, 0);
	}

	//glDisable(GL_STENCIL_TEST);

	// Draw all dynamic geometry:
	draw_dynamic(params->_dynamic_drawcalls, view_perspective);
	draw_dynamic(params->_gui_drawcalls, view_ortho);

	GLenum error = glGetError();
	assert(error == GL_NONE);

	// Swap frame buffers:
	SDL_GL_SwapWindow(static_cast<SDL_Window*>(_window));
}

void ga_output::draw_dynamic(const std::vector<ga_dynamic_drawcall>& drawcalls, const ga_mat4f& view_proj)
{
	for (auto& d : drawcalls)
	{
		if (d._material)
		{
			d._material->set_color(d._color);
			d._material->bind(view_proj, d._transform);
		}
		else
		{
			_default_material->set_color(d._color);
			_default_material->bind(view_proj, d._transform);
		}

		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		GLuint pos;
		glGenBuffers(1, &pos);
		glBindBuffer(GL_ARRAY_BUFFER, pos);
		glBufferData(GL_ARRAY_BUFFER, sizeof(ga_vec3f) * d._positions.size(), &d._positions[0], GL_STREAM_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		GLuint texcoord;
		if (!d._texcoords.empty())
		{
			glGenBuffers(1, &texcoord);
			glBindBuffer(GL_ARRAY_BUFFER, texcoord);
			glBufferData(GL_ARRAY_BUFFER, sizeof(ga_vec2f) * d._texcoords.size(), &d._texcoords[0], GL_STREAM_DRAW);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
		}

		GLuint indices;
		glGenBuffers(1, &indices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * d._indices.size(), &d._indices[0], GL_STREAM_DRAW);

		glDrawElements(d._draw_mode, (GLsizei)d._indices.size(), GL_UNSIGNED_SHORT, 0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDeleteBuffers(1, &indices);
		if (!d._texcoords.empty())
		{
			glDeleteBuffers(1, &texcoord);
		}
		glDeleteBuffers(1, &pos);
		glDeleteVertexArrays(1, &vao);
		glBindVertexArray(0);
	}
}
