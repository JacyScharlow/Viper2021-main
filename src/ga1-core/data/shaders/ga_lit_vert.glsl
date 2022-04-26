#version 400
layout (location = 0) in vec3 in_vertex;
layout (location = 1) in vec3 in_normal;
layout (location = 3) in vec2 in_texcood0;

out vec2 fragmentUV;
out vec3 fragmentNormal;

uniform mat4 u_mvp;
uniform mat4 u_m;
uniform mat4 u_vp;

// TODO: Homework 3 - Add any other necessary uniforms or input/output variables here

void main()
{
	// TODO: Homework 3
	fragmentUV = in_texcood0;
	//fragmentNormal = mat3(transpose(inverse(u_m))) * in_normal;
	fragmentNormal = (u_mvp * vec4(in_normal, 0.0)).xyz;
	gl_Position = vec4(in_vertex, 1.0) * u_mvp;

}