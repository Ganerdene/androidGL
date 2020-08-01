//
// Created by Maco on 7/30/2020.
//

#ifndef MYFIRSTNATIVE_GAMELEVEL_H
#define MYFIRSTNATIVE_GAMELEVEL_H

#include <vector>
#include "glm/glm.hpp"
#include "GameObject.h"
#include "SpriteRenderer.h"
#include "ResourceManager.h"

class GameLevel {
public:
    // level state
    std::vector<GameObject> Bricks;
    // constructor
    GameLevel() { }
    // loads level from file
    void Load(const char *file, unsigned int levelWidth, unsigned int levelHeight, unsigned int screenHeight);
    // render level
    void Draw(SpriteRenderer &renderer);
    // check if the level is completed (all non-solid tiles are destroyed)
    bool IsCompleted();
private:
    // initialize level from tile data
    void init(std::vector<std::vector<unsigned int>> tileData,unsigned int levelWidth, unsigned int levelHeight,
              unsigned int screenHeight);
};


#endif //MYFIRSTNATIVE_GAMELEVEL_H
