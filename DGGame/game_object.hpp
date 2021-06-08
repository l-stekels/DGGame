#ifndef game_object_hpp
#define game_object_hpp

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "texture.hpp"
#include "sprite_renderer.hpp"

class GameObject
{
public:
    glm::vec2 Position;
    glm::vec2 Size;
    glm::vec2 Velocity;
    glm::vec3 Color;
    float Rotation;
    bool IsSolid;
    bool Destroyed;
    
    Texture2D Sprite;
    GameObject();
    GameObject(
               glm::vec2 pos,
               glm::vec2 size,
               Texture2D sprite,
               glm::vec3 color = glm::vec3(1.0f),
               glm::vec2 velocity = glm::vec2(0.0f, 0.0f)
               );
    
    virtual void Draw(SpriteRenderer &renderer);
    void Destroy();
};

#endif /* game_object_hpp */
