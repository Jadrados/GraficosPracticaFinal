#version 330 core
// Paso 3: Phong + Texturas
// - colorTex:   albedo del material (Kd)
// - emiTex:     componente emisiva
// - specularTex: intensidad especular del material (Ks)

in vec3 fragPosCam;
in vec3 fragNormCam;
in vec2 fragTexCoord;

uniform sampler2D colorTex;
uniform sampler2D emiTex;
uniform sampler2D specularTex;

out vec4 outColor;

void main() {
    // --- Material desde texturas ---
    vec3 Kd = texture(colorTex, fragTexCoord).rgb;
    vec3 Ks = texture(specularTex, fragTexCoord).rgb;
    vec3 Ka = Kd * 0.2;
    float shininess = 64.0;

    // --- Luz puntual (hardcoded en espacio camara) ---
    vec3 lightPosCam = vec3(3.0, 4.0, 3.0);
    vec3 Ia = vec3(0.3, 0.3, 0.3);
    vec3 Id = vec3(0.9, 0.9, 0.9);
    vec3 Is = vec3(1.0, 1.0, 1.0);

    vec3 N = normalize(fragNormCam);
    vec3 L = normalize(lightPosCam - fragPosCam);
    vec3 V = normalize(-fragPosCam);
    vec3 R = reflect(-L, N);

    vec3 ambient  = Ka * Ia;
    vec3 diffuse  = Kd * Id * max(dot(N, L), 0.0);
    vec3 specular = Ks * Is * pow(max(dot(R, V), 0.0), shininess);

    // --- Componente emisiva ---
    vec3 emissive = texture(emiTex, fragTexCoord).rgb;

    outColor = vec4(ambient + diffuse + specular + emissive, 1.0);
}
