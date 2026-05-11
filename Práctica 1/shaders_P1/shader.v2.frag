#version 330 core

out vec4 outColor;

in vec3 vcolor;

void main()
{
	outColor = vec4(vcolor,0);   
}
