#include "MMU.h"
#include <stdlib.h>

MMU::MMU(void)
{
	pMemory = (char *)malloc(0xFFFF);
}

char MMU::Read8(short address)
{
	return pMemory[address];
}

short MMU::Read16(short address)
{
	return (short) pMemory[address];
}

void MMU::Write8(char value, short address)
{
	pMemory[address] = value;
}

void MMU::Write16(short value, short address)
{
	pMemory[address] = value;
}