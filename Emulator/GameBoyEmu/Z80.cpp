#include "Z80.h"
#include <stdio.h>
#include <cstdint>

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
		{
			char ci = regA & Zero ? 1:0;
			char co = regA & Zero ? Carry : 0;
			regA = (regA << 1) + ci;
			regA &= 0xFF;
			flags = (flags&0xEF)+co;
			regM = 1;
			break;
		}
		/* Save SP to given address */
	case 0x08:
		pMem->Write16(regSp, regPc);
		regM = 2;
		printf("Huh?\n");
		break;
		/* Add 16-bit BC to HL */
	case 0x09:
		{
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
		}
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
		{
		int ci = regA & 1 ? Zero : 0;
		int co = regA & 1 ? Carry : 0;
		regA = (regA >> 1) + ci; 
		regA &= 255;
		flags = (flags & 0xEF)+co;
		regM = 1;
		break;
		}
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
		{
		int ci = flags & Carry ? 1 : 0;
		int co = regA & Zero ? Carry : 0;
		regA = (regA << 1) + ci; 
		regA &= 0xFF;
		flags = (flags & 0xEF)+co;
		regM = 1;
		break;
		}
		/* Relative jump by signed immediate */
	case 0x18:
		{
		char i = pMem->Read8(regPc);
		if(i > 127)
			i =-((~i+1)&0xFF);
		regPc++;
		regM = 2;
		regPc += i;
		regM++;
		break;
		}
		/* Add 16-bit DE to HL */
	case 0x19:
		{
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
		}
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
		flags = regE ? 0 : Zero;
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
		{
		int ci = flags & Carry ? Zero : 0;
		int co = regA & 1 ? Carry : 0;
		regA = (regA >> 1) + ci;
		regA &= 0xFF;
		flags = (flags & 0xEF) + co;
		regM = 1;
		break;
		}
		/* Relative jump by signed immediate if last result was not zero */
	case 0x20:
		{
		char i = pMem->Read8(regPc);
		if(i > 127)
			i= -((~i+1)&0xFF);
		regPc++;
		regM = 2;

		if((flags & Zero) == 0x00)
		{
			regPc+=i;
			regM++;
		}
		break;
		}
		/* Load 16-bit immediate into HL */
	case 0x21:
		regL = pMem->Read8(regPc);
		regH = pMem->Read8(regPc+1);
		regPc +=2;
		regM = 3;
		break;
		/* Save A to address pointed by HL, and increment HL */
	case 0x22:
		pMem->Write8(regA, (regH << 8) + regL);
		regL = (regL+1)&0xFF;
		if(!regL)
			regH = (regH+1)&0xFF;
		regM = 2;
		break;
		/* Increment 16-bit HL */
	case 0x23:
		regL = (regL+1) & 0xFF;
		if(!regL)
			regH = (regH+1) & 0xFF;
		regM = 1;
		break;
		/* Increment H */
	case 0x24:
		regH++;
		regH &= 0xFF;
		flags = regH ? 0 : Zero;
		regM = 1;
		break;
		/* Decrement H */
	case 0x25:
		regH--;
		regH &= 0xFF;
		flags = regH ? 0 : Zero;
		regM = 1;
		break;
		/* Load 8-bit immediate into H */
	case 0x26:
		regH = pMem->Read8(regPc);
		regPc++;
		regM = 2;
		break;
		/* Adjust A for BCD addition */
	case 0x27:
		{
		char a = regA;
		if((flags & HalfCarry) || ((regA & 15) > 9))
			regA += 6;
		flags &= 0xEF;
		if(flags & HalfCarry || a > 0x99)
		{
			regA += 0x60;
			flags |= Carry;
		}
		regM = 1;
		break;
		}
		/* Relative jump by signed immediate if last result was zero */
	case 0x28:
		{
		char i = pMem->Read8(regPc);
		if(i>127)
			i=-((~i+1)&0xFF);
		regPc++;
		regM = 2;
		if((flags & Zero) == Zero)
		{
			regPc += i;
			regM++;
		}
		break;
		}
		/* Add 16-bit HL to HL */
	case 0x29:
		{
		int hl = (regH << 8) + regL;
		hl+= (regH << 8) + regL;
		if(hl > 0xFFFF)
			flags |= Carry;
		else
			flags &= 0xEF;
		regH = (hl >> 7)&0xFF;
		regL = hl&0xFF;
		regM = 3;
		break;
		}
		/* Load A from address pointed to by HL and increment HL */
	case 0x2A:
		regA = pMem->Read8((regH << 8) + regL);
		regL = (regL+1)&0xFF;
		if(!regL)
			regH = (regH+1)&0xFF;
		regM = 2;
		break;
		/* Decrement 16-bit HL */
	case 0x2B:
		regL = (regL-1)&0xFF;
		if(regL == 0xFF)
			regH = (regH-1)&0xFF;
		regM = 1;
		break;
		/* Increment L */
	case 0x2C:
		regL++;
		regL &= 0xFF;
		flags = regL ? 0 : Zero;
		regM = 1;
		break;
		/* Decrement L */
	case 0x2D:
		regL--;
		regL &= 0xFF;
		flags = regL ? 0 : Zero;
		regM = 1;
		break;
		/* Load 8-bit immediate into L */
	case 0x2E:
		regL = pMem->Read8(regPc);
		regPc++;
		regM = 2;
		break;
		/* Complement (logical NOT) on A */
	case 0x2F:
		regA ^= 0xFF;
		flags = regA ? 0 : Zero;
		regM = 1;
		break;
		/* Relative jump by signed immediate if last result caused no carry */
	case 0x30:
		{
			char i = pMem->Read8(regPc);
			if(i>127)
				i=-((~i+1)&0xFF);
			regPc++;
			regM = 2;
			if((flags & Carry) == 0)
			{
				regPc += i;
				regM++;
			}
			break;
		}
		/* Load 16-bit immediate into SP */
	case 0x31:
		regSp = pMem->Read16(regPc);
		regPc+=2;
		regM = 3;
		break;
		/* Save A to address pointed by HL and decrement HL */
	case 0x32:
		pMem->Write8(regA, (regH << 8) + regL);
		regL = (regL-1)&0xFF;
		if(!regL)
			regH = (regH-1)&0xFF;
		regM = 2;
		break;
		/* Increment 16-bit SP */
	case 0x33:
		regSp++;
		regSp &= 0xFFFF;
		regM = 1;
		break;
		/* Increment value pointed by HL */
	case 0x34:
		{
			uint8_t i = pMem->Read8((regH << 8) + regL);
			i++;
			i&=0xFF;
			pMem->Write8(i, (regH << 8) + regL);
			flags = i ? 0 : Zero;
			regM = 3;
			break;
		}
		/* Decrement value pointed by HL */
	case 0x35:
		{
			uint8_t i = pMem->Read8((regH << 8) + regL);
			i--;
			i&=0xFF;
			pMem->Write8(i, (regH << 8) + regL);
			flags = i ? 0 : Zero;
			regM = 3;
			break;
		}
		/* Load 8-bit immediate into address pointed by HL */
	case 0x36:
		{
			uint8_t i = pMem->Read8(regPc);
			pMem->Write8(i, (regH << 8) + regL);
			regPc++;
			regM = 3;
			break;
		}
		/* Set carry flag */
	case 0x37:
		flags |= Carry;
		regM = 1;
		break;
		/* Relative jump by signed immediate if last result caused carry */
	case 0x38:
		{
			uint8_t i = pMem->Read8(regPc);
			if(i>127)
				i=-((~i+1)&0xFF);
			regPc++;
			regM = 2;
			if((flags & Carry) == Carry)
			{
				regPc += i;
				regM++;
			}
			break;
		}
		/* Add 16-bit SP to HL */
	case 0x39:
		uint16_t hl = (regH << 8) + regL;
		hl += regSp;
		if(hl > 0x65535)
			flags |= Carry;
		else
			flags &= 0xEF;
		regH = (hl >> 8)&0xFF;
		regL = hl & 0xFF;
		regM = 3;
		break;
		/* Load A from address pointed to by HL, and decrement HL */
	case 0x3A:
		regA = pMem->Read8((regH << 8) + regL);
		regL = (regL-1)&0xFF;
		if(!regL)
			regH = (regH-1)&0xFF;
		regM = 2;
		break;
		/* Decrement 16-bit SP */
	case 0x3B:
		regSp--;
		regSp &= 0xFFFF;
		regM = 1;
		break;
		/* Increment A */
	case 0x3C:
		regA++;
		regA &= 0xFF;
		flags = regA ? 0 : Zero;
		regM = 1;
		break;
		/* Decrement A */
	case 0x3D:
		regA--;
		regA &= 0xFF;
		flags = regA ? 0 : Zero;
		regM = 1;
		break;
		/* Load 8-bit immediate into A */
	case 0x3E:
		regA = pMem->Read8(regPc);
		regPc++;
		regM = 2;
		break;
		/* Clear Carry Flag */
	case 0x3F:
		flags &= ~Carry;
		regM = 1;
		break;
		/* Copy B to B */
	case 0x40:
		regB = regB;
		regM = 1;
		break;
		/* Copy C to B */
	case 0x41:
		regB = regC;
		regM = 1;
		break;
		/* Copy D to B */
	case 0x42:
		regB = regD;
		regM = 1;
		break;
		/* Copy E to B */
	case 0x43:
		regB = regE;
		regM = 1;
		break;
		/* Copy H to B */
	case 0x44:
		regB = regH;
		regM = 1;
		break;
		/* Copy L to B */
	case 0x45:
		regB = regL;
		regM = 1;
		break;
		/* Copy value pointed by HL to B */
	case 0x46:
		regB = pMem->Read8((regH << 8) + regL);
		regM = 2;
		break;
		/* Copy A to B */
	case 0x47:
		regB = regA;
		regM = 1;
		break;
		/* Copy B to C */
	case 0x48:
		regC = regB;
		regM = 1;
		break;
		/* Copy C to C */
	case 0x49:
		regC = regC;
		regM = 1;
		break;
		/* Copy D to C */
	case 0x4A:
		regC = regD;
		regM = 1;
		break;
		/* Copy E to C */
	case 0x4B:
		regC = regE;
		regM = 1;
		break;
		/* Copy H to C */
	case 0x4C:
		regC = regH;
		regM = 1;
		break;
		/* Copy L to C */
	case 0x4D:
		regC = regL;
		regM = 1;
		break;
		/* Copy value pointed by HL to C */
	case 0x4E:
		regC = pMem->Read8((regH << 8) + regL);
		regM = 2;
		break;
		/* Copy A to C */
	case 0x4F:
		regC = regA;
		regM = 1;
		break;
		/* Copy B to D */
	case 0x50:
		regD = regB;
		regM = 1;
		break;
		/* Copy C to D */
	case 0x51:
		regD = regC;
		regM = 1;
		break;
		/* Copy D to D */
	case 0x52:
		regD = regD;
		regM = 1;
		break;
		/* Copy E to D */
	case 0x53:
		regD = regE;
		regM = 1;
		break;
		/* Copy H to D */
	case 0x54:
		regD = regH;
		regM = 1;
		break;
		/* Copy L to D */
	case 0x55:
		regD = regL;
		regM = 1;
		break;
		/* Copy value pointed by HL to D */
	case 0x56:
		regD = pMem->Read8((regH << 8) + regL);
		regM = 2;
		break;
		/* Copy A to D */
	case 0x57:
		regD = regA;
		regM = 1;
		break;
		/* Copy B to E */
	case 0x58:
		regE = regB;
		regM = 1;
		break;
		/* Copy C to E */
	case 0x59:
		regE = regC;
		regM = 1;
		break;
		/* Copy D to E */
	case 0x5A:
		regE = regD;
		regM = 1;
		break;
		/* Copy E to E */
	case 0x5B:
		regE = regE;
		regM = 1;
		break;
		/* Copy H to E */
	case 0x5C:
		regE = regH;
		regM = 1;
		break;
		/* Copy L to E */
	case 0x5D:
		regE = regL;
		regM = 1;
		break;
		/* Copy value pointed by HL to E */
	case 0x5E:
		regE = pMem->Read8((regH << 8) + regL);
		regM = 2;
		break;
		/* Copy A to E */
	case 0x5F:
		regE = regA;
		regM = 1;
		break;

	}

}
