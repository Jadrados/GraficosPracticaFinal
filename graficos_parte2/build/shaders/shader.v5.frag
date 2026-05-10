#version 330 core
// Tarea obligatoria 3: Atenuacion por distancia.
// Luz 0: ligada a la camara en (0,0,0) espacio camara -> antorcha.
// Luz 1: puntual en mundo sin atenuacion (referencia).
//
// Funcion de atenuacion: att = 1 / (kc + kl*d + kq*d^2)

in vec3 fragPosCam;
in vec3 fragNormCam;
in vec2 fragTexCoord;
in vec3 fragLightPosCam1;

uniform sampler2D colorTex;
uniform sampler2D specularTex;

out vec4 outColor;

// Atenuacion cuadratica
float attenuate(float dist) {
    float kc = 1.0;
    float kl = 0.09;
    float kq = 0.032;
    return 1.0 / (kc + kl * dist + kq * dist * dist);
}

vec3 phongContrib(vec3 N, vec3 V, vec3 fragPos, vec3 lightPos,
                  vec3 Kd, vec3 Ks, float shininess,
                  vec3 Id, vec3 Is, bool useAttenuation) {
    vec3 toLight = lightPos - fragPos;
    float dist = length(toLight);
    vec3 L = toLight / dist;
    vec3 R = reflect(-L, N);

    float att = useAttenuation ? attenuate(dist) : 1.0;

    vec3 diffuse  = att * Kd * Id * max(dot(N, L), 0.0);
    vec3 specular = att * Ks * Is * pow(max(dot(R, V), 0.0), shininess);
    return diffuse + specular;
}

void main() {
    vec3 Kd = texture(colorTex, fragTexCoord).rgb;
    vec3 Ks = texture(specularTex, fragTexCoord).rgb;
    float shininess = 64.0;

    vec3 N = normalize(fragNormCam);
    vec3 V = normalize(-fragPosCam);

    // Ambiental global
    vec3 ambient = Kd * 0.1;

    // Luz 0: en el ojo (0,0,0 espacio camara), CON atenuacion, color blanca-calida
    vec3 contrib0 = phongContrib(N, V, fragPosCam, vec3(0.0),
                                 Kd, Ks, shininess,
                                 vec3(1.0, 0.9, 0.7), vec3(1.0, 0.9, 0.7),
                                 true);

    // Luz 1: puntual en mundo, SIN atenuacion, color fria
    vec3 contrib1 = phongContrib(N, V, fragPosCam, fragLightPosCam1,
                                 Kd, Ks, shininess,
                                 vec3(0.4, 0.5, 1.0), vec3(1.0),
                                 false);

    outColor = vec4(ambient + contrib0 + contrib1, 1.0);
}
