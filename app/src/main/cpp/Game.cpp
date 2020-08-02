//
// Created by Maco on 7/23/2020.
//

#include "Game.h"
#include "GameLevel.h"

#define LOG_TAG "libNative"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

SpriteRenderer  *Renderer;

Game game;

Game::Game(unsigned int width, unsigned int height)
        : State(GAME_ACTIVE), Keys(), Width(width), Height(height) {

}
Game::Game() {

}
void on_surface_created() {
    game.Init();
}
void on_surface_changed(int width, int height){
    LOGI("working on_surface_changed() ");
    game.Width = width;
    game.Height = height;

    uint m_game_screen_top = game.Height - game.Height * 0.9;
    uint m_game_screen_low = game.Height - game.Height * 0.8;

    glViewport(0,0,width,height);

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width),static_cast<float>(height), 0.0f, -1.0f, 1.0f);

    ResourceManager::GetShader("sprite").Use().SetMatrix4("projection", projection);


    GameLevel one;
    one.Load("levels/one_lvl.json", game.Width, game.Height * 0.3, m_game_screen_top);
    game.Levels.push_back(one);
    game.Level = 0;

}
void on_update() {
    game.Render();
}
Game::~Game() {
    delete Renderer;
}

void Game::Init() {
    ResourceManager::set_asset_manager(m_asset_manager);
    ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.fs", nullptr, "sprite");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    // set render-specific controls
    Shader shader = ResourceManager::GetShader("sprite");
    Renderer = new SpriteRenderer(shader);
    ResourceManager::LoadTexture("textures/background.jpg", false, "background");
    ResourceManager::LoadTexture("textures/awesomeface2.png", true, "face");
    ResourceManager::LoadTexture("textures/block.png", false, "block");
    ResourceManager::LoadTexture("textures/block_solid.png", false, "block_solid");

}

void Game::Update(float dt) {

}

void Game::ProcessInput(float dt) {

}
//void set_asset_manager(AAssetManager *asset_manager) {
//    LOGI("setting asset Manager void set_asset_manager");
//    game.set_asset_manager(asset_manager);
//}
void Game::Render() {
    Texture2D texture2D = ResourceManager::GetTexture("background");
    // draw background
    Renderer->DrawSprite(texture2D,glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
    // draw level
    this->Levels[this->Level].Draw(*Renderer);
}

void Game::set_asset_manager(AAssetManager *asset_manager) {
    LOGI("setting asset Manager Game::set_asset_manager");
   m_asset_manager = asset_manager;
}
void set_asset_manager(AAssetManager *asset_manager){
    LOGI("setting asset Manager void set_asset_manager");
    game.set_asset_manager(asset_manager);
}