#include "Z80.h"
#include <stdio.h>

Z80::Z80(MMU *pMemory)
{
	pMem = pMemory;
	Reset();
}

void Z80::Reset()
{
	m_time = 0;

	regA = regB = regC = regD = regE = regH = regL = flags =0;
	regPc = regSp = 0;
	regM =  0;
}


void Z80::ExecuteInstruction(char instruction)
{
	switch(instruction)
	{
		/* NOP */
	case 0x00:
		regM = 1;
		break;
		/* Load 16-bit immediate into BC */
	case 0x01:
		regC = pMem->Read8(regPc);
		regB = pMem->Read8(regPc+1);
		regPc +=2;
		regM = 3;
		break;
		/* Save A to address pointed by BC */
	case 0x02:
		pMem->Write8(regA, (regB << 8) + regC);
		regM = 2;
		break;
		/* Incriment 16-bit BC */
	case 0x03:
		regC = (regC+1) & 0xFF;
		if(!regC)
			regB = (regB+1) & 0xFF;
		regM = 1;
		break;
		/* Increment B */
	case 0x04:
		regB++;
		regB &= 0xFF;
		flags = regB ? 0 : Zero;
		regM = 1;
		break;
		/* Decrement B */
	case 0x05:
		regB--;
		regB &= 0xFF;
		flags = regB ? 0 : Zero;
		regM = 1;
		break;
		/* Load 8-bit immediate into B */
	case 0x06:
		regB = pMem->Read8(regPc);
		regPc++;
		regM = 2;
		break;
		/* Rotate A left with Carry */
	case 0x07:
		char ci = regA & Zero ? 1:0;
		char co = regA & Zero ? Carry : 0;
		regA = (regA << 1) + ci;
		regA &= 0xFF;
		flags = (flags&0xEF)+co;
		regM = 1;
		break;
		/* Save SP to given address */
	case 0x08:
		pMem->Write16(regSp, regPc);
		regM = 2;
		printf("Huh?\n");
		break;
		/* Add 16-bit BC to HL */
	case 0x09:
		int hl = (regH << 8) + regL;
		hl += (regB <<8)+regC;
		if(hl > 0xFFFF)
			flags |= Carry;
		else
			flags |= 0xEF;
		regH = (hl >> 8) & 0xFF;
		regL = hl&0xFF;
		regM = 3;
		break;
		/* Load A from address pointed to by BC */
	case 0xA:
		regA = pMem->Read8((regB << 8) + regC);
		regM = 2;
		break;
		/* Decrement 16-bit BC */
	case 0xB:
		regC = (regC-1)&0xFF;
		if(regC == 0xFF)
			regB = (regB-1)&0xFF;
		regM = 1;
		break;
		/* Increment C */
	case 0xC:
		regC++;
		regC &= 0xFF;
		flags = regC ? 0 : Zero;
		regM = 1;
		break;
		/* Decrement C */
	case 0xD:
		regC--;
		regC &= 0xFF;
		flags = regC ? 0 : Zero;
		regM = 1;
		break;
		/* Load 8 bit immediate into C */
	case 0xE:
		regC = pMem->Read8(regPc);
		regPc++;
		regM = 2;
		break;
		/* Rotate A right with Carry */
	case 0xF:
		int ci = regA & 1 ? Zero : 0;
		int co = regA & 1 ? Carry : 0;
		regA = (regA >> 1) + ci; 
		regA &= 255;
		flags = (flags & 0xEF)+co;
		regM = 1;
		break;
		/* Stop Processor */
	case 0x10:
		printf("Stop Processor");
		break;
		/* Load 16-bit immediate into DE */
	case 0x11:
		regE = pMem->Read8(regPc);
		regD = pMem->Read8(regPc+1);
		regPc +=2;
		regM = 3;
		break;
		/* Save A to address pointed to by DE */
	case 0x12:
		pMem->Write8(regA, (regD << 8) + regE);
		regM = 2;
		break;
		/* Increment 16-bit DE */
	case 0x13:
		regE = (regE+1) & 0xFF;
		if(!regE)
			regD = (regD+1) & 0xFF;
		regM = 1;
		break;
		/* Increment D */
	case 0x14:
		regD++;
		regD &= 0xFF;
		flags = regD ? 0 : Zero;
		regM = 1;
		break;
		/* Decrement D */
	case 0x15:
		regD--;
		regD &= 0xFF;
		flags = regD ? 0 : Zero;
		regM = 1;
		break;
		/* Load 8-bit immediate into D */
	case 0x16:
		regD = pMem->Read8(regPc);
		regPc++;
		regM = 2;
		break;
		/* Rotate A left */
	case 0x17:
		int ci = flags & Carry ? 1 : 0;
		int co = regA & Zero ? Carry : 0;
		regA = (regA << 1) + ci; 
		regA &= 0xFF;
		flags = (flags & 0xEF)+co;
		regM = 1;
		break;
		/* Relative jump by signed immediate */
	case 0x18:
		char i = pMem->Read8(regPc);
		if(i > 127)
			i =-((~i+1)&0xFF);
		regPc++;
		regM = 2;
		regPc += i;
		regM++;
		break;
		/* Add 16-bit DE to HL */
	case 0x19:
		int hl = (regH << 8) + regL;
		hl += (regD <<8)+regE;
		if(hl > 0xFFFF)
			flags |= Carry;
		else
			flags |= 0xEF;
		regH = (hl >> 8) & 0xFF;
		regL = hl&0xFF;
		regM = 3;
		break;
		/* Load A from address pointed to by DE */
	case 0x1A:
		regA = pMem->Read8((regD << 8) + regE);
		regM = 2;
		break;
		/* Decrement 16-bit DE */
	case 0x1B:
		regE = (regE-1)&0xFF;
		if(regE == 0xFF)
			regD = (regD-1)&0xFF;
		regM = 1;
		break;
		/* Increment E */
	case 0x1C:
		regE++;
		regE &= 0xFF;
		flags = regC ? 0 : Zero;
		regM = 1;
		break;
		/* Decrement E */
	case 0x1D:
		regE--;
		regE &= 0xFF;
		flags = regE ? 0 : Zero;
		regM = 1;
		break;
		/* Load 8-bit immediate into E */
	case 0x1E:
		regE = pMem->Read8(regPc);
		regPc++;
		regM = 2;
		break;
		/* Rotate A right */
	case 0x1F:
		int ci = flags & Carry ? Zero : 0;
		int co = regA & 1 ? Carry : 0;
		regA = (regA >> 1) + ci;
		regA &= 0xFF;
		flags = (flags & 0xEF) + co;
		regM = 1;
		break;
	}

}
