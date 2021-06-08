#ifndef game_level_hpp
#define game_level_hpp
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "game_object.hpp"
#include "sprite_renderer.hpp"
#include "resource_manager.hpp"

using std::vector;

class GameLevel
{
public:
    vector<GameObject> Bricks;
    
    GameLevel() {}
    
    void Load(const char *file, unsigned int levelWidth, unsigned int levelHeight);
    
    void Draw(SpriteRenderer &renderer);
    
    bool IsCompleted();
private:
    void init(
              vector<vector<unsigned int>> tileData,
              unsigned int levelWidth,
              unsigned int levelHeight
              );
};

#endif /* game_level_hpp */
