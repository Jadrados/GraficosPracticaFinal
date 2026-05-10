#version 330 core
// Paso 1: Sombreado de Gouraud
// La iluminacion se calcula por vertice (en el vertex shader)
// y se interpola entre fragmentos.

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;

uniform mat4 modelView;
uniform mat4 modelViewProj;
uniform mat4 normal; // normal matrix = transpose(inverse(modelView))

out vec3 outColor;

void main() {
    // --- Propiedades del material (hardcoded) ---
    vec3 Ka = vec3(0.2, 0.2, 0.2); // ambiental
    vec3 Kd = inColor;              // difuso = color del vertice
    vec3 Ks = vec3(0.8, 0.8, 0.8); // especular
    float shininess = 32.0;

    // --- Propiedades de la luz puntual (hardcoded, en coords mundo) ---
    vec3 lightPosWorld = vec3(3.0, 4.0, 3.0);
    vec3 Ia = vec3(0.3, 0.3, 0.3);
    vec3 Id = vec3(0.8, 0.8, 0.8);
    vec3 Is = vec3(1.0, 1.0, 1.0);

    // --- Calculo en espacio de camara ---
    vec4 posCam  = modelView * vec4(inPos, 1.0);
    vec3 normCam = normalize(mat3(normal) * inNormal);

    // Vector hacia la luz (la luz se pasa en coords mundo; convertimos a cam)
    // Para simplificar la practica, usamos la posicion de luz en espacio camara
    // asumiendo que view es identidad (hardcoded)
    vec3 lightPosCam = vec3(3.0, 4.0, 3.0); // simplificacion guiada
    vec3 L = normalize(lightPosCam - posCam.xyz);
    vec3 V = normalize(-posCam.xyz);         // vector hacia camara
    vec3 R = reflect(-L, normCam);           // reflejo de L

    // --- Componentes de iluminacion de Phong ---
    vec3 ambient  = Ka * Ia;
    vec3 diffuse  = Kd * Id * max(dot(normCam, L), 0.0);
    vec3 specular = Ks * Is * pow(max(dot(R, V), 0.0), shininess);

    outColor = ambient + diffuse + specular;

    gl_Position = modelViewProj * vec4(inPos, 1.0);
}
