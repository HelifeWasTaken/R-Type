#include "PileAA/ResourceManager.hpp"
#include "PileAA/external/nlohmann/json.hpp"

#include <fstream>

namespace paa {

void ResourceManager::remove(const std::string& name)
{
    auto found = _resourceMap.find(name);
    if (found == _resourceMap.end())
        throw ResourceManagerError(
            "ResourceHolder::remove - Resource not found: " + name);
    _resourceMap.erase(found);
}

void ResourceManager::clear() { _resourceMap.clear(); }

}