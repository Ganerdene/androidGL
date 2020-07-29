//
// Created by Maco on 7/23/2020.
//

#include "Game.h"

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
    glViewport(0,0,width,height);
    game.Width = width;
    game.Height = height;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void on_update() {
    game.Render();
}
Game::~Game() {
    delete Renderer;
}

void Game::Init() {
    LOGI("working Game::Init() ");
    if(m_asset_manager != nullptr){
        LOGI("working m_asset_manager not null ");
    }else{
        LOGI("working m_asset_manager is NULL ");
    }

    ResourceManager::set_asset_manager(m_asset_manager);
    ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.fs", nullptr, "sprite");
    LOGI("working after shader ortho width %d height %d ", this->Width, this->Height);
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    // set render-specific controls
    Shader shader = ResourceManager::GetShader("sprite");
    LOGI("working Shader id on init %d", shader.ID);
    Renderer = new SpriteRenderer(shader);
    //Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    // load textures
    ResourceManager::LoadTexture("textures/firefox.png", true, "face");
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
    Texture2D texture2D = ResourceManager::GetTexture("face");
    Renderer->DrawSprite(texture2D, glm::vec2(200.0f, 200.0f), glm::vec2(300.0f, 400.0f), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Game::set_asset_manager(AAssetManager *asset_manager) {
    LOGI("setting asset Manager Game::set_asset_manager");
   m_asset_manager = asset_manager;
}
void set_asset_manager(AAssetManager *asset_manager){
    LOGI("setting asset Manager void set_asset_manager");
    game.set_asset_manager(asset_manager);
}