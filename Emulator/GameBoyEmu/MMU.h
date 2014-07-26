#pragma once
class MMU
{
public:
	MMU(void);

	char Read8(short address);
	short Read16(short address);
	void Write8(char value, short address);
	void Write16(short value, short address);

private:
	char *pMemory;
};

