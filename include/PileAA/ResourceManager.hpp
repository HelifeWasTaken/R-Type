#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <variant>
#include <fstream>
#include "external/nlohmann/json.hpp"

#include "Error.hpp"
#include "Types.hpp"

namespace paa {

HL_SUB_ERROR_IMPL(ResourceManagerError, AABaseError);

/**
 * @brief Resource manager of all loadable resources
 *
 */
class ResourceManager {
private:
    using LoadableResource = std::variant<Texture, Font,
                                        SoundBuffer, Image>;
    using ResourceHolder    = std::unordered_map<std::string,
                            std::unique_ptr<LoadableResource>>;

    ResourceHolder _resourceMap;

public:
    /**
     * @brief Load a resource
     *
     * @tparam T Type of the resource
     * @param filename Where to load the resource
     * @param name The name of the resource
     * @param loadArgs Additionals arguments
     */
    template <typename T, typename... LoadArgs>
    void load(
        const std::string& filename, const std::string& name, LoadArgs&&... loadArgs)
    {
        LoadableResource* resource = new LoadableResource(T());
        T& ref = std::get<T>(*resource);

        if (!ref.loadFromFile(filename, std::forward<LoadArgs>(loadArgs)...))
            throw ResourceManagerError("ResourceHolder::load - Failed to load " + filename);
        _resourceMap[name] = std::unique_ptr<LoadableResource>(resource);
    }

    /**
     * @brief Get a resource from the name
     *
     * @tparam T Type to get
     * @param name Name of the resource
     * @return T& A ref to the resource
     */
    template <typename T> T& get(const std::string& name)
    {
        auto found = _resourceMap.find(name);
        if (found == _resourceMap.end())
            throw ResourceManagerError("ResourceHolder::get - Resource not found: " + name);
        return std::get<T>(*found->second);
    }

    /**
     * @brief Remove a resource of a specific name
     *
     * @param name The name of the resource
     */
    void remove(const std::string& name)
    {
        auto found = _resourceMap.find(name);
        if (found == _resourceMap.end())
            throw ResourceManagerError("ResourceHolder::remove - Resource not found: " + name);
        _resourceMap.erase(found);
    }

    /**
     * @brief Clears the resource holder
     */
    void clear() { _resourceMap.clear(); }
};

HL_SINGLETON_IMPL(ResourceManager, ResourceManagerInstance);

static inline void load_configuration_file(const std::string& filename)
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