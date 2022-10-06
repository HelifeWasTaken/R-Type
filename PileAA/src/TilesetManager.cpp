#include "PileAA/external/nlohmann/json.hpp"
#include "PileAA/TilesetManager.hpp"
#include <string>
#include <filesystem>
#include <fstream>

namespace paa {

    TilesetManager::TilesetManager(const std::string& filename)
    {
        std::string filenameNoEnding = filename.substr(0, filename.find_last_of('/'));
        std::ifstream file(filename);

        nlohmann::json json;
        file >> json;

        for (const auto& it : json["tilesets"]) {
            auto tileset = std::make_unique<Tileset>();
            if (tileset->texture.loadFromFile(filenameNoEnding + "/" + (std::string)it["image"]) == false) {
                throw std::runtime_error("Failed to load tileset: " + (std::string)it["image"]);
            }
            tileset->tileWidth = it["tilewidth"];
            tileset->tileHeight = it["tileheight"];
            tileset->firstGid = it["firstgid"];
            tileset->tileCount = it["tilecount"];
            _tilesets.push_back(std::move(tileset));
        }

        for (auto& it : json["layers"]) {
            int tilesetIndex = 0;

            if (it["type"] != "tilelayer") {
                continue;
            }
            if (it["visible"] != true) {
                continue;
            }

            const auto data = it["data"];

            for (uint64_t i = 0; i < data.size(); ++i) {
                int gid = data[i];
                int pureGid = gid & ~(FLIP_H_FLAG | FLIP_V_FLAG | FLIP_D_FLAG);
                if (data[i] != 0) {
                    for (size_t j = 0; j < _tilesets.size(); ++j) {
                        if ((pureGid >= _tilesets[j]->firstGid) &&
                            (pureGid < _tilesets[j]->firstGid + _tilesets[j]->tileCount)) {
                            tilesetIndex = j;
                            break;
                        }
                    }
                    break;
                }
            }
            auto layer = std::unique_ptr<std::pair<Texture&, VertexArray>>(
                new std::pair<Texture&, VertexArray>(
                    _tilesets[tilesetIndex]->texture,
                    VertexArray()
                )
            );

            unsigned int width = it["width"];
            unsigned int height = it["height"];

            layer->second.setPrimitiveType(sf::Quads);
            layer->second.resize(width * height * 4);

            for (uint64_t i = 0; i < width; i++) {
                for (uint64_t j = 0; j < height; j++) {
                    int rawId = data[i + j * width];
                    int tileNumber = (rawId & ~(FLIP_H_FLAG | FLIP_V_FLAG | FLIP_D_FLAG)) - _tilesets[tilesetIndex]->firstGid;
                    Vertex* vertices = &layer->second[(i + j * width) * 4];

                    int tu = tileNumber % (layer->first.getSize().x / _tilesets[tilesetIndex]->tileWidth);
                    int tv = tileNumber / (layer->first.getSize().x / _tilesets[tilesetIndex]->tileWidth);

                    vertices[0].position = Vector2f(i * _tilesets[tilesetIndex]->tileWidth, j * _tilesets[tilesetIndex]->tileHeight);
                    vertices[1].position = Vector2f((i + 1) * _tilesets[tilesetIndex]->tileWidth, j * _tilesets[tilesetIndex]->tileHeight);
                    vertices[2].position = Vector2f((i + 1) * _tilesets[tilesetIndex]->tileWidth, (j + 1) * _tilesets[tilesetIndex]->tileHeight);
                    vertices[3].position = Vector2f(i * _tilesets[tilesetIndex]->tileWidth, (j + 1) * _tilesets[tilesetIndex]->tileHeight);

                    if (tileNumber < 0) {
                        vertices[0].color = Color(0, 0, 0, 0);
                        vertices[1].color = Color(0, 0, 0, 0);
                        vertices[2].color = Color(0, 0, 0, 0);
                        vertices[3].color = Color(0, 0, 0, 0);
                    } else if (rawId < FLIP_D_FLAG) {
                        vertices[0].texCoords = Vector2f(tu * _tilesets[tilesetIndex]->tileWidth, tv * _tilesets[tilesetIndex]->tileHeight);
                        vertices[1].texCoords = Vector2f((tu + 1) * _tilesets[tilesetIndex]->tileWidth, tv * _tilesets[tilesetIndex]->tileHeight);
                        vertices[2].texCoords = Vector2f((tu + 1) * _tilesets[tilesetIndex]->tileWidth, (tv + 1) * _tilesets[tilesetIndex]->tileHeight);
                        vertices[3].texCoords = Vector2f(tu * _tilesets[tilesetIndex]->tileWidth, (tv + 1) * _tilesets[tilesetIndex]->tileHeight);
                    } else if ((rawId & FLIP_H_FLAG) && (rawId & FLIP_D_FLAG)) {
                        vertices[1].texCoords = Vector2f(tu * _tilesets[tilesetIndex]->tileWidth, tv * _tilesets[tilesetIndex]->tileHeight);
                        vertices[2].texCoords = Vector2f((tu + 1) * _tilesets[tilesetIndex]->tileWidth, tv * _tilesets[tilesetIndex]->tileHeight);
                        vertices[3].texCoords = Vector2f((tu + 1) * _tilesets[tilesetIndex]->tileWidth, (tv + 1) * _tilesets[tilesetIndex]->tileHeight);
                        vertices[0].texCoords = Vector2f(tu * _tilesets[tilesetIndex]->tileWidth, (tv + 1) * _tilesets[tilesetIndex]->tileHeight);
                    } else if ((rawId & FLIP_H_FLAG) && (rawId & FLIP_V_FLAG)) {
                        vertices[2].texCoords = Vector2f(tu * _tilesets[tilesetIndex]->tileWidth, tv * _tilesets[tilesetIndex]->tileHeight);
                        vertices[3].texCoords = Vector2f((tu + 1) * _tilesets[tilesetIndex]->tileWidth, tv * _tilesets[tilesetIndex]->tileHeight);
                        vertices[0].texCoords = Vector2f((tu + 1) * _tilesets[tilesetIndex]->tileWidth, (tv + 1) * _tilesets[tilesetIndex]->tileHeight);
                        vertices[1].texCoords = Vector2f(tu * _tilesets[tilesetIndex]->tileWidth, (tv + 1) * _tilesets[tilesetIndex]->tileHeight);
                    } else {
                        vertices[3].texCoords = Vector2f(tu * _tilesets[tilesetIndex]->tileWidth, tv * _tilesets[tilesetIndex]->tileHeight);
                        vertices[0].texCoords = Vector2f((tu + 1) * _tilesets[tilesetIndex]->tileWidth, tv * _tilesets[tilesetIndex]->tileHeight);
                        vertices[1].texCoords = Vector2f((tu + 1) * _tilesets[tilesetIndex]->tileWidth, (tv + 1) * _tilesets[tilesetIndex]->tileHeight);
                        vertices[2].texCoords = Vector2f(tu * _tilesets[tilesetIndex]->tileWidth, (tv + 1) * _tilesets[tilesetIndex]->tileHeight);
                    }
                }
            }
            _layers.push_back(std::move(layer));
        }
    }

    void TilesetManager::draw(RenderTarget& target, RenderStates states) const {
        states.transform *= getTransform();
        for (auto& layer : _layers) {
            states.texture = &layer->first;
            target.draw(layer->second, states);
        }
    }

    void TilesetManager::drawLayer(RenderWindow& window, size_t layerIndex) const {
        window.draw(_layers[layerIndex]->second,
            RenderStates(sf::BlendAlpha, Transform::Identity,
                        &_layers[layerIndex]->first, nullptr));
    }

    size_t TilesetManager::layerCount() const {
        return _layers.size();
    }
}