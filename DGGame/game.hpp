#ifndef game_hpp
#define game_hpp

#include <glad/glad.h>
#define GL_SILENCE_DEPRECATION 1
#include <GLFW/glfw3.h>
#include <vector>
#include <tuple>

#include "game_level.hpp"
#include "ball_object.hpp"

using std::vector;
using glm::vec2;

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN,
    GAME_PAUSE
};

enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};


typedef std::tuple<bool, Direction, vec2> Collision;

const vec2 PLAYER_SIZE(100.0f, 20.0f);
const float PLAYER_VELOCITY(500.0f);
const vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
const float BALL_RADIUS = 12.5f;

class Game
{
public:
    GameState State;
    bool Keys[1024];
    bool KeysProcessed[1024];
    unsigned int Width;
    unsigned int Height;
    
    vector<GameLevel> Levels;
    unsigned int Level;
    
    Game(unsigned int width, unsigned int height);
    ~Game();
    
    void Init();
    
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
    void DoCollisions();
    Collision CheckCollision(BallObject &ball, GameObject &object);
    Direction VectorDirection(vec2 target);

    void ResetLevel();
    void ResetPlayer();
private:
    void activeState(float dt);
    void menuState(float dt);
    void renderGame();
    void renderMenu();
    bool shouldProcessKey(int key);
};
#endif /* game_hpp */
