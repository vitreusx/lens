#pragma once
#include <random>
#include <vector>
using namespace std;

class Random {
public:
    random_device dev;
    default_random_engine eng;

    Random() {
        eng = default_random_engine(dev());
    }

    float uniform(float low, float high) {
        return uniform_real_distribution<float>(low, high)(eng);
    }
};
