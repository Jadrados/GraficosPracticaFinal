#version 330 core

in vec3 inPos;
in vec3 inNormal;
in vec3 inColor;

uniform mat4 modelViewProj;
uniform mat4 modelView;
uniform mat4 normal;

out vec3 vp;
out vec3 vN;
out vec3 vColor;

void main() {
    // Posición en espacio de cámara
    vp = (modelView * vec4(inPos, 1.0)).xyz;
    
    // Normal transformada a espacio de cámara
    vN = (normal * vec4(inNormal, 0.0)).xyz;
    
    // Color original del cubo (para cada vértice)
    vColor = inColor;
    
    // Posición final en pantalla
    gl_Position = modelViewProj * vec4(inPos, 1.0);
}