#include <random>

namespace Random
{
    inline std::mt19937 &get_engine()
    {
        static thread_local std::mt19937 engine(std::random_device{}());
        return engine;
    }

   
    inline int get_int(int min, int max)
    {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(get_engine());
    }

    inline float get_float(float min, float max){
        std::uniform_real_distribution<float> dist(min, max);
        return dist(get_engine());
    }
}