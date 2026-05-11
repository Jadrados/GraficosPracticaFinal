#version 330 core

out vec4 outColor;
in vec3 vN;
in vec3 vP;

//uniform mat4 normal;


vec3 F0 = vec3(0.0);
float FPower = 2.0; 
vec3 fresnelSchlick(in vec3 N, in vec3 V, in vec3 F0, in float FPower)
{
  return F0 + (vec3(1.0) - F0) * pow(1.0 - max(dot(V, N), 0), FPower);
}


void main()
{
	//outColor = vec4(vN, 1);   
	//outColor = normal * vec4(vN,0);
	vec3 V = normalize(-vP);

	float a = fresnelSchlick(vN, V, F0, FPower).r;

	outColor = ((1- a) * vec4(0,0,1,1)) + (a* vec4(1.0, 0.5, 0.5,1));
}
