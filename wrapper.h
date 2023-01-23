#pragma once
#include <cassert>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "Shader.h"

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

struct ModelData
{
    unsigned int glVertexData;
    unsigned int glIndexData;
    size_t indexDataLength;
    glm::vec3 offset;
    glm::vec3 size;
};

struct Vertex
{
    Vertex(float inX, float inY, float inZ) : x(inX), y(inY), z(inZ) {}
    float x, y, z;
};

struct Texture
{
    Texture(float inU, float inV) : u(inU), v(inV) {}
    float u, v;
};

struct Normal
{
    Normal(float inX, float inY, float inZ) : x(inX), y(inY), z(inZ) {}
    float x, y, z;
};

struct Face
{
    Face(int v[], int t[], int n[])
    {
        vIndex[0] = v[0];
        vIndex[1] = v[1];
        vIndex[2] = v[2];
        tIndex[0] = t[0];
        tIndex[1] = t[1];
        tIndex[2] = t[2];
        nIndex[0] = n[0];
        nIndex[1] = n[1];
        nIndex[2] = n[2];
    }
    unsigned int vIndex[3], tIndex[3], nIndex[3];
};

// static std::ostream& operator<<(std::ostream& os, const glm::vec3& v) {
//     return os << "[" << v.x << ", " << v.y << ", " << v.z << "]";
// }

void draw_model(const ModelData& model, Shader& s, const glm::vec3& position, const float y_rot_angle, const glm::vec3& scale, const glm::vec3& color);
ModelData load_model(const std::string& fileName);