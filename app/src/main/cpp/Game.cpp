//
// Created by Maco on 7/23/2020.
//

#include "Game.h"
#include "GameLevel.h"
#include "BallObject.h"
#include "ParticleGenerator.h"
#include "PostProcessor.h"
#include "TextRenderer.h"
#include <chrono>
#include <sstream>

#define LOG_TAG "libNative"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};
typedef std::tuple<bool, Direction, glm::vec2> Collision;
float ShakeTime = 0.0f;
SpriteRenderer *Renderer;
std::chrono::time_point<std::chrono::system_clock> last_frame;
Game game;
// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(200.0f, 40.0f);
// Initial velocity of the player paddle
const float PLAYER_VELOCITY(500.0f);
GameObject *Player;
// Initial velocity of the Ball
const glm::vec2 INITIAL_BALL_VELOCITY(200.0f, -700.0f);
// Radius of the ball object
const float BALL_RADIUS = 24.5f;

BallObject *Ball;
ParticleGenerator *Particles;
PostProcessor *Effects;
TextRenderer *Text;

// collision detection
Collision CheckCollision(BallObject &one, GameObject &two);

Direction VectorDirection(glm::vec2 closest);

Game::Game(unsigned int width, unsigned int height)
        : State(GAME_ACTIVE), Keys(), Width(width), Height(height), Lives(3) {

}

Game::Game() : Lives(3), State(GAME_MENU) {

}

void on_surface_created() {
    game.Init();
}

void on_drag(float x, float y, int idx) {
    game.on_drag(x, y, idx);
}

void on_click(float x, float y, int idx) {
    game.on_click(x, y, idx);
}

Direction VectorDirection(glm::vec2 target) {
    glm::vec2 compass[] = {
            glm::vec2(0.0f, 1.0f),//up
            glm::vec2(1.0f, 0.0f),
            glm::vec2(0.0f, -1.0f),
            glm::vec2(-1.0f, 0.0f)
    };
    float max = 0.0f;
    unsigned int best_match = -1;
    for (unsigned int i = 0; i < 4; i++) {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max) {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction) best_match;
}

Collision CheckCollision(BallObject &one, GameObject &two) {
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
    if (glm::length(difference) <= one.Radius) {
        return std::make_tuple(true, VectorDirection(difference), difference);
    } else {
        return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
    }
}

bool CheckCollision(GameObject &one, GameObject &two) // AABB - AABB collision
{
    // collision x-axis?
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
                      two.Position.x + two.Size.x >= one.Position.x;
    // collision y-axis?
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
                      two.Position.y + two.Size.y >= one.Position.y;
    // collision only if on both axes
    return collisionX && collisionY;
}

void on_surface_changed(int width, int height) {
    LOGI("working on_surface_changed() ");
    game.Width = width;
    game.Height = height;
    LOGI("on_surface_changed() width %d height %d ", game.Width, game.Height);
    uint m_game_screen_top = game.Height - game.Height * 0.9;
    uint m_game_screen_low = game.Height - game.Height * 0.8;

    glViewport(0, 0, width, height);

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height),
                                      0.0f, -1.0f, 1.0f);

    ResourceManager::GetShader("sprite").Use().SetMatrix4("projection", projection);
    ResourceManager::GetShader("particle").Use().SetMatrix4("projection", projection);


    GameLevel one, two, three;
    one.Load("levels/one_lvl.json", game.Width, game.Height * 0.3, m_game_screen_top);
    two.Load("levels/two_lvl.json", game.Width, game.Height * 0.3, m_game_screen_top);
    three.Load("levels/three_lvl.json", game.Width, game.Height * 0.3, m_game_screen_top);
    game.Levels.push_back(one);
    game.Levels.push_back(two);
    game.Levels.push_back(three);
    game.Level = 0;

    glm::vec2 playerPos = glm::vec2(
            game.Width / 2.0f - PLAYER_SIZE.x / 2.0f,
            game.Height - PLAYER_SIZE.y - 280
    );

    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
    glm::vec2 ballPos =
            playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
    Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY,
                          ResourceManager::GetTexture("face"));

    Particles = new ParticleGenerator(
            ResourceManager::GetShader("particle"),
            ResourceManager::GetTexture("particle"),
            200
    );
    Effects = new PostProcessor(ResourceManager::GetShader("postprocessing"), width, height);

    Text = new TextRenderer(width, height);
    Text->Load("font/OCRAEXT.TTF", 70);
}

