#include "threads/EthThread.hpp"
#include "SystemState.hpp"

#include "app_netxduo.h"

static NX_PACKET_POOL ethPacketPool;
static NX_IP ethIp;
static NX_TCP_SOCKET ethSocket;

void EthThread::entry(ULONG arg)
{
    UINT status;
    uint8_t ipStack[ipStackSize];
    uint8_t arpCacheArea[sizeof(NX_ARP) * 16];
    uint8_t poolMemory[packetPoolSize];

    status = nx_packet_pool_create(&ethPacketPool, "ETH Pool",
                                   packetPayloadSize,
                                   poolMemory, sizeof(poolMemory));

    /// TODO: get MAC address from ETH HAL or use hardcoded
    /// nx_ip_create(&ethIp, ...);

    /// TODO: enable ARP, TCP, ICMP

    /// TODO: create TCP server socket, listen on port

    while (1)
    {
        /// TODO: accept client connection

        while (1)
        {
            /// TODO: receive SCPI command from client
            /// TODO: parse command (e.g. "*IDN?", "MEAS:TEMP?")
            /// TODO: build response, send back
            /// TODO: handle disconnect

            tx_thread_sleep(threadSleep);
        }

        /// TODO: disconnect, clean up, re-listen
    }
}
