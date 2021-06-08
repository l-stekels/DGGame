#include "ball_object.hpp"

using glm::vec2;
using glm::vec3;

BallObject::BallObject() : GameObject(), Radius(12.5f), Stuck(true) {}

BallObject::BallObject(vec2 pos, float radius, vec2 velocity, Texture2D sprite, vec3 color)
    : GameObject(pos, vec2(radius * 2.0f, radius * 2.0f), sprite, color, velocity), Radius(radius), Stuck(true) { }

vec2 BallObject::Move(float dt, unsigned int window_width)
{
    if (this->Stuck)
    {
        return this->Position;
    }

    this->Position += this->Velocity * dt;
    
    if (this->Position.x <= 0.0f)
    {
        this->Velocity.x = -this->Velocity.x;
        this->Position.x = 0.0f;
    }
    else if (this->Position.x + this->Size.x >= window_width)
    {
        this->Velocity.x = -this->Velocity.x;
        this->Position.x = window_width - this->Size.x;
    }
    if (this->Position.y <= 0.0f)
    {
        this->Velocity.y = -this->Velocity.y;
        this->Position.y = 0.0f;
    }
    return this->Position;
}

void BallObject::Reset(vec2 position, vec2 velocity)
{
    this->Position = position;
    this->Velocity = velocity;
    this->Stuck = true;
}