void on_update() {
    std::chrono::time_point<std::chrono::system_clock> current_frame = std::chrono::system_clock::now();
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
    oslSound = new OSLSound(m_asset_manager, false);
    oslSound->setSound(true);
    oslSound->play(OSLSound::BACKGROUND);
    ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.fs", nullptr, "sprite");
    ResourceManager::LoadShader("shaders/particle.vs", "shaders/particle.fs", nullptr, "particle");
    ResourceManager::LoadShader("shaders/post_processor.vs", "shaders/post_processor.fs", nullptr,
                                "postprocessing");

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
                                      static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("particle").SetMatrix4("projection", projection);
    // set render-specific controls
    Shader shader = ResourceManager::GetShader("sprite");
    Renderer = new SpriteRenderer(shader);
    ResourceManager::LoadTexture("textures/background.jpg", false, "background");
    ResourceManager::LoadTexture("textures/awesomeface2.png", false, "face");
    ResourceManager::LoadTexture("textures/block.png", false, "block");
    ResourceManager::LoadTexture("textures/block_solid.png", false, "block_solid");
    ResourceManager::LoadTexture("textures/paddle.png", false, "paddle");
    ResourceManager::LoadTexture("textures/powerup_speed.png", true, "powerup_speed");
    ResourceManager::LoadTexture("textures/powerup_sticky.png", true, "powerup_sticky");
    ResourceManager::LoadTexture("textures/powerup_increase.png", true, "powerup_increase");
    ResourceManager::LoadTexture("textures/powerup_confuse.png", true, "powerup_confuse");
    ResourceManager::LoadTexture("textures/powerup_chaos.png", true, "powerup_chaos");
    ResourceManager::LoadTexture("textures/powerup_passthrough.png", true, "powerup_passthrough");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Game::Update(float dt) {
    Ball->Move(dt, this->Width);
    this->DoCollisions();
    if (ShakeTime > 0.0f) {
        ShakeTime -= dt;
        if (ShakeTime <= 0.0f) {
            Effects->Shake = false;
        }
    }
    this->UpdatePowerUps(dt);
    if (Ball->Position.y >= this->Height) { // did ball reach bottom edge?
        --this->Lives;
        LOGI("hasagdsan_ami %d", this->Lives);
        if (this->Lives == 0) {
            LOGI("lives == 0 %d", this->Lives);
            this->ResetLevel();
            this->State = GAME_MENU;
        }
        this->ResetPlayer();
    }
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

                    if (Ball->Stuck) {
                        Ball->Position.x += delta;
                    }
                }
            } else {
                if ((Player->Position.x + Player->Size.x / 2) > m_mouse_x) {
                    GLfloat delta = (m_diff_pos * dt) / m_move_time;
                    Player->Position.x += delta;

                    if (Ball->Stuck) {
                        Ball->Position.x += delta;
                    }

                    m_diff_pos -= delta;
                    m_move_time -= dt;
                }
            }
        }

    }
    if (this->State == GAME_ACTIVE && this->Levels[this->Level].IsCompleted()) {
        this->ResetLevel();
        this->ResetPlayer();
        this->State = GAME_WIN;
    }

    //update particles
    Particles->Update(dt, *Ball, 1, glm::vec2(Ball->Radius));
}

void Game::ProcessInput(float dt) {

}

