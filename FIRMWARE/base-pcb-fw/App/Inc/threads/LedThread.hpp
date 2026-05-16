#pragma once

extern "C" {
#include "tx_api.h"
}

class LedThread
{
public:
    static void entry(ULONG arg);

private:
    static constexpr ULONG sleepTicks = 50;
};