#version 330 core

out vec4 outColor;
in vec3 vN;
in vec3 vp;
in vec3 vColor;


vec3 Ks = vec3(1.0, 1.0, 1.0);  // Brillo blanco
float n = 32.0;                  // Exponente de brillo


// Luz Blanca - desde arriba y derecha
vec3 lightPos1 = vec3(3.0, 3.0, 2.0); 
vec3 Ia1 = vec3(0.15, 0.15, 0.15);  // Ambiental
vec3 Id1 = vec3(0.8, 0.8, 0.8);     // Difusa
vec3 Is1 = vec3(0.5, 0.5, 0.5);     // Especular

// Luz Verde suave - desde abajo y izquierda
vec3 lightPos2 = vec3(-2.0, -1.0, 3.0); 
vec3 Ia2 = vec3(0.0, 0.08, 0.0);  // Ambiental
vec3 Id2 = vec3(0.0, 0.3, 0.0);     // Difusa
vec3 Is2 = vec3(0.0, 0.2, 0.0);     // Especular


vec3 calculateLight(vec3 lightPos, vec3 Ia, vec3 Id, vec3 Is, 
                 vec3 N, vec3 p, vec3 colorObj) {
    
    // Dirección a la luz (se normaliza la distancia del pixel a la posición de luz)
    vec3 L = normalize(lightPos - p);
    
    // Dirección a la cámara (en espacio cámara, la cámara está en el origen)
    vec3 V = normalize(-p);
    
    // Vector de reflexión (para el brillo especular)
    vec3 R = reflect(-L, N);
    
    // Componente difusa: depende del ángulo entre normal y luz
    float diff = max(dot(N, L), 0.0);
    
    // Componente especular: depende de si vemos el reflejo
    float spec = pow(max(dot(R, V), 0.0), n);
    
    // Calcular los 3 componentes
    vec3 ambient = Ia * colorObj;
    vec3 diffuse = Id * colorObj * diff;
    vec3 specular = Is * Ks * spec;
    
    return ambient + diffuse + specular;
}


void main() {
    // Normalizar la normal (puede que la interpolación la haya desviado)
    vec3 N = normalize(vN);
    
    //Se obtiene la posición del pixel (Vp es posicion del pixel ante la camara)
    vec3 p = vp;
    
    // Color del objeto en este píxel (azul, rojo, verde... según la cara)
    vec3 colorObjeto = vColor;
    
    // Sumar contribución de ambas luces
    vec3 color = vec3(0.0);
    color += calculateLight(lightPos1, Ia1, Id1, Is1, N, p, colorObjeto);
    color += calculateLight(lightPos2, Ia2, Id2, Is2, N, p, colorObjeto);
    
    // Asignar el color final al píxel
    outColor = vec4(color, 1.0);
}