//void set_asset_manager(AAssetManager *asset_manager) {
//    LOGI("setting asset Manager void set_asset_manager");
//    game.set_asset_manager(asset_manager);
//}
void Game::Render() {
    if (this->State == GAME_ACTIVE || this->State == GAME_MENU || this->State == GAME_WIN) {
        Effects->BeginRender();
        Texture2D texture2D = ResourceManager::GetTexture("background");
        // draw background
        Renderer->DrawSprite(texture2D, glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height),
                             0.0f);
        // draw level
        this->Levels[this->Level].Draw(*Renderer);
        // draw player
        Player->Draw(*Renderer);
        //draw powerups
        for (PowerUp &powerUp : this->PowerUps) {
            if (!powerUp.Destroyed) {
                powerUp.Draw(*Renderer);
            }
        }
        // draw particles
        Particles->Draw();
        // draw ball
        Ball->Draw(*Renderer);
        Effects->EndRender();

        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::chrono::system_clock::duration tp = now.time_since_epoch();
        std::chrono::microseconds mcs = std::chrono::duration_cast<std::chrono::microseconds>(tp);
        std::chrono::seconds s = std::chrono::duration_cast<std::chrono::seconds>(tp);
        std::chrono::hours h = std::chrono::duration_cast<std::chrono::hours>(tp);

        std::chrono::seconds diff_s = s - h;
        std::chrono::microseconds diff_mcs = mcs - s;
        GLfloat diff_mcs_count = diff_mcs.count();
        GLfloat time = diff_s.count() + diff_mcs_count / 1000000.0f;
        Effects->Render(time);

        std::stringstream ss;
        ss << this->Lives;
        //LOGI("hasagdsan_ami_render %d", this->Lives);
        Text->RenderText("Lives: " + ss.str(), 15.0f, 15.0f, 1.0f);
    }
    if (this->State == GAME_MENU) {
        Text->RenderText("Press tap center to start", 90.0f, this->Height / 2.0f - 40.0f, 0.9f);
        Text->RenderText("Swipe left or right to select level", 135.0f, this->Height / 2.0f + 40.0f,
                         0.55f);
    }
    if (this->State == GAME_WIN) {
        Text->RenderText("You WON!!!", 380.0f, this->Height / 2.0f - 60.0f, 1.0f,
                         glm::vec3(0.0f, 1.0f, 0.0f));
        Text->RenderText("tap center to retry", 135.0f, this->Height / 2.0f, 1.0f,
                         glm::vec3(1.0f, 1.0f, 0.0f));
    }
}

void Game::set_asset_manager(JNIEnv *env, AAssetManager *asset_manager) {
    m_asset_manager = asset_manager;
    envv = env;
}

void Game::on_drag(float x, float y, int idx) {

    if (idx > 0)
        return;


    //LOGI("taped x %f  y %f", x,y);

    m_mouse_x = x;
    m_mouse_y = y;
}


void Game::DoCollisions() {
    for (GameObject &box : this->Levels[this->Level].Bricks) {
        if (!box.Destroyed) {
            Collision collision = CheckCollision(*Ball, box);
            if (std::get<0>(collision)) {
                if (!box.IsSolid) {
                    this->SpawnPowerUps(box);
                    oslSound->play(OSLSound::DESTROYED);
                    LOGI("box_position x %f y %f", box.Position.x, box.Position.y);
                    box.Destroyed = true;
                } else {
                    oslSound->play(OSLSound::SOLID);
                    ShakeTime = 0.05f;
                    Effects->Shake = true;
                }

                //collision resulution
                Direction dir = std::get<1>(collision);
                glm::vec2 diff_vector = std::get<2>(collision);
                if (!(Ball->PassThrough && !box.IsSolid)) {
                    if (dir == LEFT || dir == RIGHT) {
                        Ball->Velocity.x = -Ball->Velocity.x; // reverse horizontal velocity;
                        // relocate
                        float penetration = Ball->Radius - std::abs(diff_vector.x);
                        if (dir == LEFT) {
                            Ball->Position.x += penetration; // move ball to right
                        } else {
                            Ball->Position.x -= penetration; // move ball to left
                        }
                    } else { //vertical collision
                        Ball->Velocity.y = -Ball->Velocity.y;
                        //relocate
                        float penetration = Ball->Radius - std::abs(diff_vector.y);
                        if (dir == UP) {
                            Ball->Position.y -= penetration; // move ball back up;
                        } else {
                            Ball->Position.y += penetration; // move ball back down;
                        }

                    }
                }
            }
        }
    }
    // also check collisions on PowerUps and if so, acviate them
    for (PowerUp &powerUp : this->PowerUps) {
        if (!powerUp.Destroyed) {
            // first check if powerup passed bottom edge, if so: keep as inactive and destroy
            if (powerUp.Position.y >= this->Height) {
                powerUp.Destroyed = true;
            }

            if (CheckCollision(*Player, powerUp)) {
                // collided with player, now activate powerup
                ActivatePowerUp(powerUp);
                powerUp.Destroyed = true;
                powerUp.Activated = true;
                oslSound->play(OSLSound::POWER_UP);
            }
        }
    }
    Collision result = CheckCollision(*Ball, *Player);
    if (!Ball->Stuck && std::get<0>(result)) {
        //check where it hit the board, and change velocity based on where it hit the board
        float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
        float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
        float percentage = distance / (Player->Size.x / 2.0f);
        // then move accordingly
        float strength = 2.0f;
        glm::vec2 oldVelocity = Ball->Velocity;
        Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
        Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
        Ball->Velocity.y = -1.0f * abs(Ball->Velocity.y);

        // if sticky powerup is activated, also stick ball to paddle once new velocity vectors were calculated
        Ball->Stuck = Ball->Sticky;
    }
}

