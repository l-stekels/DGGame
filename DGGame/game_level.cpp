#include "game_level.hpp"

#include <fstream>
#include <sstream>

using std::string;
using std::vector;
using std::ifstream;
using std::istringstream;
using glm::vec2;
using glm::vec3;

void GameLevel::Load(const char *file, unsigned int levelWidth, unsigned int levelHeight)
{
    // clear old data
    this->Bricks.clear();
    // load from file
    unsigned int tileCode;
    GameLevel level;
    string line;
    ifstream fstream(file);
    vector<vector<unsigned int>> tileData;
    if (!fstream) {
        return;
    }

    while (std::getline(fstream, line)) // read each line from level file
    {
        istringstream sstream(line);
        vector<unsigned int> row;
        while (sstream >> tileCode)
        {
            row.push_back(tileCode);
        }
        tileData.push_back(row);
    }
    if (tileData.size() <= 0) {
        return;
    }
    this->init(tileData, levelWidth, levelHeight);
}

void GameLevel::Draw(SpriteRenderer &renderer)
{
    for (GameObject &tile : this->Bricks)
    {
        if (!tile.Destroyed)
        {
            tile.Draw(renderer);
        }
    }
}

bool GameLevel::IsCompleted()
{
    for (GameObject &tile : this->Bricks)
    {
        if (!tile.IsSolid && !tile.Destroyed)
        {
            return false;
        }
    }
    return true;
}

void GameLevel::init(
          vector<vector<unsigned int>> tileData,
          unsigned int levelWidth,
          unsigned int levelHeight
          )
{
    // calculate dimensions
    unsigned int height = tileData.size();
    unsigned int width  = tileData[0].size();
    float unit_width    = levelWidth / static_cast<float>(width);
    float unit_height   = levelHeight / height;
    // initialize level tiles based on tileData
    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            vec2 pos(unit_width * x, unit_height * y);
            vec2 size(unit_width, unit_height);
            Texture2D& texture = tileData[y][x] == 1 ? ResourceManager::GetTexture("block_solid") : ResourceManager::GetTexture("block");
            vec3 color;
            switch (tileData[y][x]) {
                case 1:
                    color = vec3(0.8f, 0.8f, 0.7f);
                    break;
                case 2:
                    color = vec3(0.2f, 0.6f, 1.0f);
                    break;
                case 3:
                    color = vec3(0.0f, 0.7f, 0.0f);
                    break;
                case 4:
                    color = vec3(0.8f, 0.8f, 0.4f);
                    break;
                case 5:
                    color = vec3(1.0f, 0.5f, 0.0f);
                    break;
                default:
                    color = vec3(1.0f);
                    break;
            }
            
            GameObject obj(pos, size, texture, color);
            obj.IsSolid = tileData[y][x] == 1;
            this->Bricks.push_back(obj);
        }
    }
}
