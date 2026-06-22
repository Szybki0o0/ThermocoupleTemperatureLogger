/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_netxduo.c
  * @author  MCD Application Team
  * @brief   NetXDuo applicative file
  ******************************************************************************
    * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_netxduo.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
#include "nx_stm32_phy_driver.h"
#include "ChannelManager.hpp"
#include "ScpiParser.hpp"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
NX_PACKET_POOL NxAppPool;
NX_IP          NetXDuoEthIpInstance;
NX_TCP_SOCKET  TcpServer;
TX_THREAD      NxAppThread;
static ScpiParser scpi_parser;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static VOID nx_app_thread_entry (ULONG thread_input);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/**
  * @brief  Application NetXDuo Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT MX_NetXDuo_Init(VOID *memory_ptr)
{
  UINT ret = NX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;
   CHAR *pointer;

   /* USER CODE BEGIN App_NetXDuo_MEM_POOL */
  (void)byte_pool;
  /* USER CODE END App_NetXDuo_MEM_POOL */
  /* USER CODE BEGIN 0 */

  /* USER CODE END 0 */

  /* Initialize the NetXDuo system. */
  nx_system_initialize();

    /* Allocate the memory for packet_pool.  */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, NX_APP_PACKET_POOL_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Create the Packet pool to be used for packet allocation,
   * If extra NX_PACKET are to be used the NX_APP_PACKET_POOL_SIZE should be increased
   */
  ret = nx_packet_pool_create(&NxAppPool, "NetXDuo App Pool", DEFAULT_PAYLOAD_SIZE, pointer, NX_APP_PACKET_POOL_SIZE);

  if (ret != NX_SUCCESS)
  {
    return NX_POOL_ERROR;
  }

    /* Allocate the memory for Ip_Instance */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, Nx_IP_INSTANCE_THREAD_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

   /* Create the main NX_IP instance */
  ret = nx_ip_create(&NetXDuoEthIpInstance, "NetX Ip instance", NX_APP_DEFAULT_IP_ADDRESS, NX_APP_DEFAULT_NET_MASK, &NxAppPool, nx_stm32_eth_driver,
                     pointer, Nx_IP_INSTANCE_THREAD_SIZE, NX_APP_INSTANCE_PRIORITY);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }

    /* Allocate the memory for ARP */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, DEFAULT_ARP_CACHE_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Enable the ARP protocol and provide the ARP cache size for the IP instance */

  /* USER CODE BEGIN ARP_Protocol_Initialization */

  /* USER CODE END ARP_Protocol_Initialization */

  ret = nx_arp_enable(&NetXDuoEthIpInstance, (VOID *)pointer, DEFAULT_ARP_CACHE_SIZE);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }

  /* Enable the ICMP */

  /* USER CODE BEGIN ICMP_Protocol_Initialization */

  /* USER CODE END ICMP_Protocol_Initialization */

  ret = nx_icmp_enable(&NetXDuoEthIpInstance);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }

  /* Enable TCP Protocol */

  /* USER CODE BEGIN TCP_Protocol_Initialization */

  /* USER CODE END TCP_Protocol_Initialization */

  ret = nx_tcp_enable(&NetXDuoEthIpInstance);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }

   /* Allocate the memory for main thread   */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, NX_APP_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Create the main thread */
  ret = tx_thread_create(&NxAppThread, "NetXDuo App thread", nx_app_thread_entry , 0, pointer, NX_APP_THREAD_STACK_SIZE,
                         NX_APP_THREAD_PRIORITY, NX_APP_THREAD_PRIORITY, TX_NO_TIME_SLICE, TX_AUTO_START);

  if (ret != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }

  /* USER CODE BEGIN MX_NetXDuo_Init */
  /* USER CODE END MX_NetXDuo_Init */

  return ret;
}

