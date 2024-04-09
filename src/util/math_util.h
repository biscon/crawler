//
// Created by bison on 10/13/18.
//

#ifndef GAME_MATH_UTIL_H
#define GAME_MATH_UTIL_H
#define PI 3.1415926535897f

#include <defs.h>
#include <stdlib.h>

double remapDouble(double oMin, double oMax, double nMin, double nMax, double x);
float remapFloat(float oMin, float oMax, float nMin, float nMax, float x);

// *Really* minimal PCG32 code / (c) 2014 M.E. O'Neill / pcg-random.org
// Licensed under Apache License 2.0 (NO WARRANTY, etc. see website)
typedef struct { uint64_t state;  uint64_t inc; } pcg32_random_t;

static inline u32 pcg32_random_r(pcg32_random_t* rng)
{
    uint64_t oldstate = rng->state;
    // Advance INTERNAL state
    rng->state = oldstate * 6364136223846793005ULL + (rng->inc|1);
    // Calculate output function (XSH RR), uses old state for max ILP
    u32 xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    u32 rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

float lerp(float a, float b, float f);
float decelerate_lerp(float a, float b, float f);
float accelerate_lerp(float a, float b, float f);


#endif //GAME_MATH_UTIL_H
