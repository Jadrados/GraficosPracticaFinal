/*
 * Práctica 2: Iluminación en GLSL
 * Gráficos por Computador - URJC 2024/25
 *
 * Selección de tarea mediante teclas:
 *   0  -> shader.v0  (color por vértice, sin iluminación)
 *   1  -> shader.v1  (Gouraud shading)
 *   2  -> shader.v2  (Phong shading)
 *   3  -> shader.v3  (Phong + texturas + emisivo + especular)
 *   4  -> shader.v4  (Phong + 2 luces puntuales, tarea obligatoria 2)
 *   5  -> shader.v5  (Phong + atenuación por distancia, tarea obligatoria 3)
 *
 * Cámara (modo arrastrar ratón):
 *   Botón izq + arrastrar -> orbitar
 *   Rueda                 -> zoom
 */

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "BOX.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>

// ============================================================
// Globals
// ============================================================
GLFWwindow* gWindow = nullptr;
int gWidth = 800, gHeight = 600;

// Active shader index (0-5)
int gActiveShader = 3;

// Per-shader program IDs
GLuint gPrograms[6] = {};

// VAO / VBO / EBO
GLuint gVAO, gVBO_pos, gVBO_col, gVBO_norm, gVBO_tex, gVBO_tan, gEBO;

// Textures
GLuint gTexColor  = 0;
GLuint gTexColor2 = 0;
GLuint gTexEmissive = 0;
GLuint gTexSpecular = 0;

// Matrices
glm::mat4 gModel(1.0f);
glm::mat4 gView(1.0f);
glm::mat4 gProj(1.0f);

// Camera orbit state
float gCamAzimuth = 0.0f;   // horizontal angle (radians)
float gCamElevation = 0.3f; // vertical angle (radians)
float gCamDist = 6.0f;
bool  gMouseDown = false;
double gLastX = 0, gLastY = 0;

// Rotation angle
float gAngle = 0.0f;

// ============================================================
// Shader loading
// ============================================================
static std::string readFile(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) {
        std::cerr << "[ERROR] Cannot open shader: " << path << "\n";
        return "";
    }
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

static GLuint compileShader(GLenum type, const std::string& src, const std::string& name) {
    GLuint id = glCreateShader(type);
    const char* c = src.c_str();
    glShaderSource(id, 1, &c, nullptr);
    glCompileShader(id);
    GLint ok;
    glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[2048];
        glGetShaderInfoLog(id, sizeof(log), nullptr, log);
        std::cerr << "[ERROR] Shader compile error [" << name << "]:\n" << log << "\n";
    }
    return id;
}

static GLuint linkProgram(const std::string& vertPath, const std::string& fragPath) {
    std::string vsrc = readFile(vertPath);
    std::string fsrc = readFile(fragPath);
    if (vsrc.empty() || fsrc.empty()) return 0;

    GLuint vs = compileShader(GL_VERTEX_SHADER,   vsrc, vertPath);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fsrc, fragPath);

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    GLint ok;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[2048];
        glGetProgramInfoLog(prog, sizeof(log), nullptr, log);
        std::cerr << "[ERROR] Program link error:\n" << log << "\n";
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

// ============================================================
// Texture loading
// ============================================================
static GLuint loadTexture(const std::string& path) {
    stbi_set_flip_vertically_on_load(true);
    int w, h, ch;
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &ch, 4);
    if (!data) {
        std::cerr << "[WARN] Cannot load texture: " << path << "\n";
        return 0;
    }
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);
    return tex;
}

