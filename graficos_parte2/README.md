# Práctica 2: Iluminación en GLSL
**Gráficos por Computador – URJC 2024/25**

## Dependencias (Linux)

```bash
sudo apt install libglfw3-dev libglew-dev libglm-dev libstb-dev cmake build-essential
```

## Compilar y ejecutar

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel
./P2GLSL
```

O desde VS Code: `Ctrl+Shift+B` para compilar, luego `F5` para depurar.

> **Nota:** el ejecutable debe correrse desde dentro de `build/`  
> (los shaders y texturas se copian ahí automáticamente al compilar).

---

## Controles de teclado

| Tecla | Acción |
|-------|--------|
| `0`   | Shader v0 – color por vértice (sin iluminación) |
| `1`   | Shader v1 – **Sombreado de Gouraud** |
| `2`   | Shader v2 – **Sombreado de Phong** |
| `3`   | Shader v3 – **Phong + Texturas** (albedo, emisivo, especular) |
| `4`   | Shader v4 – **2 luces puntuales** (tarea obligatoria 2) |
| `5`   | Shader v5 – **Atenuación por distancia** (tarea obligatoria 3) |
| `ESC` | Salir |

**Cámara:**
- Botón izquierdo del ratón + arrastrar → orbitar
- Rueda del ratón → zoom

---

## Estructura del proyecto

```
P2GLSL/
├── CMakeLists.txt
├── .vscode/
│   ├── settings.json         ← CMake Tools config
│   ├── tasks.json            ← Ctrl+Shift+B para compilar
│   ├── launch.json           ← F5 para depurar con gdb
│   └── c_cpp_properties.json ← IntelliSense
├── src/
│   ├── main.cpp              ← Aplicación principal (OpenGL/GLFW)
│   ├── BOX.h                 ← Geometría del cubo
│   └── stb_image.h           ← Carga de texturas
├── shaders/
│   ├── shader.v0.*           ← Color por vértice, sin iluminación
│   ├── shader.v1.*           ← Sombreado de Gouraud (iluminación en vertex)
│   ├── shader.v2.*           ← Sombreado de Phong (iluminación en fragment)
│   ├── shader.v3.*           ← Phong + texturas (albedo, emisivo, especular)
│   ├── shader.v4.*           ← 2 luces puntuales sin atenuación
│   └── shader.v5.*           ← Atenuación por distancia, luz ligada a cámara
└── img/
    ├── color.png             ← Textura albedo (v1/v2)
    ├── color2.png            ← Textura albedo alternativa (v3)
    ├── emissive.png          ← Mapa emisivo
    ├── specMap.png           ← Mapa especular
    └── normal.png            ← Mapa de normales (para bump mapping opcional)
```

---

## Descripción de los shaders

### v0 – Sin iluminación
El color del fragmento es directamente el color por vértice interpolado. Punto de partida.

### v1 – Sombreado de Gouraud (Paso 1 guiado)
El modelo de iluminación de Phong (ambiental + difuso + especular) se evalúa **por vértice** en el vertex shader. El color resultante se interpola entre fragmentos (interpolación de Gouraud). Produce artefactos en los bordes de brillos especulares.

### v2 – Sombreado de Phong (Paso 2 guiado)
La posición y la normal se interpolan al fragment shader, donde se evalúa la iluminación **por fragmento**. Resultado mucho más suave y correcto que Gouraud.

### v3 – Phong + Texturas (Paso 3 guiado)
- `colorTex` (color2.png) como albedo difuso (Kd)
- `emiTex` (emissive.png) como componente emisiva
- `specularTex` (specMap.png) como mapa de intensidad especular (Ks)

### v4 – 2 Luces Puntuales (Tarea obligatoria 2)
Dos fuentes de luz puntuales **en coordenadas del mundo** (no ligadas a la cámara), sin atenuación por distancia. Una luz blanca-fría y otra anaranjada. Se puede orbitar la cámara con el ratón para verificar que las luces permanecen fijas en el mundo.

### v5 – Atenuación por Distancia (Tarea obligatoria 3)
- **Luz 0:** ligada a la cámara, situada en el origen del espacio cámara `(0,0,0)`. Simula una antorcha. Usa atenuación cuadrática: `att = 1 / (kc + kl·d + kq·d²)`.
- **Luz 1:** puntual fija en el mundo, sin atenuación.

---

## Extensiones recomendadas para VS Code

- `ms-vscode.cmake-tools` – Integración CMake
- `ms-vscode.cpptools` – IntelliSense C++
- `slevesque.shader` – Resaltado de sintaxis GLSL
