#pragma once

#include "paa_command_state.hpp"
#include "paa_getters.hpp"

#include <spdlog/spdlog.h>

#define PAA_PROGRAM_START_CONTENT(baseScene, resources_file) \
    { \
        paa::setup_paa_system(resources_file);                         \
        PAA_SET_SCENE(baseScene);                                      \
        bool res = PAA_APP.run();                                      \
        paa::stop_paa_system(); \
    }

/**
 * @brief Main loop of the game
 */
#define PAA_PROGRAM_START(baseScene, resources_file, retry)                        \
    int main()                                                                     \
    {                                                                              \
        do {                                                                       \
            try {                                                                  \
                do {                                                               \
                    PAA_PROGRAM_START_CONTENT(baseScene, resources_file);          \
                    if (!res)                                                      \
                        break;                                                     \
                } while (1);                                                       \
                return 0;                                                          \
            } catch (const paa::AABaseError& e) {                                  \
                spdlog::critical("paa::AABaseError: Error: {}", e.what());         \
            } catch (const std::exception& e) {                                    \
                spdlog::critical("std::exception: Error: {}", e.what());           \
            } catch (...) {                                                        \
                spdlog::critical("Unknown error");                                 \
            }                                                                      \
            if (retry) {                                                           \
                spdlog::critical("Retrying in 10 seconds...");                     \
                std::this_thread::sleep_for(std::chrono::seconds(10));             \
            }                                                                      \
        } while (retry);                                                           \
        return 1;                                                                  \
    }

#define PAA_UNSAFE_PROGRAM_START(baseScene, resources_file) \
    int main() { PAA_PROGRAM_START_CONTENT(baseScene, resources_file); }
