#pragma once
#include "MMU.h"
#include <cstdint>

enum Flags : uint8_t
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
	void Update();

	int m_time;

	/* Registers */
	uint8_t regA, regB, regC, regD, regE, regH, regL; //8 bit registers
	uint8_t flags;

	short regPc, regSp; //16-bit registers (program counter, stack pointer)
	uint8_t regM;
	//Each instruction is executed in steps that are usually termed machine cycles
	//(M-cycles), each of which can take between three and six clock periods (T-cycles).
	//Each m-cycle corresponds roughly to one memory access and/or internal operation.

	/* Shadow Registers? */
	uint8_t rsvA, rsvB, rsvC, rsvD, rsvE, rsvH, rsvL, rsvF;

private:
	MMU *pMem;

	void ExecuteInstruction(uint8_t instruction);

	void Rsv();
	void Rrs();

	void AddRegisters(uint8_t add, uint8_t &to);
	void AddRegistersAndCarry(uint8_t add, uint8_t &to);

	void SubtractRegisters(uint8_t sub, uint8_t &from);
	void SubtractRegistersAndCarry(uint8_t sub, uint8_t &from);

	void AndRegisters(uint8_t from, uint8_t &against);
	void XorRegisters(uint8_t from, uint8_t &against);
	void OrRegisters(uint8_t from, uint8_t &against);
	void CompareRegisters(uint8_t reg, uint8_t against);

	void CallRoutine(uint16_t address);
	void CallRoutineIfZero(uint16_t address);
	void NopWarn(uint8_t op);

	void PopStack(uint8_t &intoA, uint8_t &intoB);
	void PushStack(uint8_t pushA, uint8_t pushB);
};

