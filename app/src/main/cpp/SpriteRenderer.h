//
// Created by Maco on 7/25/2020.
//

#ifndef MYFIRSTNATIVE_SPRITERENDERER_H
#define MYFIRSTNATIVE_SPRITERENDERER_H


#include <GLES3/gl32.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Texture.h"
#include "Shader.h"
#include <android/log.h>
class SpriteRenderer {
public:
    // Constructor (inits shaders/shapes)
    SpriteRenderer(Shader &shader);

    SpriteRenderer(const Shader &shader, unsigned int quadVao);

    // Destructor
    ~SpriteRenderer();
    // Renders a defined quad textured with given sprite
    void DrawSprite(Texture2D &texture, glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));
private:
    // Render state
    Shader       shader;
    unsigned int quadVAO;
    // Initializes and configures the quad's buffer and vertex attributes
    void initRenderData();
};


#endif //MYFIRSTNATIVE_SPRITERENDERER_H
