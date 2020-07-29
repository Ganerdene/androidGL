//
// Created by Maco on 7/23/2020.
//
#define STB_IMAGE_IMPLEMENTATION
#include "ResourceManager.h"
#include "stb_image.h"
#include <android/asset_manager_jni.h>
#include <GLES3/gl32.h>
#include <vector>
#include <map>

#include <android/log.h>
#define LOG_TAG "ResourceManager"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

AAssetManager *ResourceManager::m_asset_manager;
std::map<std::string, Shader> ResourceManager::Shaders;
std::map<std::string, Texture2D> ResourceManager::Textures;

Texture2D ResourceManager::LoadTextureFromFile(const char *file, bool alpha) {
    Texture2D texture2D;
    if (m_asset_manager != nullptr) {
        LOGI("working load texture m_asset_manager NOT NULL");
        AAsset *asset = AAssetManager_open(m_asset_manager, file, AASSET_MODE_UNKNOWN);
        if (asset) {

            auto size = AAsset_getLength(asset);
            auto data = new unsigned char[size];
            int32_t readSize = AAsset_read(asset, data, (size_t) size);

            if (readSize != size) {
                delete[] data;

                return texture2D;
            }

            stbi_set_flip_vertically_on_load(0);
            int32_t width, height, channel_count;
          //  uint8_t *img_buf;
            unsigned char *img_buf;
            LOGI("asset__ img_buf before %d ", img_buf);
            img_buf = stbi_load_from_memory(data, size, &width, &height, &channel_count, 3);
            LOGI("asset__ img_buf after %d ", img_buf);
            texture2D.Height = height;
            texture2D.Width = width;
            texture2D.Image_Format = 1;
            texture2D.Internal_Format = 1;
            texture2D.Generate(width, height, img_buf);
        }
    }
    return texture2D;
}

Texture2D ResourceManager::LoadTexture(const char *file, bool alpha, std::string name) {
    Textures[name] = LoadTextureFromFile(file, alpha);
    return Textures[name];
}

Texture2D ResourceManager::GetTexture(std::string name) {
    return Textures[name];
}

Shader ResourceManager::LoadShaderFromFile(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile) {

    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;

    vertexCode = Load_file(vShaderFile);
    fragmentCode = Load_file(fShaderFile);
    if (gShaderFile != nullptr) {
        geometryCode = Load_file(gShaderFile);
    }
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();
    LOGI("qweqwe before geometery shader");
    const char *gShaderCode = geometryCode.c_str();

    Shader shader;
    shader.Compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
    LOGI("shader have error? %d", glGetError());
    return shader;
}

Shader ResourceManager::LoadShader(const char *vShaderFile, const char *fShaderFile,
                                   const char *gShaderFile, std::string name) {
    Shaders[name] = LoadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
    return Shaders[name];
}

Shader ResourceManager::GetShader(std::string name) {
    return Shaders[name];
}

void ResourceManager::Clear() {
    // (properly) delete all shaders
    for (auto iter : Shaders)
        glDeleteProgram(iter.second.ID);
    // (properly) delete all textures
    for (auto iter : Textures)
        glDeleteTextures(1, &iter.second.ID);
}

std::string ResourceManager::Load_file(const GLchar *file_path) {
    std::string file_content;
    AAsset *assetFile = AAssetManager_open(m_asset_manager, file_path, AASSET_MODE_STREAMING);
    LOGI("Trying to load shaders");
    uint8_t *data = (uint8_t *) AAsset_getBuffer(assetFile);
    int32_t size = AAsset_getLength(assetFile);
    if (data == NULL) {
        AAsset_close(assetFile);
    } else {
        std::vector<uint8_t> buffer_ref;
        buffer_ref.reserve(size);
        buffer_ref.assign(data, data + size);
        AAsset_close(assetFile);
        file_content.assign(buffer_ref.begin(), buffer_ref.end());
    }
    LOGI("Trying to loaded shader %s", file_content.c_str());
    return file_content;
}

void ResourceManager::set_asset_manager(AAssetManager *asset_manager) {
    m_asset_manager = asset_manager;
}