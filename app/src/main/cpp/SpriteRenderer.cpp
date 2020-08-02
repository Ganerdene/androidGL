//
// Created by Maco on 7/25/2020.
//

#include "SpriteRenderer.h"
#define LOG_TAG "SPriteRenderer"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

SpriteRenderer::SpriteRenderer(Shader &shader) {
    this->shader = shader;
    this->initRenderData();
}

SpriteRenderer::~SpriteRenderer() {
    glDeleteVertexArrays(1, &this->quadVAO);
}

void
SpriteRenderer::DrawSprite(Texture2D &texture, glm::vec2 position, glm::vec2 size, float rotate,glm::vec3 color) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
   // glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    this->shader.Use();
       glm::mat4 model{
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
    };
    model = glm::translate(model, glm::vec3(position,0.0f));  // first translate (transformations are: scale happens first, then rotation, and then final translation happens; reversed order)
    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y,0.0f)); // move origin of rotation to center of quad
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); // then rotate
    model = glm::translate(model,glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // move origin back
    model = glm::scale(model, glm::vec3(size, 1.0f)); // last scale
    this->shader.SetMatrix4("model", model);
    this->shader.SetVector3f("spriteColor", color);
    glActiveTexture(GL_TEXTURE0);
    texture.Bind();
    glBindBuffer(GL_ARRAY_BUFFER, this->quadVAO);
    glEnableVertexAttribArray(0);
    //glBindVertexArray(this->quadVAO);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (GLvoid *) 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

}

void SpriteRenderer::initRenderData() {
    // configure VAO/VBO
    unsigned int VBO;
    float vertices[] = {
            // pos      // tex
            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,

            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &this->quadVAO);

    glGenBuffers(1, &VBO);

    __android_log_print(ANDROID_LOG_INFO, "Breakout", "glGetAttribLocation(\"vPosition\") = %d\n",
                        quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(this->quadVAO);

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

}

