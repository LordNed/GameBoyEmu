#pragma once
#include "MMU.h"

enum Flags : unsigned char
{
	Zero = 0x80, //Last operation produced a result of zero
	Operation = 0x40, //Last operation was a subtraction
	HalfCarry = 0x20, //last operation the lower half of the byte overflowed past 15
	Carry = 0x10, //Last operation produced a result over 255 (for addition) or under 0 (for subtraction)
};

class Z80
{
public:
	Z80(MMU *pMemory);

	void Reset();
	void ExecuteInstruction(char instruction);

	int m_time;

	/* Registers */
	char regA, regB, regC, regD, regE, regH, regL; //8 bit registers
	char flags;

	short regPc, regSp; //16-bit registers (program counter, stack pointer)
	char regM;
	//Each instruction is executed in steps that are usually termed machine cycles
	//(M-cycles), each of which can take between three and six clock periods (T-cycles).
	//Each m-cycle corresponds roughly to one memory access and/or internal operation.
private:
	MMU *pMem;
};

