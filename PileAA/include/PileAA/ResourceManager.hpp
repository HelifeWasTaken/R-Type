#pragma once

#include <memory>
#include <unordered_map>
#include <variant>

#include <spdlog/spdlog.h>

#include "Error.hpp"
#include "FrameBuffer.hpp"
#include "Types.hpp"

namespace paa {

HL_SUB_ERROR_IMPL(ResourceManagerError, AABaseError);

/**
 * @brief Resource manager of all loadable resources
 *
 */
class ResourceManager {
private:
    using LoadableResource = std::variant<Texture, Font, SoundBuffer, Image>;
    using ResourceHolder
        = std::unordered_map<std::string, std::unique_ptr<LoadableResource>>;

    ResourceHolder _resourceMap;

    static inline const char* DEFAULT_TEXTURE
        = "__paa_resource_manager_default_texture__";
    static inline const char* DEFAULT_IMAGE
        = "__paa_resource_manager_default_image__";

public:
    /**
     * @brief Construct a new Resource Manager object
     */
    ResourceManager()
    {
        copyAs(DEFAULT_TEXTURE, FrameBuffer::generateDefaultTexture());
        copyAs(DEFAULT_IMAGE, FrameBuffer::generateDefaultImage());
    }

    /**
     * @brief Destroy the Resource Manager object
     */
    ~ResourceManager() = default;

    /**
     * @brief Returns the default texture
     */
    Texture& getDefaultTexture() { return get<Texture>(DEFAULT_TEXTURE); }

    /**
     * @brief Returns the default image
     */
    Image& getDefaultImage() { return get<Image>(DEFAULT_IMAGE); }

    /**
     * @brief Load a resource
     *
     * @tparam T Type of the resource
     * @param filename Where to load the resource
     * @param name The name of the resource
     * @param loadArgs Additionals arguments
     */
    template <typename T, typename... LoadArgs>
    void load(const std::string& filename, const std::string& name,
        LoadArgs&&... loadArgs)
    {
        LoadableResource* resource = new LoadableResource(T());
        T& ref = std::get<T>(*resource);

        if (!ref.loadFromFile(filename, std::forward<LoadArgs>(loadArgs)...))
            spdlog::error(
                "ResourceHolder::load - Failed to load \"{}\"", filename);
        _resourceMap[name] = std::unique_ptr<LoadableResource>(resource);
    }

    /**
     * @brief Copy as
     * @tparam T Type of the resource
     * @param name The name of the resource
     * @param resource The resource to copy
     */
    template <typename T>
    void copyAs(const std::string& name, const T& resource)
    {
        _resourceMap[name] = std::make_unique<LoadableResource>(resource);
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
        if (found == _resourceMap.end()) {
            if constexpr (std::is_same_v<T, Texture>) {
                spdlog::warn("ResourceHolder::get - Texture not found: \"{}\", "
                             "using default texture",
                    name);
                return getDefaultTexture();
            } else if constexpr (std::is_same_v<T, Image>) {
                spdlog::warn("ResourceHolder::get - Image not found: \"{}\", "
                             "using default image",
                    name);
                return getDefaultImage();
            } else {
                throw ResourceManagerError(
                    "ResourceHolder::get - Resource not found: " + name);
            }
        }
        return std::get<T>(*found->second);
    }

    /**
     * @brief Remove a resource of a specific name
     *
     * @param name The name of the resource
     */
    void remove(const std::string& name);

    /**
     * @brief Clears the resource holder
     */
    void clear();
};

/**
 * @brief Construct a new hl singleton impl object
 */
HL_SINGLETON_IMPL(ResourceManager, ResourceManagerInstance);

}
