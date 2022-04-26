/*
** RPI Game Architecture Engine
** FINAL
**
** By: Jacy Scharlow
*/

#include "f_render.h"

#include "framework/ga_frame_params.h"

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

f_render::f_render(void* win) : _window(win) {

  int width, height;
  SDL_GetWindowSize(static_cast<SDL_Window*>(_window), &width, &height);

  glViewport(0, 0, width, height);

  glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glDisable(GL_CULL_FACE);

}

void f_render::draw() {

  

}

void f_render::update(ga_frame_params* params) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //glUseProgram(programID);

  int width, height;
  SDL_GetWindowSize(static_cast<SDL_Window*>(_window), &width, &height);
  glViewport(0, 0, width, height);

  ga_mat4f perspective;
  perspective.make_perspective_rh(ga_degrees_to_radians(45.0f), (float)width / (float)height, 0.1f, 10000.0f);
  ga_mat4f view_perspective = params->_view * perspective;

  ga_mat4f ortho;
  ortho.make_orthographic(0.0f, (float)width, (float)height, 0.0f, 0.1f, 10000.0f);
  ga_mat4f view;
  view.make_lookat_rh(ga_vec3f::z_vector(), -ga_vec3f::z_vector(), ga_vec3f::y_vector());
  ga_mat4f view_ortho = view * ortho;

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  if (!shadow) {
		for (auto& d : params->_static_drawcalls)
		{
			d._material->set_lit(true); // FINAL : Jacy scharlow

			d._material->bind(view_perspective, d._transform);
			glBindVertexArray(d._vao);
			glDrawElements(d._draw_mode, d._index_count, GL_UNSIGNED_SHORT, 0);
		}
  }
  else if (shadow) {
		/*glEnable(GL_DEPTH_TEST); // standard depth testing
		glDepthFunc(GL_LEQUAL);
		glDepthMask(0);
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
		//glStencilMask(~0u);
		glStencilMask(0xFF);
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

		//const int pixels = width * height;
		//GLint* prms = new GLint[width * height];
		//glGetIntegerv(GL_STENCIL_BITS, prms);
		//
		//for (int n = 0; n < (width * height); n++)
		//	std::cout << prms[n] << std::endl;

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_EQUAL); // must match depth from 1st step
		glDepthMask(0);
		glEnable(GL_STENCIL_TEST); // and use stencil to update only
		glStencilFunc(GL_EQUAL, 0, 0xFF); // pixels tagged as
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); // “in the shadow volume”
		glColorMask(1, 1, 1, 1);
		
		for (auto& d : params->_static_drawcalls)
		{
			d._material->set_lit(false); // FINAL : Jacy scharlow
		
			d._material->bind(view_perspective, d._transform);
			glBindVertexArray(d._vao);
			glDrawElements(d._draw_mode, d._index_count, GL_UNSIGNED_SHORT, 0);
		}*/

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
  }

	SDL_GL_SwapWindow(static_cast<SDL_Window*>(_window));
}