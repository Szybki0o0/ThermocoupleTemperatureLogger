#pragma once

#include "tx_api.h"
#include "nx_api.h"

class EthThread
{
public:
    static void entry(ULONG arg);

private:
    static constexpr UINT serverPort = 5025;
    static constexpr ULONG threadSleep = 10;
    static constexpr UINT packetPoolSize = 10 * (1536 + sizeof(NX_PACKET));
    static constexpr UINT packetPayloadSize = 1536;
    static constexpr UINT ipStackSize = 2048;
    static constexpr UINT ipThreadPriority = 1;
};
