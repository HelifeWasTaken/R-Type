#pragma once

namespace paa {

/**
 * @brief Class for portable random number generation
 * 
 */
class Random {
private:
    static inline int _seed = std::time(nullptr);

public:
    /**
     * @brief Sets the seed for the random number generator
     * 
     * @param seed 
     */
    static void srand(const int& seed) { _seed = seed; }

    /**
     * @brief Returns a random number between INT_MIN and INT_MAX
     * 
     * @return int
     */
    static int rand()
    {
        _seed = (214013 * _seed + 2531011);
        return (_seed >> 16) & 0x7FFF;
    }
};

}