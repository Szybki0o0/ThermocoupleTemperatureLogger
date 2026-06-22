#include "threads/EthThread.hpp"
#include "SystemState.hpp"
#include "ChannelManager.hpp"

#include "nx_stm32_eth_driver.h"
#include <cstdio>
#include <cstring>

NX_PACKET_POOL  EthThread::pool;
NX_IP           EthThread::ip;
NX_TCP_SOCKET   EthThread::server;
ScpiParser      EthThread::parser;

void EthThread::entry(ULONG arg) {
    UINT status;
    uint8_t ip_stack[ipStackSize];
    uint8_t arp_cache[sizeof(NX_ARP) * 32];
    uint8_t pool_mem[packetPoolSize];

    status = nx_packet_pool_create(&pool, (CHAR*)"ETH Pool",
                                   packetPayloadSize,
                                   pool_mem, sizeof(pool_mem));
    if (status != NX_SUCCESS) {
        printf("ETH: pool create failed 0x%x\r\n", status);
        return;
    }

    status = nx_ip_create(&ip, (CHAR*)"ETH IP",
                          IP_ADDRESS(192, 168, 0, 100),
                          IP_ADDRESS(255, 255, 255, 0),
                          &pool, nx_stm32_eth_driver,
                          ip_stack, sizeof(ip_stack), 1);
    if (status != NX_SUCCESS) {
        printf("ETH: ip create failed 0x%x\r\n", status);
        return;
    }

    nx_arp_enable(&ip, (void *)arp_cache, sizeof(arp_cache));
    nx_tcp_enable(&ip);
    nx_icmp_enable(&ip);

    ULONG link_status = 0;
    for (int i = 0; i < 50; i++) {
        nx_ip_status_check(&ip, NX_IP_LINK_ENABLED, &link_status, 100);
        if (link_status & NX_IP_LINK_ENABLED) break;
    }
    if (!(link_status & NX_IP_LINK_ENABLED)) {
        printf("ETH: link wait timeout\r\n");
        return;
    }
    printf("ETH: IP link up\r\n");

    status = nx_tcp_socket_create(&ip, &server, (CHAR*)"SCPI Server",
                                  NX_IP_NORMAL, NX_FRAGMENT_OKAY,
                                  NX_IP_TIME_TO_LIVE, 1024,
                                  NX_NULL, NX_NULL);
    if (status != NX_SUCCESS) {
        printf("ETH: socket create failed 0x%x\r\n", status);
        return;
    }

    status = nx_tcp_server_socket_listen(&ip, serverPort, &server, 5, NX_NULL);
    if (status != NX_SUCCESS) {
        printf("ETH: listen failed 0x%x\r\n", status);
        return;
    }

    printf("ETH: SCPI server listening on port %u\r\n", serverPort);

    char rx_buf[2048];
    int rx_len = 0;

    while (1) {
        status = nx_tcp_server_socket_accept(&server, 200);
        if (status != NX_SUCCESS) {
            tx_thread_sleep(threadSleep);
            continue;
        }

        printf("ETH: client connected\r\n");
        rx_len = 0;

        while (1) {
            NX_PACKET *pkt;
            status = nx_tcp_socket_receive(&server, &pkt, 200);

            if (status == NX_SUCCESS) {
                char *data = (char *)pkt->nx_packet_prepend_ptr;
                ULONG data_len = pkt->nx_packet_length;

                for (ULONG i = 0; i < data_len && rx_len < (int)sizeof(rx_buf) - 1; i++) {
                    char c = data[i];
                    if (c == '\n') {
                        rx_buf[rx_len] = 0;

                        if (rx_len > 0) {
                            char resp[SCPI_MAX_RESPONSE];
                            int rlen = parser.execute((const char*)rx_buf, resp, sizeof(resp));

                            NX_PACKET *tx_pkt;
                            if (nx_packet_allocate(&pool, &tx_pkt, NX_TCP_PACKET, 20) == NX_SUCCESS) {
                                if ((ULONG)rlen <= (ULONG)(tx_pkt->nx_packet_data_end - tx_pkt->nx_packet_prepend_ptr)) {
                                    memcpy(tx_pkt->nx_packet_prepend_ptr, resp, (size_t)rlen);
                                    tx_pkt->nx_packet_length = (ULONG)rlen;
                                    tx_pkt->nx_packet_append_ptr = tx_pkt->nx_packet_prepend_ptr + rlen;
                                    nx_tcp_socket_send(&server, tx_pkt, 200);
                                } else {
                                    nx_packet_release(tx_pkt);
                                }
                            }
                        }
                        rx_len = 0;
                    } else if (c != '\r') {
                        rx_buf[rx_len++] = c;
                    }
                }
                nx_packet_release(pkt);
            }

            if (status == NX_NOT_CONNECTED) {
                break;
            }

            tx_thread_sleep(threadSleep);
        }

        nx_tcp_socket_disconnect(&server, 200);
        nx_tcp_server_socket_unlisten(&ip, serverPort);
        nx_tcp_server_socket_relisten(&ip, serverPort, &server);
        printf("ETH: client disconnected\r\n");
    }
}
