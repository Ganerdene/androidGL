//
// Created by Maco on 9/15/2020.
//

#include <ft2build.h>
#include FT_FREETYPE_H
#include "TextRenderer.h"
#include "ResourceManager.h"
#include "glm/gtc/matrix_transform.hpp"

#include <android/log.h>

#define LOG_TAG "TextRenderer"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

TextRenderer::TextRenderer(unsigned int width, unsigned int height) {
    //load and configure shader
    this->TextShader = ResourceManager::LoadShader("shaders/text.vs", "shaders/text.fs", nullptr, "text");
    this->TextShader.SetMatrix4("projection", glm::ortho(0.0f, static_cast<GLfloat>(width), static_cast<GLfloat>(height), 10.0f),
                                true);
    this->TextShader.SetInteger("text", 0);

    uint a  = glGetAttribLocation(this->TextShader.ID, "vertex");

    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(a);
    glVertexAttribPointer(a, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

void TextRenderer::Load(std::string font, unsigned int fontSize) {
    // first clear the previously loaded Characteres
    this->Characters.clear();
    // then initialize and load the FreeType library
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) { // all functions return a value different that 0 whenever an error occurred
        LOGE("ERROR::FREETYPE: Could not init FreeType Library");
    }else{
        LOGI("SUCCESS::FREETYPE: Could init FreeType Library");
    }
    FileHandle handle = ResourceManager::get_file_data(font.c_str());

    FT_Face face;

    if(FT_New_Memory_Face(ft, (const FT_Byte*)handle.m_buffer, handle.m_size, 0, &face)) {
        LOGE("ERROR::FREETYPE: Failed to load font");
    }
    FT_Set_Pixel_Sizes(face, 0, fontSize);

    // disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // then for the first 128 ASCII characters, pre-load/compile their characters and store them
    for (GLubyte c = 0; c < 128; ++c) {
        // load character glyph
        FT_Error ftError = FT_Load_Char(face, c, FT_LOAD_RENDER);
        if (ftError) {
            LOGE("ERROR::FREETYTPE: Failed to load Glyph %d ", ftError);
            continue;
        }else{
            LOGI("SUCCESS::FREETYTPE: sUCCESS to load Glyph");
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);


        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_ALPHA,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_ALPHA,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        //now store character for later use
        Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                face->glyph->advance.x
        };
        Characters[c] = character;
//        Characters.insert(std::pair<char, Character>(c, character));
        // destroy FreeType once we're finished
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void TextRenderer::RenderText(std::string text, float x, float y, float scale, glm::vec3 color) {
    //activate corresponding render state
    this->TextShader.Use();
    this->TextShader.SetVector3f("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(this->VAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y + (this->Characters['H'].Bearing.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        // update VBO for each character
        float vertices[6][4] = {
                {xpos,     ypos + h, 0.0f, 1.0f},
                {xpos + w, ypos,     1.0f, 0.0f},
                {xpos,     ypos,     0.0f, 0.0f},
                {xpos,     ypos + h, 0.0f, 1.0f},
                {xpos + w, ypos + h, 1.0f, 1.0f},
                {xpos + w, ypos,     1.0f, 0.0f}
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (1/64 times 2^6 = 64)
    }
//    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
