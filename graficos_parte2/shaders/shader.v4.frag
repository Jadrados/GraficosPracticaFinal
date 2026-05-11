#version 330 core
// Tarea obligatoria 2: 2 fuentes de luz puntuales sin atenuacion por distancia.
// Ambas luces estan en coordenadas del mundo (no ligadas a la camara).

in vec3 fragPosCam;
in vec3 fragNormCam;
in vec2 fragTexCoord;
in vec3 fragLightPosCam0;
in vec3 fragLightPosCam1;

uniform sampler2D colorTex;
uniform sampler2D specularTex;

out vec4 outColor;

// Calcula la contribucion de Phong de una luz puntual (sin atenuacion)
vec3 phongLight(vec3 N, vec3 V, vec3 L,
                vec3 Kd, vec3 Ks, float shininess,
                vec3 Ia, vec3 Id, vec3 Is) {
    vec3 R = reflect(-L, N);
    vec3 ambient  = Kd * 0.15 * Ia;
    vec3 diffuse  = Kd * Id * max(dot(N, L), 0.0);
    vec3 specular = Ks * Is * pow(max(dot(R, V), 0.0), shininess);
    return ambient + diffuse + specular;
}

void main() {
    // Material desde texturas
    vec3 Kd = texture(colorTex, fragTexCoord).rgb;
    vec3 Ks = texture(specularTex, fragTexCoord).rgb;
    float shininess = 64.0;

    vec3 N = normalize(fragNormCam);
    vec3 V = normalize(-fragPosCam);

    // Luz 0: blanca, posicion (3,4,3) en mundo -> cam
    vec3 L0 = normalize(fragLightPosCam0 - fragPosCam);
    vec3 contrib0 = phongLight(N, V, L0, Kd, Ks, shininess,
                               vec3(1.0), vec3(0.8,0.8,1.0), vec3(1.0));

    // Luz 1: anaranjada, posicion (-3,2,-3) en mundo -> cam
    vec3 L1 = normalize(fragLightPosCam1 - fragPosCam);
    vec3 contrib1 = phongLight(N, V, L1, Kd, Ks, shininess,
                               vec3(1.0), vec3(1.0,0.6,0.2), vec3(1.0,0.6,0.2));

    outColor = vec4(contrib0 + contrib1, 1.0);
}
