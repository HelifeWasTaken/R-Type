#pragma once

#include "paa_getters.hpp"
#include "paa_command_state.hpp"

#define PAA_PROGRAM_START(baseScene, resources_file) \
    int main() \
    { \
        try { \
            paa::setup_paa_system(); \
            paa::ResourceManager::load_configuration_file(resources_file); \
            PAA_SET_SCENE(baseScene); \
            PAA_APP.run(); \
            paa::stop_paa_system(); \
            return 0; \
        } catch (const std::exception& e) { \
            std::printf("std::exception: Error: %s\n", e.what()); \
            return 1; \
        } catch (const paa::AABaseError& e) { \
            std::printf("paa::AABaseError: Error: %s\n", e.what()); \
            return 1; \
        } catch (...) { \
            std::printf("Unknown error\n"); \
            return 1; \
        } \
    }
