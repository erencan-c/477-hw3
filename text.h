#ifndef _TEXT_H_
#define _TEXT_H_

#include <map>
#include "Shader.h"

using namespace std;

struct Character {
    unsigned int TextureID;   
    glm::ivec2 Size;    
    glm::ivec2 Bearing;  
    unsigned int Advance;    
};

struct FontData
{
    unsigned int fontVBO;
    std::map<char, Character> chars;
};

FontData load_font(const std::string& filename);
float draw_text(const FontData& font, Shader& shader, const std::string& text, glm::vec2 position, float scale, const glm::vec3 color);

#endif