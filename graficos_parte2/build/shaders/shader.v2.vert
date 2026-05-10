#version 330 core
// Paso 2: Sombreado de Phong
// Se interpolan posicion y normal al fragment shader,
// donde se calcula la iluminacion por fragmento.

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;

uniform mat4 modelView;
uniform mat4 modelViewProj;
uniform mat4 normal; // transpose(inverse(modelView))

out vec3 fragPosCam;
out vec3 fragNormCam;
out vec3 fragColor;

void main() {
    vec4 posCam = modelView * vec4(inPos, 1.0);
    fragPosCam  = posCam.xyz;
    fragNormCam = mat3(normal) * inNormal;
    fragColor   = inColor;

    gl_Position = modelViewProj * vec4(inPos, 1.0);
}
