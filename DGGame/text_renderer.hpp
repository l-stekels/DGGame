#ifndef text_renderer_hpp
#define text_renderer_hpp

#include <ft2build.h>
#include <freetype/freetype.h>
#include <map>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "texture.hpp"
#include "shader.hpp"

using glm::ivec2;
using glm::vec3;
using std::map;
using std::string;

struct Character {
    unsigned int TextureID;
    ivec2 Size;
    ivec2 Bearing;
    unsigned int Advance;
};

class TextRenderer
{
public:
    map<char, Character> Characters;
    
    Shader TextShader;
    
    TextRenderer(unsigned int width, unsigned int height);
    void Load(string font, unsigned int fontSize);
    
    void RenderText(string text, float x, float y, float scale, vec3 color = vec3(1.0f));
private:
    unsigned int VAO;
    unsigned int VBO;
};

#endif /* text_renderer_hpp */
