#version 330 core
// Tarea obligatoria 2: 2 fuentes de luz puntuales sin atenuacion
// Las luces se pasan en coordenadas del mundo, sin ligar a la camara.

layout(location = 0) in vec3 inPos;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 modelView;
uniform mat4 modelViewProj;
uniform mat4 normal;

out vec3 fragPosCam;
out vec3 fragNormCam;
out vec2 fragTexCoord;

// Posiciones de las 2 luces en espacio mundo (se transforman a cam aqui)
uniform vec3 lightWorldPos0;
uniform vec3 lightWorldPos1;

out vec3 fragLightPosCam0;
out vec3 fragLightPosCam1;

void main() {
    vec4 posCam = modelView * vec4(inPos, 1.0);
    fragPosCam   = posCam.xyz;
    fragNormCam  = mat3(normal) * inNormal;
    fragTexCoord = inTexCoord;

    // Transformar posiciones de luz de mundo a camara
    fragLightPosCam0 = (view * vec4(lightWorldPos0, 1.0)).xyz;
    fragLightPosCam1 = (view * vec4(lightWorldPos1, 1.0)).xyz;

    gl_Position = modelViewProj * vec4(inPos, 1.0);
}
