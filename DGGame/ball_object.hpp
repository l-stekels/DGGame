#ifndef ball_object_hpp
#define ball_object_hpp

#include "game_object.hpp"
#include "texture.hpp"

using glm::vec2;

class BallObject : public GameObject
{
public:
    float Radius;
    bool Stuck;
    
    BallObject();
    BallObject(vec2 pos, float radius, vec2 velocity, Texture2D sprite, vec3 color);
    
    vec2 Move(float dt, unsigned int window_width);
    
    void Reset(vec2 position, vec2 velocity);
};

#endif /* ball_object_hpp */
