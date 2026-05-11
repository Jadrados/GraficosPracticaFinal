#version 330 core

in vec3 inPos;		

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
uniform mat4 modelViewProj;

void main()
{
	//gl_Position =  proj * view * model * vec4(inPos,1.);
	gl_Position =  modelViewProj * vec4(inPos,1.);
}
