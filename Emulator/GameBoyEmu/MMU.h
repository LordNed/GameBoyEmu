#pragma once
#include <cstdint>

class MMU
{
public:
	MMU(void);

	uint8_t Read8(uint16_t address);
	uint16_t Read16(uint16_t address);
	void Write8(uint8_t value, uint16_t address);
	void Write16(uint16_t value, uint16_t address);

private:
	uint8_t *pMemory;
};

