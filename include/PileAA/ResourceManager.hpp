#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <variant>

#include "Error.hpp"

namespace paa {

HL_SUB_ERROR_IMPL(ResourceManagerError, AABaseError);

/**
 * @brief Resource manager of all loadable resources
 *
 */
class ResourceManager {
private:
    using LoadableResource = std::variant<sf::Texture, sf::Font, sf::SoundBuffer>;
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
        T& ref = std::get<T&>(*resource);

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
        return get<T&>(*found->second);
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

}