void Game::ResetPlayer() {
// reset player/ball stats

    Player->Size = PLAYER_SIZE;
    Player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f,
                                 this->Height - PLAYER_SIZE.y - 280);
    Ball->Reset(
            Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)),
            INITIAL_BALL_VELOCITY);
    Effects->Chaos = Effects->Confuse = false;
    Ball->PassThrough = Ball->Sticky = false;
    Player->Color = glm::vec3(1.0f);
    Ball->Color = glm::vec3(1.0f);
}

void Game::ResetLevel() {
    this->Lives = 3;
    if (this->Level == 0) {
        this->Levels[0].Load("levels/one_lvl.json", game.Width, game.Height * 0.3,
                             m_game_screen_top);
    } else if (this->Level == 1) {
        this->Levels[1].Load("levels/two_lvl.json", game.Width, game.Height * 0.3,
                             m_game_screen_top);
    } else if (this->Level == 2) {
        this->Levels[2].Load("levels/three_lvl.json", game.Width, game.Height * 0.3,
                             m_game_screen_top);
    }
}

void Game::SpawnPowerUps(GameObject &block) {
    if (ShouldSpawn(70)) {// 1 in 75 chance
        this->PowerUps.push_back(
                PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 10.0f, block.Position,
                        ResourceManager::GetTexture("powerup_speed"))
        );
        LOGI("powerup__speed", "speed");
    }
    if (ShouldSpawn(70)) {
        this->PowerUps.push_back(
                PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.Position,
                        ResourceManager::GetTexture("powerup_sticky"))
        );
        LOGI("powerup__sticky", "sticky");

    }
    if (ShouldSpawn(70)) {
        this->PowerUps.push_back(
                PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.Position,
                        ResourceManager::GetTexture("powerup_passthrough"))
        );
        LOGI("powerup__pass-through", "pass-through");

    }
    if (ShouldSpawn(70)) {
        this->PowerUps.push_back(
                PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4f), 10.0f, block.Position,
                        ResourceManager::GetTexture("powerup_increase"))
        );
        LOGI("powerup__pad-size-increase", "pad-size-increase");
    }
    if (ShouldSpawn(10)) {
        this->PowerUps.push_back(
                PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.Position,
                        ResourceManager::GetTexture("powerup_confuse"))
        );
        LOGI("powerup__confuse", "confuse");
    }
    //todo check in future
    // multisampled frame buffer doesnt work i dont know why so i set it 0
    if (ShouldSpawn(0)) {
        this->PowerUps.push_back(
                PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.Position,
                        ResourceManager::GetTexture("powerup_chaos"))
        );
        LOGI("powerup__chaos", "chaos");
    }
}

void ActivatePowerUp(PowerUp &powerUp) {
    if (powerUp.Type == "speed") {
        Ball->Velocity *= 1.3;
    } else if (powerUp.Type == "sticky") {
        Ball->Sticky = true;
        Player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
    } else if (powerUp.Type == "pass-through") {
        Ball->PassThrough = true;
        Ball->Color = glm::vec3(1.0f, 0.5f, 0.5f);
    } else if (powerUp.Type == "pad-size-increase") {
        Player->Size.x += 60;
    } else if (powerUp.Type == "confuse") {
        if (!Effects->Chaos) {
            Effects->Confuse = true; // only activate if chaos wasn't already active
        }
    } else if (powerUp.Type == "chaos") {
        if (!Effects->Confuse) {
            Effects->Chaos = true;
        }
    }
}

