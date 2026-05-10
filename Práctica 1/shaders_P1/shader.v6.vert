#version 330 core

in vec3 inPos;	
in vec2 inTexCoord;
in vec3 inNormal;


uniform mat4 modelViewProj;
uniform mat4 modelView;
uniform mat4 normal;

out vec3 vN;
out vec3 vP;

void main()
{
	vN = (normal * vec4(inNormal,0)).xyz;
	vP = (modelView * vec4 (vP, 1)).xyz;
	gl_Position =  modelViewProj * vec4(inPos,1.);
}
