//
// Created by Maco on 7/23/2020.
//

#ifndef MYFIRSTNATIVE_GAME_H
#define MYFIRSTNATIVE_GAME_H

#include "SpriteRenderer.h"
#include <android/asset_manager_jni.h>
#include "ResourceManager.h"
#include "glm/glm.hpp"
#include <android/log.h>
#include "stb_image.h"
#include "GameLevel.h"

void set_asset_manager(AAssetManager* asset_manager);
void on_surface_created();
void on_surface_changed(int width, int height);
void on_update();

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

class Game {
public:
    void set_asset_manager(AAssetManager* asset_manager);
    AAssetManager* m_asset_manager = nullptr;
    SpriteRenderer  *Renderer;
    GameState State;
    bool Keys[1024];
    unsigned int Width, Height;
    Game(unsigned int width, unsigned int height);

    std::vector<GameLevel> Levels;
    unsigned int           Level;

    Game();

    ~Game();
    // initialize game state (load all shaders/textures/levels)
    void Init();
    // game loop
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();

    GLuint m_game_screen_top;
    GLuint m_game_screen_low;
};


#endif //MYFIRSTNATIVE_GAME_H
