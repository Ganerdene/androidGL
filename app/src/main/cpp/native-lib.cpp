#include <jni.h>
#include <android/log.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <android/asset_manager_jni.h>

#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Matrix.cpp"

#define LOG_TAG "libNative"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

GLuint simpleCube;
GLuint vertexLocation;
GLuint samplerLocation;
GLuint projectionLocation;
GLuint modelViewLocation;
GLuint textureCordLocation;
GLuint textureId;

AAssetManager *assetManager;
float cx = 0;
float cy = 0;

float projectionMatrix[16];
float modelViewMatrix[16];
float angle = 0;

GLfloat cubeVertices[] = {-1.0f, 1.0f, -1.0f, /* Back. */
                          1.0f, 1.0f, -1.0f,
                          -1.0f, -1.0f, -1.0f,
                          1.0f, -1.0f, -1.0f,
                          -1.0f, 1.0f, 1.0f, /* Front. */
                          1.0f, 1.0f, 1.0f,
                          -1.0f, -1.0f, 1.0f,
                          1.0f, -1.0f, 1.0f,
                          -1.0f, 1.0f, -1.0f, /* Left. */
                          -1.0f, -1.0f, -1.0f,
                          -1.0f, -1.0f, 1.0f,
                          -1.0f, 1.0f, 1.0f,
                          1.0f, 1.0f, -1.0f, /* Right. */
                          1.0f, -1.0f, -1.0f,
                          1.0f, -1.0f, 1.0f,
                          1.0f, 1.0f, 1.0f,
                          -1.0f, -1.0f, -1.0f, /* Top. */
                          -1.0f, -1.0f, 1.0f,
                          1.0f, -1.0f, 1.0f,
                          1.0f, -1.0f, -1.0f,
                          -1.0f, 1.0f, -1.0f, /* Bottom. */
                          -1.0f, 1.0f, 1.0f,
                          1.0f, 1.0f, 1.0f,
                          1.0f, 1.0f, -1.0f
};

GLfloat colour[] = {1.0f, 0.0f, 0.0f,
                    1.0f, 0.0f, 0.0f,
                    1.0f, 0.0f, 0.0f,
                    1.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f,
                    0.0f, 1.0f, 0.0f,
                    0.0f, 1.0f, 0.0f,
                    0.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, 1.0f,
                    0.0f, 0.0f, 1.0f,
                    0.0f, 0.0f, 1.0f,
                    0.0f, 0.0f, 1.0f,
                    1.0f, 1.0f, 0.0f,
                    1.0f, 1.0f, 0.0f,
                    1.0f, 1.0f, 0.0f,
                    1.0f, 1.0f, 0.0f,
                    0.0f, 1.0f, 1.0f,
                    0.0f, 1.0f, 1.0f,
                    0.0f, 1.0f, 1.0f,
                    0.0f, 1.0f, 1.0f,
                    1.0f, 0.0f, 1.0f,
                    1.0f, 0.0f, 1.0f,
                    1.0f, 0.0f, 1.0f,
                    1.0f, 0.0f, 1.0f
};


GLfloat textureCords[] = {1.0f, 1.0f, /* Back. */
                          0.0f, 1.0f,
                          1.0f, 0.0f,
                          0.0f, 0.0f,
                          0.0f, 1.0f, /* Front. */
                          1.0f, 1.0f,
                          0.0f, 0.0f,
                          1.0f, 0.0f,
                          0.0f, 1.0f, /* Left. */
                          0.0f, 0.0f,
                          1.0f, 0.0f,
                          1.0f, 1.0f,
                          1.0f, 1.0f, /* Right. */
                          1.0f, 0.0f,
                          0.0f, 0.0f,
                          0.0f, 1.0f,
                          0.0f, 1.0f, /* Top. */
                          0.0f, 0.0f,
                          1.0f, 0.0f,
                          1.0f, 1.0f,
                          0.0f, 0.0f, /* Bottom. */
                          0.0f, 1.0f,
                          1.0f, 1.0f,
                          1.0f, 0.0f
};

GLushort indices[] = {0, 2, 3, 0, 1, 3, 4, 6, 7, 4, 5, 7, 8, 9, 10, 11, 8, 10, 12, 13, 14, 15, 12,
                      14, 16, 17, 18, 16, 19, 18, 20, 21, 22, 20, 23, 22};

static const char glVertexShader[] =
        "attribute vec4 vertexPosition;\n"
        "attribute vec2 vertexTextureCord;\n"
        "varying vec2 textureCord;\n"
        "uniform mat4 projection;\n"
        "uniform mat4 modelView;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = projection * modelView * vertexPosition;\n"
        "    textureCord = vertexTextureCord;\n"
        "}\n";

static const char glFragmentShader[] =
        "precision mediump float;\n"
        "uniform sampler2D texture;\n"
        "varying vec2 textureCord;\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor = texture2D(texture, textureCord);\n"
        "}\n";

