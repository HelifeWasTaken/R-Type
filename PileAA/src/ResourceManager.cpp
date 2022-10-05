#include "PileAA/ResourceManager.hpp"
#include "PileAA/external/nlohmann/json.hpp"

#include <fstream>

namespace paa {

    void ResourceManager::remove(const std::string& name)
    {
        auto found = _resourceMap.find(name);
        if (found == _resourceMap.end())
            throw ResourceManagerError("ResourceHolder::remove - Resource not found: " + name);
        _resourceMap.erase(found);
    }

    void ResourceManager::clear()
    {
        _resourceMap.clear();
    }

    void load_configuration_file(const std::string& filename)
    {
        ResourceManager& resourceManager = ResourceManagerInstance::get();

        std::ifstream file(filename);
        if (!file.is_open())
            throw ResourceManagerError("load_configuration_file - Failed to open " + filename);
        nlohmann::json json;
        file >> json;

        if (json.type() != nlohmann::json::value_t::array)
            throw ResourceManagerError("load_configuration_file - Invalid json file");

        // For each resource checks every items
        for (const auto& it : json) {
            if (it.type() != nlohmann::json::value_t::object)
                throw ResourceManagerError("load_configuration_file - Invalid json file");
            if (it.find("type") == it.end() || it.find("path") == it.end()
                || it.find("name") == it.end())
                throw ResourceManagerError("load_configuration_file - Invalid json file");
            try {
                const auto type = it["type"].get<std::string>();
                const auto path = it["path"].get<std::string>();
                const auto name = it["name"].get<std::string>();
                if (type == "texture") resourceManager.load<Texture>(path, name);
                else if (type == "font") resourceManager.load<Font>(path, name);
                else if (type == "sound") resourceManager.load<SoundBuffer>(path, name);
                else if (type == "image") resourceManager.load<Image>(path, name);
                else
                    throw ResourceManagerError("load_configuration_file - Invalid type");
            } catch (const nlohmann::json::exception& e) {
                throw ResourceManagerError("load_configuration_file - Invalid json file");
            }
        }
    }
}