//
// Created by Maco on 8/5/2020.
//

#ifndef MYFIRSTNATIVE_BALLOBJECT_H
#define MYFIRSTNATIVE_BALLOBJECT_H


#include "GameObject.h"

class BallObject : public GameObject{
public:
    // ball state
    float Radius;
    bool Stuck;
    // constructor (s)
    BallObject();
    BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite);
    // moves the ball, keeping it constrained within the window bounds (except bottom edge); returns new position
    glm::vec2 Move(float dt, unsigned int window_width);
    // resets the ball to original  state with given position and velocity
    void Reset(glm::vec2 position, glm::vec2 velocity);
};


#endif //MYFIRSTNATIVE_BALLOBJECT_H
