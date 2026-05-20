#pragma once

#include <cstdint>

struct SystemState
{
    int32_t temperature[8];
};

extern SystemState systemState;
