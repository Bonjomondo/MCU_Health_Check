#ifndef __W25Q64_H
#define __W25Q64_H

#include "stm32f10x.h"

// W25Q64指令
#define W25Q64_WRITE_ENABLE             0x06
#define W25Q64_WRITE_DISABLE            0x04
#define W25Q64_READ_STATUS_REGISTER_1   0x05
#define W25Q64_READ_STATUS_REGISTER_2   0x35
#define W25Q64_WRITE_STATUS_REGISTER    0x01
#define W25Q64_PAGE_PROGRAM             0x02
#define W25Q64_QUAD_PAGE_PROGRAM        0x32
#define W25Q64_BLOCK_ERASE_64KB         0xD8
#define W25Q64_BLOCK_ERASE_32KB         0x52
#define W25Q64_SECTOR_ERASE_4KB         0x20
#define W25Q64_CHIP_ERASE               0xC7
#define W25Q64_ERASE_SUSPEND            0x75
#define W25Q64_ERASE_RESUME             0x7A
#define W25Q64_POWER_DOWN               0xB9
#define W25Q64_HIGH_PERFORMANCE_MODE    0xA3
#define W25Q64_MANUFACTURER_DEVICE_ID   0x90
#define W25Q64_JEDEC_ID                 0x9F
#define W25Q64_READ_DATA                0x03
#define W25Q64_FAST_READ                0x0B

void W25Q64_Init(void);
void W25Q64_ReadID(uint8_t *MID, uint16_t *DID);
void W25Q64_PageWrite(uint32_t Address, uint8_t *DataArray, uint16_t Count);
void W25Q64_SectorErase(uint32_t Address);
void W25Q64_ReadData(uint32_t Address, uint8_t *DataArray, uint32_t Count);

#endif
