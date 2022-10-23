namespace paa {

    class Random {
    private:
        static inline int _seed = std::time(nullptr);

    public:
        static void srand(const int& seed) {
            _seed = seed;
        }

        static int rand() {
            _seed = (214013 * _seed + 2531011);
            return (_seed >> 16) & 0x7FFF;
        }
    };

}