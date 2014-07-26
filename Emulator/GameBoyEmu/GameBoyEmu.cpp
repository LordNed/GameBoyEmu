#include "Z80.h"

int main(int argc, char* argv[])
{
	MMU *pMemory = new MMU();
	Z80 *pZ80 = new Z80(pMemory);
	return 0;
}

