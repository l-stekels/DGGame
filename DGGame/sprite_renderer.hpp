#ifndef sprite_renderer_hpp
#define sprite_renderer_hpp

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "texture.hpp"
#include "shader.hpp"

using namespace glm;

class SpriteRenderer
{
public:
    SpriteRenderer(Shader &shader);
    
    ~SpriteRenderer();
    void DrawSprite(Texture2D &texture, vec2 position, vec2 size = vec2(10.0f, 10.0f), float rotate = 0.0f, vec3 color = vec3(1.0f));
private:
    Shader shader;
    unsigned int quadVAO;
    void initRenderData();
};

#endif /* sprite_renderer_hpp */
