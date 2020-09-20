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
#include "PowerUp.h"
#include "BallObject.h"
#include "OSLSound.h"

void set_asset_manager(JNIEnv *env, AAssetManager* asset_manager);
bool ShouldSpawn(unsigned int chance);
bool IsOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type);
void on_surface_created();
void on_surface_changed(int width, int height);
void on_update();
void on_drag(float x, float y, int idx);
void ActivatePowerUp(PowerUp &powerUp);
bool CheckCollision(GameObject &one, GameObject &two);

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

class Game {
public:
    unsigned int Lives;
    void set_asset_manager(JNIEnv *env, AAssetManager* asset_manager);
    void on_touch_press(float x, float y, int idx);
    AAssetManager* m_asset_manager = nullptr;
    JNIEnv *envv = nullptr;
    SpriteRenderer  *Renderer;
    GameState State;
    OSLSound * oslSound;
    bool Keys[1024];
    unsigned int Width, Height;
    Game(unsigned int width, unsigned int height);
    void DoCollisions();
    std::vector<PowerUp> PowerUps;
    std::vector<GameLevel> Levels;
    unsigned int           Level;
    void SpawnPowerUps(GameObject &block);
    void UpdatePowerUps(float dt);

    Game();

    ~Game();
    // initialize game state (load all shaders/textures/levels)
    void Init();
    // game loop
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
    void ResetLevel();
    void ResetPlayer();

    GLuint m_game_screen_top;
    GLuint m_game_screen_low;

    float m_mouse_x, m_prev_mouse_x, m_mouse_y;
    float m_diff_pos;
    float m_move_time;
};


#endif //MYFIRSTNATIVE_GAME_H
