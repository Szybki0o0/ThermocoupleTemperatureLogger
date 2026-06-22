#pragma once

#include <cstdint>
#include "tx_api.h"

extern "C" {
#include "sdmmc.h"
}

class SdCard {
public:
    static bool init();
    static bool isPresent();
    static bool readSectors(uint32_t sector, uint8_t* buffer, uint32_t count);
    static bool writeSectors(uint32_t sector, const uint8_t* buffer, uint32_t count);
    static bool getCardInfo(HAL_SD_CardInfoTypeDef* info);
    static const char* getStateStr();

private:
    static TX_MUTEX mutex_;
    static bool initialized_;
    static HAL_SD_CardInfoTypeDef cardInfo_;
};
