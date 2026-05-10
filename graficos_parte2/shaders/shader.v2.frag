#version 330 core
// Paso 2: Sombreado de Phong
// La iluminacion se calcula por fragmento usando la normal interpolada.

in vec3 fragPosCam;
in vec3 fragNormCam;
in vec3 fragColor;

out vec4 outColor;

void main() {
    // --- Material (hardcoded) ---
    vec3 Ka = vec3(0.2, 0.2, 0.2);
    vec3 Kd = fragColor;
    vec3 Ks = vec3(0.8, 0.8, 0.8);
    float shininess = 32.0;

    // --- Luz puntual (hardcoded en espacio camara) ---
    vec3 lightPosCam = vec3(3.0, 4.0, 3.0);
    vec3 Ia = vec3(0.3, 0.3, 0.3);
    vec3 Id = vec3(0.8, 0.8, 0.8);
    vec3 Is = vec3(1.0, 1.0, 1.0);

    vec3 N = normalize(fragNormCam);
    vec3 L = normalize(lightPosCam - fragPosCam);
    vec3 V = normalize(-fragPosCam);
    vec3 R = reflect(-L, N);

    vec3 ambient  = Ka * Ia;
    vec3 diffuse  = Kd * Id * max(dot(N, L), 0.0);
    vec3 specular = Ks * Is * pow(max(dot(R, V), 0.0), shininess);

    outColor = vec4(ambient + diffuse + specular, 1.0);
}
