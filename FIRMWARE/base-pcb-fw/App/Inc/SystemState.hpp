#pragma once

#include <cstdint>
#include "ChannelManager.hpp"

struct SystemState {
    // temperatures from ADC
    int32_t temperature[TC_NUM_CHANNELS];

    // channel configuration (mirrored for C access)
    ChannelConfig ch_config[TC_NUM_CHANNELS];
};

extern SystemState systemState;
