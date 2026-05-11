#version 330 core
// Paso 1: Sombreado de Gouraud
// El color ya viene calculado del vertex shader, solo se interpola.

in vec3 outColor;
out vec4 fragColor;

void main() {
    fragColor = vec4(outColor, 1.0);
}
