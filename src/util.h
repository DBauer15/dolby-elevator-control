#ifndef UTIL_H
#define UTIL_H

#include <chrono>
#include <stdlib.h>
#include "elevator.h"

typedef std::chrono::high_resolution_clock::time_point timepoint_t;

static timepoint_t time_now() {
    return std::chrono::high_resolution_clock::now();
}

template <typename duration_t>
static double duration(timepoint_t a, timepoint_t b) {
    return std::chrono::duration_cast<duration_t>(b - a).count();
}

static float randrange(float min, float max) {
    float r = (float)std::rand()/RAND_MAX;

    return min + r * (max-min);
}

static float rand01() {
    return randrange(0.0f, 1.0f);
}

#endif