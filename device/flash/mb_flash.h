
#ifndef MB_FLASH_H
#define MB_FLASH_H

#include "stm32f4xx_flash.h"

#define FLASH_ADDR_START (0x0800C000)
// #define FLASH_ADDR_END (0x0803ffff)
#define FLASH_ADDR_HEAD (0x0800ffff)
#define FLASH_ADDR_SECTOR1 (0x0801ffff)
#define FLASH_ADDR_SECTOR2 (0x0803ffff)

#define FLASH_64K (0x10000)
#define FLASH_128K (0x20000)

static int16_t _get_falsh_sector(uint32_t addr)
{
	if (addr > FLASH_ADDR_SECTOR2 || addr < FLASH_ADDR_START)
		return -1;

	int sector = -1;

	if (addr >= FLASH_ADDR_START && addr <= FLASH_ADDR_HEAD)
		sector = FLASH_Sector_3;
	else if (addr > FLASH_ADDR_HEAD && addr <= FLASH_ADDR_SECTOR1)
		sector = FLASH_Sector_4;
	else if (addr > FLASH_ADDR_SECTOR1 && addr <= FLASH_ADDR_SECTOR2)
		sector = FLASH_Sector_5;
	return sector;
}

static uint32_t _get_flash_sector_size(uint32_t addr)
{
	uint32_t size = 0;
	switch (_get_falsh_sector(addr)) {
	case FLASH_Sector_4:
		size = FLASH_64K;
		break;
	case FLASH_Sector_5:
		size = FLASH_128K;
		break;
	}

	return size;
}

static int mb_flash_write(uint32_t addr, uint8_t *buffer, uint32_t len)
{
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGAERR | FLASH_FLAG_WRPERR | FLASH_FLAG_OPERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
	uint8_t *p = buffer;
	uint32_t state;
	uint32_t cur_addr = addr;
	while (len--) {
		state  = FLASH_ProgramByte(cur_addr, *(p++));
		if (state != FLASH_COMPLETE)
			break;
		else
			cur_addr++;
	}
	FLASH_Lock();
	return state;
}

static int mb_flash_write_word(uint32_t addr, uint32_t word)
{
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGAERR | FLASH_FLAG_WRPERR | FLASH_FLAG_OPERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
	FLASH_Status status = FLASH_ProgramWord(addr, word);
	FLASH_Lock();
	return status == FLASH_COMPLETE;
}

static int mb_flash_read(uint32_t addr, uint8_t *buffer, uint32_t len)
{
	FLASH_Unlock();
	uint8_t *p = buffer;
	uint32_t cur_addr = addr;
	while (len--) {
		*(p++) = *((uint8_t *)cur_addr++);
	}
	FLASH_Lock();
	return 1;
}

static int mb_flash_erase(uint32_t addr, uint32_t len)
{
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGAERR | FLASH_FLAG_WRPERR | FLASH_FLAG_OPERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
	FLASH_DataCacheCmd(DISABLE);

	int16_t current_sector = _get_falsh_sector(addr);
	FLASH_Status status = FLASH_BUSY;
	if (current_sector != -1) {
		status = FLASH_EraseSector(current_sector, VoltageRange_3);
	}
	FLASH_DataCacheCmd(ENABLE);
	FLASH_Lock();
	return status;
}

#endif