// ============================================================
// Setup geometry (VAO)
// ============================================================
static void setupGeometry() {
    glGenVertexArrays(1, &gVAO);
    glBindVertexArray(gVAO);

    auto upload = [](GLuint& vbo, const float* data, int nFloats, int attrib, int comp) {
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, nFloats * sizeof(float), data, GL_STATIC_DRAW);
        glEnableVertexAttribArray(attrib);
        glVertexAttribPointer(attrib, comp, GL_FLOAT, GL_FALSE, 0, nullptr);
    };

    // attrib locations must match shaders:
    // 0=pos, 1=color, 2=normal, 3=texcoord, 4=tangent
    upload(gVBO_pos,  cubeVertexPos,      cubeNVertex*3, 0, 3);
    upload(gVBO_col,  cubeVertexColor,    cubeNVertex*3, 1, 3);
    upload(gVBO_norm, cubeVertexNormal,   cubeNVertex*3, 2, 3);
    upload(gVBO_tex,  cubeVertexTexCoord, cubeNVertex*2, 3, 2);
    upload(gVBO_tan,  cubeVertexTangent,  cubeNVertex*3, 4, 3);

    glGenBuffers(1, &gEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        cubeNTriangleIndex * 3 * sizeof(unsigned int),
        cubeTriangleIndex, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

// ============================================================
// Build projection matrix
// ============================================================
static glm::mat4 buildProjection(float aspect) {
    float fovY = glm::radians(60.0f);
    float nearZ = 0.1f, farZ = 100.0f;
    return glm::perspective(fovY, aspect, nearZ, farZ);
}

// ============================================================
// Set common uniforms
// ============================================================
static void setUniforms(GLuint prog) {
    // Update view from camera orbit
    glm::vec3 eye(
        gCamDist * cos(gCamElevation) * sin(gCamAzimuth),
        gCamDist * sin(gCamElevation),
        gCamDist * cos(gCamElevation) * cos(gCamAzimuth)
    );
    gView = glm::lookAt(eye, glm::vec3(0), glm::vec3(0,1,0));

    glm::mat4 mv  = gView * gModel;
    glm::mat4 mvp = gProj * mv;
    glm::mat4 normalMat = glm::transpose(glm::inverse(mv));

    auto setMat4 = [&](const char* name, const glm::mat4& m) {
        GLint loc = glGetUniformLocation(prog, name);
        if (loc >= 0) glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m));
    };
    setMat4("model",        gModel);
    setMat4("view",         gView);
    setMat4("proj",         gProj);
    setMat4("modelView",    mv);
    setMat4("modelViewProj", mvp);
    setMat4("normal",       normalMat);

    // Light uniforms for v4 (2 point lights, world coords)
    {
        GLint loc0 = glGetUniformLocation(prog, "lightWorldPos0");
        if (loc0 >= 0) glUniform3f(loc0, 3.0f, 4.0f, 3.0f);
        GLint loc1 = glGetUniformLocation(prog, "lightWorldPos1");
        if (loc1 >= 0) glUniform3f(loc1, -3.0f, 2.0f, -3.0f);
    }

    // Light uniforms for v5 (attenuation, light 1 in world, light 0 at camera origin)
    {
        GLint loc1 = glGetUniformLocation(prog, "lightWorldPos1");
        if (loc1 >= 0) glUniform3f(loc1, 3.0f, 4.0f, 5.0f);
    }

    // Texture units
    auto setTex = [&](const char* name, GLuint tex, int unit) {
        GLint loc = glGetUniformLocation(prog, name);
        if (loc >= 0 && tex != 0) {
            glActiveTexture(GL_TEXTURE0 + unit);
            glBindTexture(GL_TEXTURE_2D, tex);
            glUniform1i(loc, unit);
        }
    };

    // v3 uses color2, others use color
    if (gActiveShader == 3) {
        setTex("colorTex",   gTexColor2,   0);
    } else {
        setTex("colorTex",   gTexColor,    0);
    }
    setTex("emiTex",     gTexEmissive, 1);
    setTex("specularTex",gTexSpecular, 2);
}

