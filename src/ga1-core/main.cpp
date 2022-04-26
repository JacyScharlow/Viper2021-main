/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "framework/ga_camera.h"
#include "framework/ga_compiler_defines.h"
#include "framework/ga_input.h"
#include "framework/ga_sim.h"
#include "framework/ga_output.h"
#include "jobs/ga_job.h"

#include "entity/ga_entity.h"
#include "entity/ga_lua_component.h"

#include "graphics/ga_model_component.h" //*** Final: Added -Jacy Scharlow
#include "graphics/f_obj_parser.h" //*** Final: Added -Jacy Scharlow
#include "graphics/ga_geometry.h" //*** Final: Added -Jacy Scharlow
#include "graphics/ga_egg_parser.h" //*** Final: Added -Jacy Scharlow
#include "graphics/f_mesh.h" //*** Final: Added -Jacy Scharlow
#include "graphics/f_render.h" //*** Final: Added -Jacy Scharlow
#include "graphics/ga_cube_component.h"
#include "graphics/ga_program.h"

#include "gui/ga_font.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#include <assimp/Importer.hpp>      // *** Final: Added -Jacy Scharlow C++ importer interface
#include <assimp/postprocess.h>     // *** Final: Added -Jacy Scharlow Post processing flags

#if defined(GA_MINGW)
#include <unistd.h>
#endif

ga_font* g_font = nullptr;
static void set_root_path(const char* exepath);


//***Final: Added obj object references -Jacy Scharlow
//====================================================
ga_entity statue;
ga_entity scene;
ga_entity volume;
//=================================================

int main(int argc, const char** argv)
{
	set_root_path(argv[0]);

	ga_job::startup(0xffff, 256, 256);

	// Create objects for three phases of the frame: input, sim and output.
	ga_input* input = new ga_input();
	ga_sim* sim = new ga_sim();
	//ga_output* output = new ga_output(input->get_window());
	f_render* render = new f_render(input->get_window());

	// Create the default font:
	g_font = new ga_font("VeraMono.ttf", 16.0f, 512, 512);

	// Create camera.
	//ga_camera* camera = new ga_camera({ 0.0f, 7.0f, 20.0f });
	ga_camera* camera = new ga_camera({ 0.0f, 20.0f, 40.0f });
	//ga_camera* camera = new ga_camera({ 0.0f, 50.0f, 0.0f });
	ga_quatf rotation;
	rotation.make_axis_angle(ga_vec3f::y_vector(), ga_degrees_to_radians(180.0f));
	camera->rotate(rotation);
	rotation.make_axis_angle(ga_vec3f::x_vector(), ga_degrees_to_radians(15.0f));
	camera->rotate(rotation);

	//***Final:Scene Rendering -Jacy Scharlow
	ga_model scene_model;
	Mesh *scene_mesh = new Mesh();
	obj_to_model("data/models/soft_col.obj", &scene_model, scene_mesh);

	ga_model statue_model;
	Mesh* statue_mesh = new Mesh();
	obj_to_model("data/models/uvv.obj", &statue_model, statue_mesh);

	std::vector<ga_vec3f> quads;
	scene_mesh->packShadowVolumes( &quads );
	statue_mesh->packShadowVolumes( &quads );
	
	//***********************************************
	//********CONSTRUCTION***************************

	ga_model_component scene_comp(&scene, &scene_model);
	sim->add_entity(&scene);
	
	ga_model_component model_comp(&statue, &statue_model);
	sim->add_entity(&statue);
	
	ga_cube_component stencil_comp(&volume, quads);
	sim->add_entity(&volume);
	
	// Main loop:
	while (true)
	{
		// We pass frame state through the 3 phases using a params object.
		ga_frame_params params;

		// Gather user input and current time.
		if (!input->update(&params))
		{
			break;
		}
		
		// Update the camera.
		camera->update(&params);

		// Run gameplay.
		sim->update(&params);

		// Perform the late update.
		sim->late_update(&params);

		// Draw to screen.
		// output->update(&params);
		render->shadow = true;
		render->update(&params);

	}

	//delete output;
	delete sim;
	delete input;
	delete camera;

	delete scene_mesh;
	delete statue_mesh;

	ga_job::shutdown();

	return 0;
}

char g_root_path[256];
static void set_root_path(const char* exepath)
{
#if defined(GA_MSVC)
	strcpy_s(g_root_path, sizeof(g_root_path), exepath);

	// Strip the executable file name off the end of the path:
	char* slash = strrchr(g_root_path, '\\');
	if (!slash)
	{
		slash = strrchr(g_root_path, '/');
	}
	if (slash)
	{
		slash[1] = '\0';
	}
#elif defined(GA_MINGW)
	char* cwd;
	char buf[PATH_MAX + 1];
	cwd = getcwd(buf, PATH_MAX + 1);
	strcpy_s(g_root_path, sizeof(g_root_path), cwd);

	g_root_path[strlen(cwd)] = '/';
	g_root_path[strlen(cwd) + 1] = '\0';
#endif
}