/**
* @brief  Main thread entry.
* @param thread_input: ULONG user argument used by the thread entry
* @retval none
*/
static VOID nx_app_thread_entry (ULONG thread_input)
{
  /* USER CODE BEGIN Nx_App_Thread_Entry 0 */
  UINT ret;
  ULONG actual_status;
  int i;
  int32_t link_state;

  /* Initialize channel manager */
  chanMgr.reset();
  printf("ETH: SCPI initialized, %d channels\r\n", TC_NUM_CHANNELS);

  /* --- DIAG: rapid 6 blinks to confirm thread runs --- */
  for (i = 0; i < 6; i++)
  {
    HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
    tx_thread_sleep(5);
  }

  printf("ETH: waiting IP init+link...\r\n");
  if (nx_ip_status_check(&NetXDuoEthIpInstance, NX_IP_INITIALIZE_DONE | NX_IP_LINK_ENABLED, &actual_status, NX_APP_DEFAULT_TIMEOUT) != NX_SUCCESS)
  {
    printf("ETH: timeout! status=%ld\r\n", actual_status);
    while(1) { HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin); tx_thread_sleep(50); }
  }
  printf("ETH: IP init+link done (status=%ld)\r\n", actual_status);

  /* Enable promiscuous mode to bypass MAC filter */
  SET_BIT(ETH->MACPFR, ETH_MACPFR_PR);
  printf("ETH: promiscuous mode enabled\r\n");

  /* Check DMA status for errors */
  {
      uint32_t dma_sr = ETH->DMACSR;
      printf("ETH: DMACSR=0x%08lX (NIS=%lu, RIS=%lu, TIS=%lu, AIS=%lu, FBE=%lu)\r\n",
          dma_sr,
          (dma_sr >> 0) & 1,
          (dma_sr >> 6) & 1,
          (dma_sr >> 2) & 1,
          (dma_sr >> 1) & 1,
          (dma_sr >> 4) & 1);
  }

  /* --- Check PHY link state --- */
  link_state = nx_eth_phy_get_link_state();
  printf("ETH: link state in app thread = %ld\r\n", link_state);
  if (link_state <= ETH_PHY_STATUS_LINK_DOWN)
  {
    /* Link down - fast continuous blink */
    for (i = 0; i < 10; i++)
    {
      HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);
      tx_thread_sleep(10);
      HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);
      tx_thread_sleep(10);
    }
    /* Then solid ON so user knows link-down was detected */
    HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);
  }

  /* --- DIAG: long OFF → OFF = link-up, ready --- */
  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);

  /* Set our IP address */
   nx_ip_address_set(&NetXDuoEthIpInstance, IP_ADDRESS(192,168,0,100), IP_ADDRESS(255,255,255,0));

  /* --- Test 1: loopback ping (to self) --- */
  {
      NX_PACKET *resp;
      printf("ETH: loopback ping 192.168.0.22...\r\n");
      ret = nx_icmp_ping(&NetXDuoEthIpInstance, IP_ADDRESS(192,168,0,22),
                         "ping", 4, &resp, 200);
      printf("ETH: loopback ping=%u (%s)\r\n", ret, ret==NX_SUCCESS?"OK":"FAIL");
      if (ret == NX_SUCCESS) nx_packet_release(resp);
  }

  /* --- Test 2: external ping (through Ethernet driver) --- */
  {
      NX_PACKET *resp;
      printf("ETH: waiting for external ping (30s)...\r\n");
      /* Sleep 5s to let PC ping us */
      for (i = 0; i < 250; i++) { tx_thread_sleep(20); } /* ~5s */
      printf("ETH: now pinging 192.168.0.1...\r\n");
      ret = nx_icmp_ping(&NetXDuoEthIpInstance, IP_ADDRESS(192,168,0,1),
                         "ping", 4, &resp, 200);
      printf("ETH: external ping=%u (%s)\r\n", ret, ret==NX_SUCCESS?"OK":"FAIL");
      printf("ETH: external ping=%u (%s)\r\n", ret, ret==NX_SUCCESS?"OK":"FAIL");
      if (ret == NX_SUCCESS) nx_packet_release(resp);

      ETH_DMADescTypeDef *txd0 = (ETH_DMADescTypeDef *)ETH->DMACTDLAR;
      ETH_DMADescTypeDef *rxd0 = (ETH_DMADescTypeDef *)ETH->DMACRDLAR;
      printf("ETH: TX_DESC0=0x%08lX (OWN=%lu) RX_DESC0=0x%08lX (OWN=%lu)\r\n",
          txd0[0].DESC3, (txd0[0].DESC3>>31)&1,
          rxd0[0].DESC3, (rxd0[0].DESC3>>31)&1);
      printf("ETH: DMACSR=0x%08lX DMACIER=0x%08lX\r\n",
          ETH->DMACSR, ETH->DMACIER);
      printf("ETH: NVIC_ISER[3]=0x%08lX\r\n", NVIC->ISER[3]);
  }

  /* --- IP initialized and link up --- */
  ret = nx_tcp_socket_create(&NetXDuoEthIpInstance, &TcpServer, "TCP Server",
                             NX_IP_NORMAL, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, 1024, NX_NULL, NX_FALSE);
  if (ret != NX_SUCCESS)
  {
    while (1)
    {
      HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
      tx_thread_sleep(50);
    }
  }

  ret = nx_tcp_server_socket_listen(&NetXDuoEthIpInstance, 5025, &TcpServer, 5, NX_NULL);
  if (ret != NX_SUCCESS)
  {
    while (1)
    {
      HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
      tx_thread_sleep(50);
    }
  }

  while (1)
  {
    ret = nx_tcp_server_socket_accept(&TcpServer, NX_APP_DEFAULT_TIMEOUT);
    if (ret != NX_SUCCESS)
    {
      continue;
    }

    static char scpi_rx_buf[256];
    int scpi_rx_len = 0;

    while (1)
    {
      NX_PACKET *packet;
      UCHAR data[64];
      ULONG bytes_read;

      ret = nx_tcp_socket_receive(&TcpServer, &packet, 200);
      if (ret != NX_SUCCESS)
      {
        break;
      }

      nx_packet_data_retrieve(packet, data, &bytes_read);

      for (ULONG i = 0; i < bytes_read && scpi_rx_len < (int)sizeof(scpi_rx_buf) - 1; i++)
      {
        char c = (char)data[i];
        if (c == '\n')
        {
          scpi_rx_buf[scpi_rx_len] = 0;

          if (scpi_rx_len > 0)
          {
            char resp[SCPI_MAX_RESPONSE];
            int rlen = scpi_parser.execute(scpi_rx_buf, resp, sizeof(resp));

            NX_PACKET *tx_pkt;
            if (nx_packet_allocate(NetXDuoEthIpInstance.nx_ip_default_packet_pool,
                                   &tx_pkt, NX_TCP_PACKET, 20) == NX_SUCCESS)
            {
              if ((ULONG)rlen <= (ULONG)(tx_pkt->nx_packet_data_end - tx_pkt->nx_packet_prepend_ptr))
              {
                memcpy(tx_pkt->nx_packet_prepend_ptr, resp, (size_t)rlen);
                tx_pkt->nx_packet_length = (ULONG)rlen;
                tx_pkt->nx_packet_append_ptr = tx_pkt->nx_packet_prepend_ptr + rlen;
                nx_tcp_socket_send(&TcpServer, tx_pkt, 200);
              }
              else
              {
                nx_packet_release(tx_pkt);
              }
            }
          }
          scpi_rx_len = 0;
        }
        else if (c != '\r')
        {
          scpi_rx_buf[scpi_rx_len++] = c;
        }
      }

      nx_packet_release(packet);
    }

    nx_tcp_server_socket_unaccept(&TcpServer);
  }
  /* USER CODE END Nx_App_Thread_Entry 0 */

}
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
