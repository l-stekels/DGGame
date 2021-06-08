#include "game.hpp"
#include "resource_manager.hpp"
#include "sprite_renderer.hpp"
#include "game_object.hpp"
#include "ball_object.hpp"
#include "text_renderer.hpp"
#include <iostream>

using glm::vec2;
using glm::mat4;
using glm::ortho;
using glm::clamp;
using glm::length;

SpriteRenderer *Renderer;
GameObject *Player;
BallObject *Ball;
TextRenderer *Text;

Game::Game(unsigned int width, unsigned int height) : State(GAME_MENU), Keys(), Width(width), Height(height) {}

Game::~Game()
{
    delete Renderer;
    delete Player;
    delete Text;
}

void Game::Init()
{
    ResourceManager::LoadShader("sprite.vs", "sprite.frag", nullptr, "sprite");
    
    mat4 projection = ortho(
                            0.0f,
                            static_cast<float>(this->Width),
                            static_cast<float>(this->Height),
                            0.0f,
                            -1.0f,
                            1.0f
                            );
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    // Textures
    ResourceManager::LoadTexture("background.jpeg", false, "background");
    ResourceManager::LoadTexture("paddle.png", true, "paddle");
    ResourceManager::LoadTexture("ball.png", true, "ball");
    ResourceManager::LoadTexture("block.png", false, "block");
    ResourceManager::LoadTexture("block_solid.png", false, "block_solid");
    // Levels
    GameLevel first;
    first.Load("one.lvl", this->Width, this->Height / 2);
    
    this->Levels.push_back(first);
    this->Level = 0;
    vec2 playerPos = vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
    
    vec2 ballPos = playerPos + vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
    Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("ball"), vec3(1.0f, 0.0f, 0.0f));
    Text = new TextRenderer(this->Width, this->Height);
    Text->Load("ARDESTINE.ttf", 25);
}

void Game::Update(float dt)
{
    Ball->Move(dt, this->Width);
    this->DoCollisions();
    if (Ball->Position.y >= this->Height)
    {
        this->State = GAME_MENU;
        this->ResetLevel();
        this->ResetPlayer();
    }
}

void Game::ProcessInput(float dt)
{
    if (this->State == GAME_ACTIVE || this->State == GAME_PAUSE)
    {
        this->activeState(dt);
    }
    if (this->State == GAME_MENU)
    {
        this->menuState(dt);
    }
}

void Game::Render()
{
    Renderer->DrawSprite(
                         ResourceManager::GetTexture("background"),
                         vec2(0.0f, 0.0f),
                         vec2(this->Width, this->Height),
                         0.0f
                         );
    if (this->State == GAME_MENU)
    {
        this->renderMenu();
    }
    if (this->State == GAME_ACTIVE || this->State == GAME_PAUSE)
    {
        this->renderGame();
    }
}

void Game::ResetLevel()
{
    const char* levelName;
    switch (this->Level)
    {
        case 1:
            levelName = "two.lvl";
            break;
        case 2:
            levelName = "three.lvl";
            break;
        case 3:
            levelName = "four.lvl";
            break;
        default:
            levelName = "one.lvl";
            break;
    }
    this->Levels[this->Level].Load(levelName, this->Width, this->Height / 2);
}

void Game::ResetPlayer()
{
    Player->Size = PLAYER_SIZE;
    Player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
    Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)), INITIAL_BALL_VELOCITY);
}