// ============================================================
// Render
// ============================================================
static void render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLuint prog = gPrograms[gActiveShader];
    if (!prog) return;

    glUseProgram(prog);
    setUniforms(prog);

    glBindVertexArray(gVAO);
    glDrawElements(GL_TRIANGLES, cubeNTriangleIndex * 3, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

// ============================================================
// Callbacks
// ============================================================
static void keyCallback(GLFWwindow*, int key, int, int action, int) {
    if (action != GLFW_PRESS) return;
    if (key >= GLFW_KEY_0 && key <= GLFW_KEY_5) {
        int idx = key - GLFW_KEY_0;
        if (gPrograms[idx]) {
            gActiveShader = idx;
            std::cout << "Switched to shader v" << idx << "\n";
        } else {
            std::cout << "Shader v" << idx << " not loaded\n";
        }
    }
    if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(gWindow, true);
}

static void mouseButtonCallback(GLFWwindow*, int btn, int action, int) {
    if (btn == GLFW_MOUSE_BUTTON_LEFT) {
        gMouseDown = (action == GLFW_PRESS);
        if (gMouseDown) glfwGetCursorPos(gWindow, &gLastX, &gLastY);
    }
}

static void cursorPosCallback(GLFWwindow*, double x, double y) {
    if (!gMouseDown) return;
    float dx = float(x - gLastX) * 0.01f;
    float dy = float(y - gLastY) * 0.01f;
    gCamAzimuth   += dx;
    gCamElevation -= dy;
    gCamElevation = glm::clamp(gCamElevation, -1.4f, 1.4f);
    gLastX = x; gLastY = y;
}

static void scrollCallback(GLFWwindow*, double, double dy) {
    gCamDist -= float(dy) * 0.5f;
    gCamDist = glm::clamp(gCamDist, 1.0f, 30.0f);
}

static void framebufferCallback(GLFWwindow*, int w, int h) {
    gWidth = w; gHeight = h;
    glViewport(0, 0, w, h);
    gProj = buildProjection(float(w) / float(h));
}

// ============================================================
// Main
// ============================================================
int main() {
    std::cout << "Iniciando..." << std::endl;
    if (!glfwInit()) { std::cerr << "GLFW init failed\n"; return -1; }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    gWindow = glfwCreateWindow(gWidth, gHeight, "P2 - Iluminacion GLSL", nullptr, nullptr);
    if (!gWindow) { std::cerr << "Window creation failed\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(gWindow);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) { std::cerr << "GLEW init failed\n"; return -1; }

    std::cout << "OpenGL " << glGetString(GL_VERSION) << "\n";
    std::cout << "Teclas 0-5 para cambiar shader. Arrastra raton para orbitar camara.\n";

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);

    // Register callbacks
    glfwSetKeyCallback(gWindow, keyCallback);
    glfwSetMouseButtonCallback(gWindow, mouseButtonCallback);
    glfwSetCursorPosCallback(gWindow, cursorPosCallback);
    glfwSetScrollCallback(gWindow, scrollCallback);
    glfwSetFramebufferSizeCallback(gWindow, framebufferCallback);

    // Init projection
    gProj = buildProjection(float(gWidth) / float(gHeight));

    // Load shaders
    const char* shaderBase = "shaders/";
    struct { const char* v; const char* f; } shaderFiles[6] = {
        {"shaders/shader.v0.vert", "shaders/shader.v0.frag"},
        {"shaders/shader.v1.vert", "shaders/shader.v1.frag"},
        {"shaders/shader.v2.vert", "shaders/shader.v2.frag"},
        {"shaders/shader.v3.vert", "shaders/shader.v3.frag"},
        {"shaders/shader.v4.vert", "shaders/shader.v4.frag"},
        {"shaders/shader.v5.vert", "shaders/shader.v5.frag"},
    };
    for (int i = 0; i < 6; i++) {
        gPrograms[i] = linkProgram(shaderFiles[i].v, shaderFiles[i].f);
        if (gPrograms[i]) std::cout << "  Shader v" << i << " OK\n";
    }

    // Load textures
    gTexColor    = loadTexture("img/color.png");
    gTexColor2   = loadTexture("img/color2.png");
    gTexEmissive = loadTexture("img/emissive.png");
    gTexSpecular = loadTexture("img/specMap.png");

    // Setup geometry
    setupGeometry();

    // Main loop
    while (!glfwWindowShouldClose(gWindow)) {
        // Rotate cube
        gModel = glm::mat4(1.0f);

        render();
        glfwSwapBuffers(gWindow);
        glfwPollEvents();
    }

    glfwDestroyWindow(gWindow);
    glfwTerminate();
    return 0;
}
