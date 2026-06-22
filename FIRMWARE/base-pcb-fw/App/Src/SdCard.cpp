#include "SdCard.hpp"
#include <cstdio>

extern "C" {
#include "main.h"
}

TX_MUTEX SdCard::mutex_;
bool SdCard::initialized_ = false;
HAL_SD_CardInfoTypeDef SdCard::cardInfo_;

bool SdCard::init()
{
    if (initialized_) return true;

    UINT ret = tx_mutex_create(&mutex_, (CHAR*)"SD Mutex", TX_NO_INHERIT);
    if (ret != TX_SUCCESS) {
        printf("SD: mutex create failed (0x%02x)\r\n", ret);
        return false;
    }

    if (!isPresent()) {
        printf("SD: card not present\r\n");
        return false;
    }

    hsd1.Instance = SDMMC1;
    hsd1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
    hsd1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
    hsd1.Init.BusWide = SDMMC_BUS_WIDE_4B;
    hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
    hsd1.Init.ClockDiv = 5;

    HAL_SD_MspInit(&hsd1);

    if (HAL_SD_Init(&hsd1) != HAL_OK) {
        printf("SD: HAL_SD_Init failed\r\n");
        return false;
    }

    if (HAL_SD_GetCardState(&hsd1) != HAL_SD_CARD_TRANSFER) {
        printf("SD: card not in transfer state\r\n");
        return false;
    }

    if (HAL_SD_GetCardInfo(&hsd1, &cardInfo_) != HAL_OK) {
        printf("SD: get card info failed\r\n");
        return false;
    }

    if (HAL_SD_ConfigWideBusOperation(&hsd1, SDMMC_BUS_WIDE_4B) != HAL_OK) {
        printf("SD: wide bus config failed\r\n");
        return false;
    }

    printf("SD: init OK, %lu MB, BlockSize=%lu\r\n",
           (cardInfo_.CardCapacity / (1024 * 1024)),
           cardInfo_.BlockSize);

    initialized_ = true;
    return true;
}

bool SdCard::isPresent()
{
    return HAL_GPIO_ReadPin(SD_Detect_GPIO_Port, SD_Detect_Pin) == GPIO_PIN_RESET;
}

bool SdCard::readSectors(uint32_t sector, uint8_t* buffer, uint32_t count)
{
    if (!initialized_) {
        printf("SD: not initialized\r\n");
        return false;
    }

    tx_mutex_get(&mutex_, TX_WAIT_FOREVER);
    HAL_StatusTypeDef halRet = HAL_SD_ReadBlocks(&hsd1, buffer, sector, count, 5000);
    tx_mutex_put(&mutex_);

    if (halRet != HAL_OK) {
        printf("SD: read error at sector %lu (ret=%d)\r\n", sector, halRet);
        return false;
    }
    return true;
}

bool SdCard::writeSectors(uint32_t sector, const uint8_t* buffer, uint32_t count)
{
    if (!initialized_) {
        printf("SD: not initialized\r\n");
        return false;
    }

    tx_mutex_get(&mutex_, TX_WAIT_FOREVER);
    HAL_StatusTypeDef halRet = HAL_SD_WriteBlocks(&hsd1, (uint8_t*)buffer, sector, count, 5000);
    tx_mutex_put(&mutex_);

    if (halRet != HAL_OK) {
        printf("SD: write error at sector %lu (ret=%d)\r\n", sector, halRet);
        return false;
    }
    return true;
}

bool SdCard::getCardInfo(HAL_SD_CardInfoTypeDef* info)
{
    if (!initialized_) return false;
    *info = cardInfo_;
    return true;
}

const char* SdCard::getStateStr()
{
    if (!initialized_) return "Not initialized";
    if (!isPresent()) return "Card removed";

    HAL_SD_StateTypeDef state = HAL_SD_GetState(&hsd1);
    switch (state) {
        case HAL_SD_STATE_RESET:     return "Reset";
        case HAL_SD_STATE_READY:     return "Ready";
        case HAL_SD_STATE_TIMEOUT:   return "Timeout";
        case HAL_SD_STATE_BUSY:      return "Busy";
        case HAL_SD_STATE_PROGRAMMING: return "Programming";
        case HAL_SD_STATE_ERROR:     return "Error";
        default:                     return "Unknown";
    }
}