void Game::DoCollisions()
{
    for (GameObject &brick : this->Levels[this->Level].Bricks)
    {
        if (brick.Destroyed)
        {
            continue;
        }

        Collision collision = CheckCollision(*Ball, brick);

        if (!std::get<0>(collision))
        {
            continue;
        }

        brick.Destroy();
        
        Direction direction = std::get<1>(collision);
        vec2 diff_vector = std::get<2>(collision);
        float penetration;
        if (direction == LEFT || direction == RIGHT)
        {
            Ball->Velocity.x = -Ball->Velocity.x; // Reverse horizontal velocity
            penetration = Ball->Radius - std::abs(diff_vector.x);
        }
        else
        {
            Ball->Velocity.y = -Ball->Velocity.y; // Reverse vertical velocity
            penetration = Ball->Radius - std::abs(diff_vector.y);
        }
        switch (direction) {
            case LEFT:
                Ball->Position.x += penetration; // Move ball to right
                break;
            case RIGHT:
                Ball->Position.x -= penetration;
                break;
            case UP:
                Ball->Position.y -= penetration; // Move ball back up
                break;
            default:// Move ball back down
                Ball->Position.y += penetration;
                break;
        }
    }
    Collision result = CheckCollision(*Ball, *Player);
    if (Ball->Stuck)
    {
        return;
    }
    if (!std::get<0>(result))
    {
        return;
    }
    float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
    float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
    float percentage = distance / (Player->Size.x / 2.0f);
    
    float strength  = 2.0f;
    vec2 oldVelocity = Ball->Velocity;
    Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
    Ball->Velocity.y = -1.0f * abs(Ball->Velocity.y);
    Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
}

Collision Game::CheckCollision(BallObject &ball, GameObject &object)
{
    vec2 center(ball.Position + ball.Radius);
    vec2 aabb_half_extents(object.Size.x / 2.0f, object.Size.y / 2.0f);
    vec2 aabb_center(object.Position.x + aabb_half_extents.x, object.Position.y + aabb_half_extents.y);
    
    vec2 difference = center - aabb_center;
    vec2 clamped = clamp(difference, -aabb_half_extents, aabb_half_extents);
    
    vec2 closest = aabb_center + clamped;
    
    difference =  closest - center;
    
    if (length(difference) <= ball.Radius)
    {
        return std::make_tuple(true, VectorDirection(difference), difference);
    }
    return std::make_tuple(false, UP, vec2(0.0f, 0.0f));
}

Direction Game::VectorDirection(vec2 target)
{
    vec2 compass[] = {
        vec2(0.0f, 1.0f),  // up
        vec2(1.0f, 0.0f),  // right
        vec2(0.0f, -1.0f), // down
        vec2(-1.0f, 0.0f)  // left
    };
    
    float max = 0.0f;
    unsigned int best_match = -1;
    for (unsigned int i = 0; i < 4; i++)
    {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}

void Game::activeState(float dt)
{
    float velocity = PLAYER_VELOCITY * dt;
    if (this->Keys[GLFW_KEY_A] || this->Keys[GLFW_KEY_LEFT])
    {
        if (Player->Position.x >= 0.0f)
        {
            Player->Position.x -= velocity;
            if (Ball->Stuck && this->State != GAME_PAUSE)
            {
                Ball->Position.x -= velocity;
            }
        }
    }
    if (this->Keys[GLFW_KEY_D] ||  this->Keys[GLFW_KEY_RIGHT])
    {
        if (Player->Position.x <= this->Width - Player->Size.x)
        {
            Player->Position.x += velocity;
            if (Ball->Stuck && this->State != GAME_PAUSE)
            {
                Ball->Position.x += velocity;
            }
        }
    }
    if (this->shouldProcessKey(GLFW_KEY_SPACE))
    {
        this->State = this->State == GAME_ACTIVE ? GAME_PAUSE : GAME_ACTIVE;
        this->KeysProcessed[GLFW_KEY_SPACE] = true;
    }
}

void Game::menuState(float dt)
{
    if (this->shouldProcessKey(GLFW_KEY_SPACE))
    {
        this->State = GAME_ACTIVE;
        Ball->Stuck = false;
        this->KeysProcessed[GLFW_KEY_SPACE] = true;
    }
}

void Game::renderMenu()
{
    float line = Height / 2;
    string menu[] = {
        "SPACE: sakt speli / pauze",
        "ALT + ENTER: pilnekrana",
        "ESC: beigt speli"
    };
    for (const string &text : menu)
    {
        Text->RenderText(text, 250.0f, line, 1.0f);
        line += 25;
    }
}

void Game::renderGame()
{
    this->Levels[this->Level].Draw(*Renderer);
    Player->Draw(*Renderer);
    Ball->Stuck = this->State == GAME_PAUSE;
    Ball->Draw(*Renderer);
}

bool Game::shouldProcessKey(int key)
{
    return this->Keys[key] && !this->KeysProcessed[key];
}

