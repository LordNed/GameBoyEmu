#include "Z80.h"
#include <Windows.h>

int main(int argc, char* argv[])
{
	MMU *pMemory = new MMU();
	Z80 *pZ80 = new Z80(pMemory);

	while(true)
	{
		pZ80->Update();
		Sleep(100);
	}
	return 0;
}