void Game::UpdatePowerUps(float dt) {
    for (PowerUp &powerUp : this->PowerUps) {
        powerUp.Position += powerUp.Velocity * dt;
        if (powerUp.Activated) {
            powerUp.Duration -= dt;
            if (powerUp.Duration <= 0.0f) {
                //remove powerup from list (will later be removed)
                powerUp.Activated = false;
                //deactivate effects
                if (powerUp.Type == "sticky") {
                    if (!IsOtherPowerUpActive(this->PowerUps, "sticky")) {
                        Ball->Sticky = false;
                        Player->Color = glm::vec3(1.0f);
                    }
                } else if (powerUp.Type == "pass-through") {
                    if (!IsOtherPowerUpActive(this->PowerUps, "pass-through")) {
                        Ball->PassThrough = false;
                        Ball->Color = glm::vec3(1.0f);
                    }
                } else if (powerUp.Type == "confuse") {
                    if (!IsOtherPowerUpActive(this->PowerUps, "confuse")) {
                        //only reset if no other Powerup of type confuse is active
                        Effects->Confuse = false;
                    }
                } else if (powerUp.Type == "chaos") {
                    if (!IsOtherPowerUpActive(this->PowerUps, "chaos")) {
                        //only reset if no other Powerup of type chaos is active
                        Effects->Chaos = false;
                    }
                }
            }
        }
    }
    // Remove all PowerUps from vector that are destroyed AND !activated (thus either off the map or finished)
    // Note we use lambda expression to remove each PowerUp which is destroyed and not activated
    this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
                                        [](const PowerUp &powerUp) {
                                            return powerUp.Destroyed && !powerUp.Activated;
                                        }), this->PowerUps.end());
}

void Game::on_click(float x, float y, int idx) {
    if (idx > 0)
        return;
    LOGI("on_click_click x %f y %f", x, y);
    if (this->State == GAME_MENU) {
        if (x - 110 <= Player->Position.x && x + 110 >= Player->Position.x &&
            y - 110 <= Player->Position.y && y + 110 >= Player->Position.y) {
            this->State = GAME_ACTIVE;
            Ball->Stuck = false;
        }
        if (x - 120 <= 1080.0f / 2 && x + 120 >= 1080.0f / 2 &&
            y - 120 <= 1920.0f / 2 && y + 120 >= 1920.0f / 2) {
            this->State = GAME_ACTIVE;
            Ball->Stuck = false;

        }
        if (x > 0 && x < 401 && y > 300 && y < 1601) {
            if (this->State == GAME_MENU) {
                LOGI("left side taped x %f y %f", x, y);
                this->Level = (this->Level + 1) % 3;
            }
        }
        if (x > 1080.0f - 400 && x < 1080.0f && y > 300 && y < 1601) {
            if (this->State == GAME_MENU) {
                LOGI("right side taped x %f y %f", x, y);
                if (this->Level > 0)
                    --this->Level;
                else
                    this->Level = 2;
            }
        }
    }
    if (this->State == GAME_WIN) {
        if (x - 120 <= 1080.0f / 2 && x + 120 >= 1080.0f / 2 &&
            y - 120 <= 1920.0f / 2 && y + 120 >= 1920.0f / 2) {
            this->State = GAME_MENU;
        }
    }
    if (this->State == GAME_ACTIVE) {
        if (x - 110 <= Player->Position.x && x + 110 >= Player->Position.x &&
            y - 110 <= Player->Position.y && y + 110 >= Player->Position.y) {
            this->State = GAME_ACTIVE;
            Ball->Stuck = false;
        }
    }
}

bool IsOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type) {
    // check if another powerUp of the same type is still active
    // in which case we don't disable its effect (yet)
    for (const PowerUp &powerUp: powerUps) {
        if (powerUp.Activated) {
            if (powerUp.Type == type) {
                return true;
            }
        }
    }
    return false;
}

void set_asset_manager(JNIEnv *env, AAssetManager *asset_manager) {
    game.set_asset_manager(env, asset_manager);
}

bool ShouldSpawn(unsigned int chance) {
    unsigned int random = rand() % chance;
    return random == 0;
}
