#version 330 core
// Tarea obligatoria 3: Atenuacion por distancia.
// Una luz puntual ligada a la camara en (0,0,0,1) espacio camara.
// Otra luz puntual en coordenadas mundo (no ligada).

layout(location = 0) in vec3 inPos;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;

uniform mat4 view;
uniform mat4 modelView;
uniform mat4 modelViewProj;
uniform mat4 normal;

// Luz 0: ligada a la camara, origen del espacio camara
// Luz 1: en coordenadas mundo
uniform vec3 lightWorldPos1;

out vec3 fragPosCam;
out vec3 fragNormCam;
out vec2 fragTexCoord;
out vec3 fragLightPosCam1;

void main() {
    vec4 posCam = modelView * vec4(inPos, 1.0);
    fragPosCam   = posCam.xyz;
    fragNormCam  = mat3(normal) * inNormal;
    fragTexCoord = inTexCoord;

    // Luz 1 en espacio camara
    fragLightPosCam1 = (view * vec4(lightWorldPos1, 1.0)).xyz;

    gl_Position = modelViewProj * vec4(inPos, 1.0);
}
