//
// Created by Maco on 7/30/2020.
//

#include "GameLevel.h"
#include "rapidjson/document.h"
#include <sstream>
#include <iostream>

#define LOG_TAG "GameLevel"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

void GameLevel::Load(const char *file, unsigned int levelWidth, unsigned int levelHeight,
                     unsigned screenHeight) {
    std::string file_content = ResourceManager::Load_file(file);
    rapidjson::Document document;
    document.Parse(file_content.c_str());
    assert(document.IsObject());

    const rapidjson::Value &object_array = document["level"];
    assert(object_array.IsArray());

    std::vector<std::vector<uint>> tile_data;
    for (rapidjson::SizeType i = 0; i < object_array.Size(); ++i) {
        std::ostringstream os;
        os << i;
        std::string key = "line" + os.str();
        if (object_array[i].HasMember(key.c_str())) {
            const rapidjson::Value &vector_item = object_array[i][key.c_str()];
            assert(vector_item.IsArray());
            std::vector<uint> block_line;
            for (rapidjson::SizeType j = 0; j < vector_item.Size(); ++j) {
                uint block_type = vector_item[j].GetUint();
                block_line.emplace_back(block_type);
            }
            tile_data.emplace_back(block_line);
        }
    }
    init(tile_data, levelWidth, levelHeight, screenHeight);
}

void GameLevel::init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth,
                     unsigned int levelHeight, unsigned int screenHeight) {
    if (tileData.empty()) {
        return;
    }
    uint height = tileData.size();
    uint width = tileData[0].size();

    float unit_width = levelWidth / static_cast<float>(width);
    float unit_height = levelHeight / height;
    LOGI("toosgoni width %f height %f", unit_width, unit_height);
//    Texture2D block = ResourceManager::GetTexture("block");
//    float scale = unit_width / block.Width;
//    unit_height = block.Height * scale;
    uint sw = 0;
    uint sh = 0;

    for (uint y = 0; y < height; ++y) {
        for (uint x = 0; x < width; ++x) {

            LOGI("12pos %d %d %f %f",x ,y, unit_width * x, unit_height * y);
            if (tileData[y][x] == 1) {

                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);
                GameObject obj(pos, size, ResourceManager::GetTexture("block_solid"),
                               glm::vec3(0.8f, 0.8f, 0.7f));
                obj.IsSolid = true;
                this->Bricks.push_back(obj);
            } else if (tileData[y][x] > 1) {

                glm::vec3 color(1.0f);

                if (tileData[y][x] == 2)
                    color = glm::vec3(0.2f, 0.6f, 1.0f);
                else if (tileData[y][x] == 3)
                    color = glm::vec3(0.0f, 0.7f, 0.0f);
                else if (tileData[y][x] == 4)
                    color = glm::vec3(0.8f, 0.8f, 0.4f);
                else if (tileData[y][x] == 5)
                    color = glm::vec3(1.0f, 0.5f, 0.0f);

                sw = (uint)unit_width * x;
                sh = (uint)unit_height * y;
                glm::vec2 pos(sw, sh);
                glm::vec2 size(unit_width, unit_height);

                this->Bricks.push_back(
                        GameObject(pos, size, ResourceManager::GetTexture("block"), color));
            }
        }
    }

}

void GameLevel::Draw(SpriteRenderer &renderer) {
    for (GameObject &tile : this->Bricks){
        tile.Draw(renderer);
        LOGI("drawing bricks %f %f", tile.Position.x, tile.Position.y);
    }


}

bool GameLevel::IsCompleted() {
    for (GameObject &tile : this->Bricks)
        if (!tile.IsSolid && !tile.Destroyed) {
            return false;
        }
    return true;
}

