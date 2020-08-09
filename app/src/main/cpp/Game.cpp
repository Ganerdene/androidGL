//
// Created by Maco on 7/23/2020.
//

#include "Game.h"
#include "GameLevel.h"
#include "BallObject.h"
#include <chrono>
#define LOG_TAG "libNative"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

SpriteRenderer  *Renderer;
std::chrono::time_point <std::chrono::system_clock> last_frame;
Game game;
// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(200.0f, 40.0f);
// Initial velocity of the player paddle
const float PLAYER_VELOCITY(500.0f);
GameObject *Player;
// Initial velocity of the Ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// Radius of the ball object
const float BALL_RADIUS = 12.5f;

BallObject     *Ball;

Game::Game(unsigned int width, unsigned int height)
        : State(GAME_ACTIVE), Keys(), Width(width), Height(height) {

}
Game::Game() {

}
void on_surface_created() {
    game.Init();
}
void on_drag(float x, float y, int idx) {
    game.on_touch_press(x, y, idx);
}
bool CheckCollision(BallObject &one, GameObject &two){
    // get center point circle first
    glm::vec2 center(one.Position + one.Radius);
    // calculate AABB info (center, half-extents)
    glm::vec2 aabb_half_extentes(two.Size.x / 2.0f, two.Size.y / 2.0f);
    glm::vec2 aabb_center(
                two.Position.x + aabb_half_extentes.x,
                two.Position.y + aabb_half_extentes.y
            );
    // get difference vector between both centers
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extentes, aabb_half_extentes);
// add clamped value to AABB_center and we get the value of box closest to circle
    glm::vec2 closest = aabb_center + clamped;
    // retrieve vector between center circle and closest point AABB and check if length <= radius
    difference = closest - center;
    return glm::length(difference) < one.Radius;

}
void on_surface_changed(int width, int height){
    LOGI("working on_surface_changed() ");
    game.Width = width;
    game.Height = height;
    LOGI("on_surface_changed() width %d height %d ", game.Width, game.Height);
    uint m_game_screen_top = game.Height - game.Height * 0.9;
    uint m_game_screen_low = game.Height - game.Height * 0.8;

    glViewport(0,0,width,height);

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width),static_cast<float>(height), 0.0f, -1.0f, 1.0f);

    ResourceManager::GetShader("sprite").Use().SetMatrix4("projection", projection);


    GameLevel one;
    one.Load("levels/one_lvl.json", game.Width, game.Height * 0.3, m_game_screen_top);
    game.Levels.push_back(one);
    game.Level = 0;

    glm::vec2 playerPos = glm::vec2(
            game.Width / 2.0f - PLAYER_SIZE.x / 2.0f,
            game.Height - PLAYER_SIZE.y
    );

    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS,-BALL_RADIUS * 2.0f);
    Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY,ResourceManager::GetTexture("face"));

}
void on_update() {
    std::chrono::time_point <std::chrono::system_clock> current_frame = std::chrono::system_clock::now();
    int delta_time = std::chrono::duration_cast<std::chrono::milliseconds>(
            current_frame - last_frame).count();
    last_frame = current_frame;
    float dt = (GLfloat) delta_time / 1000;
    game.Update(dt);
    game.Render();
}
Game::~Game() {
    delete Renderer;
    delete Player;
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
    ResourceManager::LoadTexture("textures/paddle.png", true, "paddle");
    LOGI("gameinit width %d height %d ", this->Width, this->Height);



}

void Game::Update(float dt) {
    Ball->Move(dt, this->Width);
    this->DoCollisions();
    if ((Player->Position.x + Player->Size.x / 2) != m_mouse_x) {
        if (m_prev_mouse_x != m_mouse_x) {
            m_diff_pos = m_mouse_x - (Player->Position.x + Player->Size.x / 2);
            m_prev_mouse_x = m_mouse_x;
            m_move_time = 0.05f;
        }

        if (m_move_time > 0) {
            if (m_diff_pos > 0) {
                if ((Player->Position.x + Player->Size.x / 2) < m_mouse_x) {
                    GLfloat delta = (m_diff_pos * dt) / m_move_time;
                    Player->Position.x += delta;
                    m_diff_pos -= delta;
                    m_move_time -= dt;

                    if(Ball->Stuck){
                        Ball->Position.x += delta;
                    }
                }
            } else {
                if ((Player->Position.x + Player->Size.x / 2) > m_mouse_x) {
                    GLfloat delta = (m_diff_pos * dt) / m_move_time;
                    Player->Position.x += delta;
                    
                    if(Ball->Stuck){
                        Ball->Position.x += delta;
                    }

                    m_diff_pos -= delta;
                    m_move_time -= dt;
                }
            }
        }

    }
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
    // draw player
    Player->Draw(*Renderer);
    // draw ball
    Ball->Draw(*Renderer);
}

void Game::set_asset_manager(AAssetManager *asset_manager) {
   m_asset_manager = asset_manager;
}

void Game::on_touch_press(float x, float y, int idx) {
    if (idx > 0)
        return;
    LOGI("darsan x %f y %f player pos x %f y %f",x,y,Player->Position.x, Player->Position.y);
    if(x-110 <= Player->Position.x && x+110 >= Player->Position.x && y-110 <= Player->Position.y && y+110 >= Player->Position.y){
        Ball->Stuck = false;
    }

    m_mouse_x = x;
    m_mouse_y = y;
}

void Game::DoCollisions() {
    for(GameObject &box : this->Levels[this->Level].Bricks){
        if(!box.Destroyed){
            if(CheckCollision(*Ball, box)){
                if(!box.IsSolid){
                    box.Destroyed = true;
                }
            }
        }
    }
}

void set_asset_manager(AAssetManager *asset_manager){
    game.set_asset_manager(asset_manager);
}
