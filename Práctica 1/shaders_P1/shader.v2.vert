#version 330 core

in vec3 inPos;		

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
uniform mat4 modelViewProj;

out vec3 vcolor;

void main()
{
	
	vec2 bw = vec2 (0,1);
	vcolor = (mod(gl_VertexID,2)==0) ? bw.rrg : bw.ggg;

	gl_Position =  modelViewProj * vec4(inPos,1.);
}
