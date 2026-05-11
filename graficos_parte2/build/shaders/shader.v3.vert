#version 330 core
// Paso 3: Phong + Texturas (color albedo, emisivo, especular)

layout(location = 0) in vec3 inPos;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;

uniform mat4 modelView;
uniform mat4 modelViewProj;
uniform mat4 normal;

out vec3 fragPosCam;
out vec3 fragNormCam;
out vec2 fragTexCoord;

void main() {
    vec4 posCam = modelView * vec4(inPos, 1.0);
    fragPosCam  = posCam.xyz;
    fragNormCam = mat3(normal) * inNormal;
    fragTexCoord = inTexCoord;

    gl_Position = modelViewProj * vec4(inPos, 1.0);
}
