#pragma once

#include "paa_getters.hpp"
#include "paa_command_state.hpp"

#include <spdlog/spdlog.h>

#define PAA_PROGRAM_START(baseScene, resources_file) \
    int main() \
    { \
        try { \
            do { \
                paa::setup_paa_system(resources_file); \
                PAA_SET_SCENE(baseScene); \
                bool res = PAA_APP.run(); \
                paa::stop_paa_system(); \
                if (!res) break; \
            } while (1); \
            return 0; \
        } catch (const std::exception& e) { \
            spdlog::error("std::exception: Error: {}", e.what()); \
            return 1; \
        } catch (const paa::AABaseError& e) { \
            spdlog::error("paa::AABaseError: Error: {}", e.what()); \
            return 1; \
        } catch (...) { \
            spdlog::error("Unknown error"); \
            return 1; \
        } \
    }
