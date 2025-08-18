#pragma once
#include "includes.hpp"

struct xorshift {
    using result_type = uint64_t;

    uint64_t s[2];

    static constexpr result_type min() { return 0; }
    static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }

    // Uses splitmix64 to set the initial state
    xorshift(uint64_t seed=0) {
        seed += 0x9e3779b97f4a7c15;
        seed = (seed ^ (seed >> 30)) * 0xbf58476d1ce4e5b9;
        seed = (seed ^ (seed >> 27)) * 0x94d049bb133111eb;
        s[0] = seed ^ (seed >> 31);

        seed += 0x9e3779b97f4a7c15;
        seed = (seed ^ (seed >> 30)) * 0xbf58476d1ce4e5b9;
        seed = (seed ^ (seed >> 27)) * 0x94d049bb133111eb;
        s[1] = seed ^ (seed >> 31);
    }
    uint64_t rand() {
        uint64_t a = s[0];
        uint64_t b = s[1];

        s[0] = b;
        a ^= a << 23;
        a ^= a >> 18;
        a ^= b;
        a ^= b >> 5;
        s[1] = a;

        return a + b;
    }

    result_type operator()() { return rand(); }
};