#version 330 core

out vec4 outColor;
in vec3 vN;

//uniform mat4 normal;



void main()
{
	//outColor = vec4(vN, 1);   
	//outColor = normal * vec4(vN,0);
	outColor = vN.rgbb;
}