GLuint loadShader(GLenum shaderType, const char *shaderSource) {
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
        glShaderSource(shader, 1, &shaderSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infolen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infolen);
            if (infolen) {
                char *buf = (char *) malloc(infolen);
                if (buf) {
                    glGetShaderInfoLog(shader, infolen, NULL, buf);
                    LOGE("Could not Compile Shader %d:\n%s\n", shaderType, buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

GLuint loadSimpleTexture() {
    GLuint textureId;
    GLubyte pixels[9 * 4] = {
            18, 140, 171, 255, /* Some Colour Bottom Left. */
            143, 143, 143, 255, /* Some Colour Bottom Middle. */
            255, 255, 255, 255, /* Some Colour Bottom Right. */
            255, 255, 0, 255, /* Yellow Middle Left. */
            0, 255, 255, 255, /* Some Colour Middle. */
            255, 0, 255, 255, /* Some Colour Middle Right. */
            255, 0, 0, 255, /* Red Top Left. */
            0, 255, 0, 255, /* Green Top Middle. */
            0, 0, 255, 255, /* Blue Top Right. */
    };
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &textureId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 3, 3, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    return textureId;
}


GLuint createProgram(const char *vertexSource, const char *fragmentSource) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexSource);
    if (!vertexShader) {
        return 0;
    }
    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (!fragmentShader) {
        return 0;
    }
    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);

        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char *buf = (char *) malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    LOGE("Could not link program: \n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}

GLuint loadImage(std::string path) {
    GLuint texture;
    if (assetManager != nullptr) {
        AAsset *asset = AAssetManager_open(assetManager, path.c_str(), AASSET_MODE_UNKNOWN);
        LOGI("assetManager is not null");

        if (asset) {
            auto size = AAsset_getLength(asset);
            auto data = new unsigned char[size];
            int32_t readSize = AAsset_read(asset, data, (size_t) size);
            LOGI("asset__ size %d ", size);
            if (readSize != size) {
                delete[] data;

                return 0;
            }
            stbi_set_flip_vertically_on_load(1);
            int32_t  width, height, channel_count;
            uint8_t* img_buf;
            LOGI("asset__ img_buf before %d ", img_buf);
            img_buf = stbi_load_from_memory(data, size, &width, &height, &channel_count, 0);
            LOGI("asset__ img_buf after %d ", img_buf);
            glGenTextures(1, &texture);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_buf);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        } else {
            LOGI("asset null", "qweqwe");
        }
    } else {
        LOGI("assetManager is null");
    }
    LOGI("assetManager texture %d", texture);
    return texture;
}

bool setupGraphics(int w, int h) {
    std::string stw = std::to_string(w);
    std::string sth = std::to_string(h);
    stw = stw + " " + sth;
    simpleCube = createProgram(glVertexShader, glFragmentShader);
    if (!simpleCube) {
        LOGE("Could not create program");
        return false;
    }
    vertexLocation = glGetAttribLocation(simpleCube, "vertexPosition");
    textureCordLocation = glGetAttribLocation(simpleCube, "vertexTextureCord");

    projectionLocation = glGetUniformLocation(simpleCube, "projection");
    modelViewLocation = glGetUniformLocation(simpleCube, "modelView");
    samplerLocation = glGetUniformLocation(simpleCube, "texture");

    matrixPerspective(projectionMatrix, 45, (float) w / (float) h, 0.1f, 100);

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, w, h);

    textureId = loadImage("textures/net.png");

    if (textureId == 0) {
        return false;
    } else {
        return true;
    }
}


void renderFrame() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    matrixIdentityFunction(modelViewMatrix);
    matrixRotateX(modelViewMatrix, angle);
    matrixRotateY(modelViewMatrix, angle);

    matrixTranslate(modelViewMatrix, cx * 4, cy * 4, -20.0f);

    glUseProgram(simpleCube);

    glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, 0, cubeVertices);
    glEnableVertexAttribArray(vertexLocation);

//    glVertexAttribPointer(vertexColourLocation, 3, GL_FLOAT, GL_FALSE, 0, colour);
//    glEnableVertexAttribArray(vertexColourLocation);

    glVertexAttribPointer(textureCordLocation, 2, GL_FLOAT, GL_FALSE, 0, textureCords);
    glEnableVertexAttribArray(textureCordLocation);

    glUniform1i(samplerLocation, 0);

    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, projectionMatrix);
    glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, modelViewMatrix);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, indices);

    angle += 1;
    if (angle > 360) {
        angle -= 360;
    }
}

extern "C" {
JNIEXPORT void JNICALL
Java_com_example_myfirstnative_MainActivity_init(JNIEnv *env, jobject obj, jint width, jint height);
JNIEXPORT void JNICALL Java_com_example_myfirstnative_MainActivity_step(JNIEnv *env, jobject obj);
JNIEXPORT void JNICALL
Java_com_example_myfirstnative_MainActivity_touch(JNIEnv *env, jclass clazz, jfloat x, jfloat y);
JNIEXPORT jint JNICALL
Java_com_example_myfirstnative_MainActivity_loadTexture(JNIEnv *env, jobject thiz,
                                                        jobject asset_manager, jstring file_name);
};

/* [Native functions] */
JNIEXPORT void JNICALL
Java_com_example_myfirstnative_MainActivity_init(JNIEnv *env, jobject obj, jint width,
                                                 jint height) {
    setupGraphics(width, height);
}

JNIEXPORT void JNICALL Java_com_example_myfirstnative_MainActivity_step(JNIEnv *env, jobject obj) {
    renderFrame();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_myfirstnative_MainActivity_touch(JNIEnv *env, jclass clazz, jfloat x, jfloat y) {
    // TODO: implement touch()
    cx = x;
    cy = y;
}extern "C"
JNIEXPORT jint JNICALL
Java_com_example_myfirstnative_MainActivity_loadTexture(JNIEnv *env, jobject thiz,
                                                        jobject asset_manager, jstring file_name) {
    // TODO: implement loadTexture()

    assetManager = AAssetManager_fromJava(env, asset_manager);

    return 1;
}