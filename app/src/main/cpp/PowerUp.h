//
// Created by Maco on 9/1/2020.
//

#ifndef MYFIRSTNATIVE_POWERUP_H
#define MYFIRSTNATIVE_POWERUP_H


#include "GameObject.h"

const glm::vec2 SIZE(120.0f, 40.0f);
const glm::vec2 VELOCITY(0.0f, 150.0f);

class PowerUp : public GameObject {
public:
    // power state
    std::string Type;
    float Duration;
    bool Activated;

    // constructor
    PowerUp(std::string type, glm::vec3 color, float duration,
            glm::vec2 position, Texture2D texture) :
            GameObject(position, SIZE, texture, color, VELOCITY),
            Type(type), Duration(duration), Activated() {
    }

};


#endif //MYFIRSTNATIVE_POWERUP_H
