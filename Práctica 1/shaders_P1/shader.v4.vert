#version 330 core

in vec3 inPos;	
in vec2 inTexCoord;	


uniform mat4 modelViewProj;

out vec2 vtc;

void main()
{
	vtc = inTexCoord;
	gl_Position =  modelViewProj * vec4(inPos,1.);
}
