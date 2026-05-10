#version 330 core

in vec3 inPos;		

mat4 view;
mat4 proj;

void main()
{
	view = mat4(1);
	view[3].z = -6.;

	float n = 1.;
	float f = 10.;
	float inv_t30 = 1.73205080757;
	
	proj = mat4(0);
	proj[0].x = inv_t30;
	proj[1].y = inv_t30;
	proj[2].z = (n+f) / (n-f);
	proj[3].z = 2.*f*n / (n-f);
	proj[2][3] = -1;


	gl_Position =  proj * view * vec4(inPos,1.);
}
