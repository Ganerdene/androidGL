//
// Created by Maco on 8/24/2020.
//

#ifndef MYFIRSTNATIVE_PARTICLEGENERATOR_H
#define MYFIRSTNATIVE_PARTICLEGENERATOR_H

#include <vector>
#include "glm/glm.hpp"

#include "shader.h"
#include "texture.h"
#include "GameObject.h"

struct Particle {
    glm::vec2 Position, Velocity;
    glm::vec4 Color;
    float     Life;

    Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
};

class ParticleGenerator {
public:
    // constructor
    ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount);
    void Update(float dt, GameObject &object, unsigned int newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
    void Draw();
private:
    // state
    std::vector<Particle> particles;
    unsigned int amount;
    // render state
    Shader shader;
    Texture2D texture;
    unsigned int VAO;
    // initializes buffer and vertex attributes
    void init();
    // returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
    unsigned int firstUnusedParticle();
    // respawns particle
    void respawnParticle(Particle &particle, GameObject &object, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
};


#endif //MYFIRSTNATIVE_PARTICLEGENERATOR_H
