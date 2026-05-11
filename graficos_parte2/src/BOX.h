#pragma once

// Cube geometry with normals, UVs, tangents
// 24 vertices (4 per face × 6 faces) to allow per-face normals

const int cubeNVertex = 24;
const int cubeNTriangleIndex = 12; // 2 triangles × 6 faces

const unsigned int cubeTriangleIndex[] = {
    // Face z=+1
    0,1,2,  1,3,2,
    // Face z=-1
    4,6,5,  5,6,7,
    // Face x=+1
    8,10,9,  9,10,11,
    // Face x=-1
    12,13,14,  13,15,14,
    // Face y=+1
    16,17,18,  17,19,18,
    // Face y=-1
    20,22,21,  21,22,23,
};

const float cubeVertexPos[] = {
    // Face z=+1
    -1.0f,-1.0f, 1.0f,
     1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
     1.0f, 1.0f, 1.0f,
    // Face z=-1
    -1.0f,-1.0f,-1.0f,
     1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
     1.0f, 1.0f,-1.0f,
    // Face x=+1
     1.0f,-1.0f,-1.0f,
     1.0f,-1.0f, 1.0f,
     1.0f, 1.0f,-1.0f,
     1.0f, 1.0f, 1.0f,
    // Face x=-1
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    // Face y=+1
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
     1.0f, 1.0f,-1.0f,
     1.0f, 1.0f, 1.0f,
    // Face y=-1
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f, 1.0f,
     1.0f,-1.0f,-1.0f,
     1.0f,-1.0f, 1.0f,
};

const float cubeVertexColor[] = {
    1,0,0, 1,0,0, 1,0,0, 1,0,0,
    0,1,0, 0,1,0, 0,1,0, 0,1,0,
    0,0,1, 0,0,1, 0,0,1, 0,0,1,
    1,1,0, 1,1,0, 1,1,0, 1,1,0,
    1,0,1, 1,0,1, 1,0,1, 1,0,1,
    0,1,1, 0,1,1, 0,1,1, 0,1,1,
};

const float cubeVertexNormal[] = {
    0,0,1, 0,0,1, 0,0,1, 0,0,1,
    0,0,-1,0,0,-1,0,0,-1,0,0,-1,
    1,0,0, 1,0,0, 1,0,0, 1,0,0,
    -1,0,0,-1,0,0,-1,0,0,-1,0,0,
    0,1,0, 0,1,0, 0,1,0, 0,1,0,
    0,-1,0,0,-1,0,0,-1,0,0,-1,0,
};

const float cubeVertexTexCoord[] = {
    0,0, 1,0, 0,1, 1,1,
    1,0, 0,0, 1,1, 0,1,
    0,0, 1,0, 0,1, 1,1,
    1,0, 0,0, 1,1, 0,1,
    0,0, 1,0, 0,1, 1,1,
    0,1, 1,1, 0,0, 1,0,
};

// Tangents for normal mapping (along U direction for each face)
const float cubeVertexTangent[] = {
    1,0,0, 1,0,0, 1,0,0, 1,0,0,   // z=+1: T along +X
    -1,0,0,-1,0,0,-1,0,0,-1,0,0,  // z=-1: T along -X
    0,0,-1,0,0,-1,0,0,-1,0,0,-1,  // x=+1: T along -Z
    0,0,1, 0,0,1, 0,0,1, 0,0,1,   // x=-1: T along +Z
    1,0,0, 1,0,0, 1,0,0, 1,0,0,   // y=+1: T along +X
    1,0,0, 1,0,0, 1,0,0, 1,0,0,   // y=-1: T along +X
};
