#include "Z80.h"
#include <stdio.h>
#include <cstdint>
#include <Windows.h>
#include <string>

Z80::Z80(MMU *pMemory)
{
	pMem = pMemory;
	Reset();
}

void Z80::Update()
{
	uint8_t opCode = pMem->Read8(regPc);
	regPc++;
	ExecuteInstruction(opCode);
	m_time += regM;

	char buff[100];
	sprintf(buff, "Frame: %d", m_time);
	std::string titleStr = buff;
	SetConsoleTitle(titleStr.c_str());
}

void Z80::Reset()
{
	m_time = 0;

	regA = regB = regC = regD = regE = regH = regL = flags =0;
	regPc = regSp = 0;
	regM =  0;
}


void Z80::ExecuteInstruction(uint8_t instruction)
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
			uint8_t ci = regA & Zero ? 1:0;
			uint8_t co = regA & Zero ? Carry : 0;
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
		uint8_t i = pMem->Read8(regPc);
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
		uint8_t i = pMem->Read8(regPc);
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
		uint8_t a = regA;
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
		uint8_t i = pMem->Read8(regPc);
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
			uint8_t i = pMem->Read8(regPc);
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
		{
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
		}
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
		/* Copy B to H*/
	case 0x60:
		regH = regB;
		regM = 1;
		break;
		/* Copy C to H */
	case 0x61:
		regH = regC;
		regM = 1;
		break;
		/* Copy D to H */
	case 0x62:
		regH = regD;
		regM = 1;
		break;
		/* Copy E to H */
	case 0x63:
		regH = regE;
		regM = 1;
		break;
		/* Copy H to H */
	case 0x64:
		regH = regH;
		regM = 1;
		break;
		/* Copy L to H */
	case 0x65:
		regH = regL;
		regM = 1;
		break;
		/* Copy value pointed by HL to H */
	case 0x66:
		regH = pMem->Read8((regH << 8) + regL);
		regM = 2;
		break;
		/* Copy A to H */
	case 0x67:
		regH = regA;
		regM = 1;
		break;
		/* Copy B to L */
	case 0x68:
		regL = regB;
		regM = 1;
		break;
		/* Copy C to L */
	case 0x69:
		regL = regC;
		regM = 1;
		break;
		/* Copy D to L */
	case 0x6A:
		regL = regD;
		regM = 1;
		break;
		/* Copy E to L */
	case 0x6B:
		regL = regE;
		regM = 1;
		break;
		/* Copy H to L */
	case 0x6C:
		regL = regH;
		regM = 1;
		break;
		/* Copy L to L */
	case 0x6D:
		regL = regL;
		regM = 1;
		break;
		/* Copy value pointed by HL to L */
	case 0x6E:
		regL = pMem->Read8((regH << 8) + regL);
		regM = 2;
		break;
		/* Copy A to L */
	case 0x6F:
		regL = regA;
		regM = 1;
		break;
		/* Copy B to address pointed by HL */
	case 0x70:
		pMem->Write8(regB, (regH << 8) + regL);
		regM = 2;
		break;
		/* Copy C to address pointed by HL */
	case 0x71:
		pMem->Write8(regC, (regH << 8) + regL);
		regM = 2;
		break;
		/* Copy D to address pointed by HL */
	case 0x72:
		pMem->Write8(regD, (regH << 8) + regL);
		regM = 2;
		break;
		/* Copy E to address pointed by HL */
	case 0x73:
		pMem->Write8(regE, (regH << 8) + regL);
		regM = 2;
		break;
		/* Copy H to address pointed by HL */
	case 0x74:
		pMem->Write8(regH, (regH << 8) + regL);
		regM = 2;
		break;
		/* Copy L to address pointed by HL */
	case 0x75:
		pMem->Write8(regL, (regH << 8) + regL);
		regM = 2;
		break;
		/* HALT */
	case 0x76:
			printf("HALT");
			break;
		/* Copy A to address pointed by HL */
	case 0x77:
		pMem->Write8(regA, (regH << 8) + regL);
		regM = 2;
		break;
		/* Copy B to A */
	case 0x78:
		regA = regB;
		regM = 1;
		break;
		/* Copy C to A */
	case 0x79:
		regA = regC;
		regM = 1;
		break;
		/* Copy D to A */
	case 0x7A:
		regA = regD;
		regM = 1;
		break;
		/* Copy E to A */
	case 0x7B:
		regA = regE;
		regM = 1;
		break;
		/* Copy H to A */
	case 0x7C:
		regA = regH;
		regM = 1;
		break;
		/* Copy L to A */
	case 0x7D:
		regA = regL;
		regM = 1;
		break;
		/* Copy value pointed by HL to A */
	case 0x7E:
		regA = pMem->Read8((regH << 8) + regL);
		regM = 2;
		break;
		/* Copy A to A */
	case 0x7F:
		regA = regA;
		regM = 1;
		break;
		/*Add B to A */
	case 0x80:
		AddRegisters(regB, regA);
		break;
		/* Add C to A */
	case 0x81:
		AddRegisters(regC, regA);
		break;
		/* Add D to A */
	case 0x82:
		AddRegisters(regD, regA);
		break;
		/* Add  E to A */
	case 0x83:
		AddRegisters(regE, regA);
		break;
		/* Add H to A */
	case 0x84:
		AddRegisters(regH, regA);
		break;
		/* Add L to A */
	case 0x85:
		AddRegisters(regL, regA);
		break;
		/* Add value pointed by HL to A*/
	case 0x86:
		AddRegisters(pMem->Read8((regH << 8) + regL), regA);
		regM =2;
		break;
		/* Add A to A */
	case 0x87:
		AddRegisters(regA, regA);
		break;
		/* Add B and carry flag to A */
	case 0x88:
		AddRegistersAndCarry(regB, regA);
		break;
		/* Add C and carry flag to A */
	case 0x89:
		AddRegistersAndCarry(regC, regA);
		break;
		/* Add D and carry flag to A */
	case 0x8A:
		AddRegistersAndCarry(regD, regA);
		break;
		/* Add E and carry flag to A */
	case 0x8B:
		AddRegistersAndCarry(regE, regA);
		break;
		/* Add H and carry flag to A */
	case 0x8C:
		AddRegistersAndCarry(regH, regA);
		break;
		/* Add and carry flag L to A */
	case 0x8D:
		AddRegistersAndCarry(regL, regA);
		break;
		/* Add value pointed by HL and carry flag to A */
	case 0x8E:
		AddRegistersAndCarry(pMem->Read8((regH << 8) + regL), regA);
		regM = 2;
		break;
		/* Add A and carry flag to A */
	case 0x8F:
		AddRegistersAndCarry(regA, regA);
		break;
		/* Subtract B from A */
	case 0x90:
		SubtractRegisters(regB, regA);
		break;
		/* Subtract C from A */
	case 0x91:
		SubtractRegisters(regC, regA);
		break;
		/* Subtract D from A */
	case 0x92:
		SubtractRegisters(regD, regA);
		break;
		/* Subtract E from A */
	case 0x93:
		SubtractRegisters(regE, regA);
		break;
		/* Subtract H from A */
	case 0x94:
		SubtractRegisters(regH, regA);
		break;
		/* Subtract L from A */
	case 0x95:
		SubtractRegisters(regL, regA);
		break;
		/* Subtract value pointed by HL from A */
	case 0x96:
		SubtractRegisters(pMem->Read8((regH << 8) + regL), regA);
		regM = 2;
		break;
		/* Subtract A from A */
	case 0x97:
		SubtractRegisters(regA, regA);
		break;
		/* Subtract B and carry flag from A */
	case 0x98:
		SubtractRegistersAndCarry(regB, regA);
		break;
		/* Subtract C and carry flag from A */
	case 0x99:
		SubtractRegistersAndCarry(regC, regA);
		break;
		/* Subtract D and carry flag from A*/
	case 0x9A:
		SubtractRegistersAndCarry(regD, regA);
		break;
		/* Subtract E and carry flag from A*/
	case 0x9B:
		SubtractRegistersAndCarry(regE, regA);
		break;
		/* Subtract H and carry flag from A*/
	case 0x9C:
		SubtractRegistersAndCarry(regH, regA);
		break;
		/* Subtract L and carry flag from A*/
	case 0x9D:
		SubtractRegistersAndCarry(regL, regA);
		break;
		/* Subtract value pointed by HL and carry flag from A */
	case 0x9E:
		SubtractRegistersAndCarry(pMem->Read8((regH << 8) + regL), regA);
		regM = 2;
		break;
		/* Subtract A and carry flag from A */
	case 0x9F:
		SubtractRegistersAndCarry(regA, regA);
		break;
		/* Logical AND B against A */
	case 0xA0:
		AndRegisters(regB, regA);
		break;
		/* Logical AND C against A */
	case 0xA1:
		AndRegisters(regC, regA);
		break;
		/* Logical AND D against A */
	case 0xA2:
		AndRegisters(regD, regA);
		break;
		/* Logical AND E against A */
	case 0xA3:
		AndRegisters(regE, regA);
		break;
		/* Logical AND H against A */
	case 0xA4:
		AndRegisters(regH, regA);
		break;
		/* Logical AND L against A */
	case 0xA5:
		AndRegisters(regL, regA);
		break;
		/* Logical AND value pointed by HL against A */
	case 0xA6:
		AndRegisters(pMem->Read8((regH << 8) + regL), regA);
		regM = 2;
		break;
		/* Logical AND A against A */
	case 0xA7:
		AndRegisters(regA, regA);
		break;
		/* Logical XOR B against A */
	case 0xA8:
		XorRegisters(regB, regA);
		break;
		/* Logical XOR C against A */
	case 0xA9:
		XorRegisters(regC, regA);
		break;
		/* Logical XOR D against A */
	case 0xAA:
		XorRegisters(regD, regA);
		break;
		/* Logical XOR E against A */
	case 0xAB:
		XorRegisters(regE, regA);
		break;
		/* Logical XOR H against A */
	case 0xAC:
		XorRegisters(regH, regA);
		break;
		/* Logical XOR L against A */
	case 0xAD:
		XorRegisters(regL, regA);
		break;
		/* Logical XOR value pointed by HL against A */
	case 0xAE:
		XorRegisters(pMem->Read8((regH << 8) + regL), regA);
		regM = 2;
		break;
		/* Logical XOR A against A */
	case 0xAF:
		XorRegisters(regA, regA);
		break;
		/* Logical OR B against A */
	case 0xB0:
		OrRegisters(regB, regA);
		break;
		/* Logical OR C against A */
	case 0xB1:
		OrRegisters(regC, regA);
		break;
		/* Logical OR D against A */
	case 0xB2:
		OrRegisters(regD, regA);
		break;
		/* Logical OR E against A */
	case 0xB3:
		OrRegisters(regE, regA);
		break;
		/* Logical OR H against A */
	case 0xB4:
		OrRegisters(regH, regA);
		break;
		/* Logical OR L against A */
	case 0xB5:
		OrRegisters(regL, regA);
		break;
		/* Logical OR value pointed by HL against A */
	case 0xB6:
		OrRegisters(pMem->Read8((regH << 8) + regL), regA);
		break;
		/* Logical OR A against A */
	case 0xB7:
		OrRegisters(regA, regA);
		break;
		/* Compare B against A*/
	case 0xB8:
		CompareRegisters(regB, regA);
		break;
		/* Compare C against A*/
	case 0xB9:
		CompareRegisters(regC, regA);
		break;
		/* Compare D against A*/
	case 0xBA:
		CompareRegisters(regD, regA);
		break;
		/* Compare E against A*/
	case 0xBB:
		CompareRegisters(regE, regA);
		break;
		/* Compare H against A*/
	case 0xBC:
		CompareRegisters(regH, regA);
		break;
		/* Compare L against A*/
	case 0xBD:
		CompareRegisters(regL, regA);
		break;
		/* Compare value pointed by HL against A*/
	case 0xBE:
		CompareRegisters(pMem->Read8((regH << 8) + regL), regA);
		regM = 2;
		break;
		/* Compare A against A*/
	case 0xBF:
		CompareRegisters(regA, regA);
		break;
		/* Return if last result was not zero */
	case 0xC0:
		regM = 1;
		if((flags & Zero) == 0x00)
			regPc = pMem->Read16(regSp);
		regSp+=2;
		regM +=2;
		break;
		/* Pop 16-bit value from stack into BC */
	case 0xC1:
		regC = pMem->Read8(regSp);
		regSp++;
		regB = pMem->Read8(regSp);
		regSp++;
		regM = 3;
		break;
		/* Absolute jump to 16-bit location if last result was not zero */
	case 0xC2:
		regM = 3;
		if((flags & Zero) == 0)
		{
			regPc = pMem->Read16(regPc);
			regM++;
		}
		else
			regPc+=2;
		break;
		/* Absolute jump to 16 bit location */
	case 0xC3:
		regPc = pMem->Read16(regPc);
		regM = 3;
		break;
		/* Call routine at 16-bit location if last result was not zero */
	case 0xC4:
		regM = 3;
		if((flags & Zero) == 0)
		{
			regSp -= 2;
			pMem->Write8((regPc + 2) & 0xFF, regSp);
			pMem->Write8((regPc + 2) >> 8, regSp + 1);
			regPc = pMem->Read16(regPc);
			regM +=2;
		}
		else
			regPc +=2;
		break;
		/* Push 16-bit BC onto stack */
	case 0xC5:
		regSp--;
		pMem->Write8(regB, regSp);
		regSp--;
		pMem->Write8(regC, regSp);
		regM = 3;
		break;
		/* Add 8-bit immediate to A */
	case 0xC6:
		AddRegisters(pMem->Read8(regPc), regA);
		regPc++;
		regM = 2;
		break;
		/* Call routine at address 0x0000H */
	case 0xC7:
		Rsv();
		CallRoutine(0x0000);
		break;
		/* Return if last result was zero */
	case 0xC8:
		regM = 1;
		if((flags & Zero) == Zero)
		{
			regPc = pMem->Read16(regSp);
			regSp+=2;
			regM+=2;
		}
		break;
		/* Return to calling routine */
	case 0xC9:
		regPc = pMem->Read16(regSp);
		regSp += 2;
		regM = 3;
		break;
		/* Absolute jump to 16-bit location if last result was zero */
	case 0xCA:
		regM = 3;
		if((flags & Zero) == Zero)
		{
			regPc = pMem->Read16(regPc);
			regM++;
		}
		else
			regPc+=2;
		break;
		/* Extended operations (two-byte instruction code) */
	case 0xCB:
		{
			uint8_t i = pMem->Read8(regPc);
			regPc++;
			regPc &= 0xFFFF;
			//printf("Call CB map index: %d\n", i);
			ExecuteCBInstruction(i);
			break;
		}
		/* Call Routine at 16-bit location if last result was zero */
	case 0xCC:
		CallRoutineIfZero(pMem->Read16(regPc));
		break;
		/* Call Routine at 16-bit location */
	case 0xCD:
		CallRoutine(pMem->Read16(regPc));
		regM = 5;
		break;
		/* Add 8-bit immediate and carry to A */
	case 0xCE:
		AddRegistersAndCarry(pMem->Read8(regPc), regA);
		regM = 2;
		break;
		/* Call routine at address 0x08 */
	case 0xCF:
		Rsv();
		CallRoutine(0x08);
		break;
		/* Return if last result caused no carry */
	case 0xD0:
		regM = 1;
		if((flags & Carry) == 0x00)
			regPc = pMem->Read16(regSp);
		regSp+=2;
		regM +=2;
		break;
		/* Pop 16-bit value from stack into DE */
	case 0xD1:
		regE = pMem->Read8(regSp);
		regSp++;
		regD = pMem->Read8(regSp);
		regSp++;
		regM = 3;
		break;
		/* Absolute jump to 16-bit location if last result caused no carry */
	case 0xD2:
		regM = 3;
		if((flags & Carry) == 0)
		{
			regPc = pMem->Read16(regPc);
			regM++;
		}
		else
			regPc+=2;
		break;
		/* Operation Removed */
	case 0xD3:
		NopWarn(instruction);
		break;
		/* Call routine at 16-bit location if last result caused no carry */
	case 0xD4:
		regM = 3;
		if((flags & Carry) == 0)
		{
			regSp -= 2;
			pMem->Write8((regPc + 2) & 0xFF, regSp);
			pMem->Write8((regPc + 2) >> 8, regSp + 1);
			regPc = pMem->Read16(regPc);
			regM +=2;
		}
		else
			regPc +=2;
		break;
		/* Push 16-bit DE onto stack */
	case 0xD5:
		regSp--;
		pMem->Write8(regD, regSp);
		regSp--;
		pMem->Write8(regE, regSp);
		regM = 3;
		break;
		/* Subtract 8-bit immediate from A */
	case 0xD6:
		SubtractRegisters(pMem->Read8(regPc), regA);
		regM = 2;
		break;
		/* Call routine at address 0x10 */
	case 0xD7:
		Rsv();
		CallRoutine(0x10);
		break;
		/* Return if last result caused carry */
	case 0xD8:
		regM = 1;
		if((flags & Carry) == Carry)
			regPc = pMem->Read16(regSp);
		regSp+=2;
		regM +=2;
		break;
		/* Enable interrupts and return to calling routine */
	case 0xD9:
		printf("[Unhandled] Enable interrupts, return to call routine.\n");
		Rrs();
		regPc = pMem->Read16(regSp);
		regSp += 2;
		regM = 3;
		break;
		/* Absolute jump to 16-bit location if last result caused carry */
	case 0xDA:
		regM = 3;
		if((flags & Carry) == Carry)
		{
			regPc = pMem->Read16(regPc);
			regM++;
		}
		else
			regPc+=2;
		break;
		/* Operation Removed*/
	case 0xDB:
		NopWarn(instruction);
		break;
		/* Call routine at 16-bit location if last result caused carry */
	case 0xDC:
		regM = 3;
		if((flags & Carry) == Carry)
		{
			regSp -= 2;
			pMem->Write8((regPc + 2) & 0xFF, regSp);
			pMem->Write8((regPc + 2) >> 8, regSp + 1);
			regPc = pMem->Read16(regPc);
			regM +=2;
		}
		else
			regPc +=2;
		break;
		/* Operation Removed */
	case 0xDD:
		NopWarn(instruction);
		break;
		/* Subtract 8-bit immediate and carry from A */
	case 0xDE:
		SubtractRegistersAndCarry(pMem->Read8(regPc), regA);
		regM = 2;
		break;
		/* Call routine at address 0x18 */
	case 0xDF:
		Rsv();
		CallRoutine(0x18);
		break;
		/* Save A at address pointed to by (0xFF00 + 8-bit immediate) */
	case 0xE0:
		pMem->Write8(regA, 0xFF00 + pMem->Read8(regPc));
		regPc++;
		regM = 3;
		break;
		/* Pop 16-bit value from stack onto HL */
	case 0xE1:
		PopStack(regH, regL);
		break;
		/* Save A at address pointed to by (0xFF00 + regC) */
	case 0xE2:
		pMem->Write8(regA, 0xFF00 + regC);
		regPc++;
		regM = 3;
		break;
		/* Operation Removed */
	case 0xE3:
		NopWarn(instruction);
		break;
		/* Operation Removed */
	case 0xE4:
		NopWarn(instruction);
		break;
		/* Push 16-bit HL onto stack */
	case 0xE5:
		PushStack(regH, regL);
		break;
		/* Logical AND 8-bit immediate against A */
	case 0xE6:
		AndRegisters(pMem->Read8(regSp), regA);
		regM = 2;
		break;
		/* Call routine at address 0x0020 */
	case 0xE7:
		Rsv();
		CallRoutine(0x0020);
		break;
		/* Add signed 8-bit immediate to SP */
	case 0xE8:
		{
			uint8_t i = pMem->Read8(regPc);
			if(i > 127)
				i= -((~i+1)&0xFF);
			regPc++;
			regSp += i;
			regM = 4;
			break;
		}
		/* Jump to 16-bit value pointed by HL */
	case 0xE9:
		regPc = pMem->Read16((regH << 8) + regL);
		regM = 1;
		break;
		/* Save A at given 16-bit address */
	case 0xEA:
		pMem->Write8(regA, pMem->Read16(regPc));
		regPc += 2;
		regM = 4;
		break;
		/* Operation Removed */
	case 0xEB:
		NopWarn(instruction);
		break;
		/* Operation Removed */
	case 0xEC:
		NopWarn(instruction);
		break;
		/* Operation Removed */
	case 0xED:
		NopWarn(instruction);
		break;
		/* Logical XOR 8-bit immediate against A */
	case 0xEE:
		XorRegisters(pMem->Read8(regPc), regA);
		regPc++;
		regM = 2;
		break;
		/* Call routine at address 0x0028 */
	case 0xEF:
		Rsv();
		CallRoutine(0x0028);
		break;
		/* Load A from address pointed to by (0xFF00 + 8-bit immediate) */
	case 0xF0:
		regA = pMem->Read8(0xFF00 + pMem->Read8(regPc));
		regPc++;
		regM = 3;
		break;
		/* Pop 16-bit value from stack into AF */
	case 0xF1:
		PopStack(regA, flags);
		break;
		/* Operation Removed */
	case 0xF2:
		NopWarn(instruction);
		break;
		/* Disable Interrupts */
	case 0xF3:
		printf("[Unhandled] Disable interrupts.\n");
		break;
		/* Operation Removed */
	case 0xF4:
		NopWarn(instruction);
		break;
		/* Push 16-bit AF onto stack */
	case 0xF5:
		PushStack(regA, flags);
		break;
		/*Logical OR 8-bit immediate against A */
	case 0xF6:
		XorRegisters(pMem->Read8(regPc), regA);
		regPc++;
		regM = 3;
		break;
		/* Call routine at address 0x0030 */
	case 0xF7:
		Rsv();
		CallRoutine(0x0030);
		break;
		/* Add signed 8-bit immediate to SP and save result in HL */
	case 0xF8:
		{
			uint8_t i = pMem->Read8(regPc);
			if(i > 127)
				i= -((~i+1)&0xFF);
			regPc++;
			i+= regSp;
			regH = (i >> 8) & 0xFF;
			regL = i & 0xFF;
			regM = 3;
			break;
		}
		/* Copy HL to SP */
	case 0xF9:
		printf("[Warn] Not sure this opcode should exist. [%d]\n", instruction);
		regSp = (regH << 8) + regL;
		regM = 1;
		break;
		/* Load A from given 16-bit address */
	case 0xFA:
		regA = pMem->Read8(pMem->Read16(regPc));
		regPc  += 2;
		regM = 4;
		break;
		/* Enable Interrupts */
	case 0xFB:
		printf("[Unhandled] Enable interrupts.\n");
		regM = 1;
		break;
		/* Operation Removed */
	case 0xFC:
	case 0xFD:
		NopWarn(instruction);
		break;
		/* Compare 8-bit immediate against A */
	case 0xFE:
		CompareRegisters(pMem->Read8(regPc), regA);
		regPc++;
		regM = 3;
		break;
		/* Call routine at address 0x0038 */
	case 0xFF:
		Rsv();
		CallRoutine(0x0038);
		break;
	default:
		printf("Unhandled opcode: %X\n", instruction);
		break;
	}
}

void Z80::PushStack(uint8_t pushA, uint8_t pushB)
{
	regSp--;
	pMem->Write8(pushA, regSp);
	regSp--;
	pMem->Write8(pushB, regSp);
	regM = 3;
}

void Z80::PopStack(uint8_t &intoA, uint8_t &intoB)
{
	intoB = pMem->Read8(regSp);
	regSp++;
	intoA = pMem->Read8(regSp);
	regSp++;
	regM = 3;
}

void Z80::NopWarn(uint8_t op)
{
	printf("Removed opcode found, why? %X\n", op);
}

void Z80::CallRoutine(uint16_t address)
{
	regSp -= 2;
	pMem->Write8(regPc & 0xFF, regSp);
	pMem->Write8(regPc  >> 8, regSp + 1);
	regPc = address;
	regM = 3;
}

void Z80::CallRoutineIfZero(uint16_t address)
{
	regM = 3;
	if((flags & Zero) == Zero)
	{
		regSp -= 2;
		pMem->Write8(regPc & 0xFF, regSp);
		pMem->Write8(regPc  >> 8, regSp + 1);
		regPc = address;
		regM += 2;
	}
	else
	{
		regPc += 2;
	}
}

void Z80::Rsv()
{
	rsvA = regA;
	rsvB = regB;
	rsvC = regC;
	rsvD = regD;
	rsvE = regE;
	rsvH = regH;
	rsvL = regL;
	rsvF = flags;
}

void Z80::Rrs()
{
	regA = rsvA;
	regB = rsvB;
	regC = rsvC;
	regD = rsvD;
	regE = rsvE;
	regH = rsvH;
	regL = rsvL;
	flags = rsvF;
}

void Z80::AddRegisters(uint8_t add, uint8_t &to)
{
	int a = to;
	to += add;
	flags = (to > 0xFF) ? Carry : 0;
	to &= 0xFF;
	if(!to)
		flags |= Zero;
	if((to ^ add ^ a) & Carry)
		flags = Carry;
	regM = 1;
}

void Z80::AddRegistersAndCarry(uint8_t add, uint8_t &to)
{
	int a = to;
	to += add;
	to += (flags & Carry) ? 1 : 0;
	flags = (to > 0xFF) ? Carry : 0;
	to &= 0xFF;
	if(!to)
		flags |= Zero;
	if((to ^ add ^ a) & Carry)
		flags |= HalfCarry;
	regM = 1;
}

void Z80::SubtractRegisters(uint8_t sub, uint8_t &from)
{
	int a = from;
	from -= sub;
	flags = (from < 0x0) ? 0x50 : Operation;
	from &= 0xFF;
	if(!from)
		flags |= Zero;
	if((from ^ sub ^ a) & Carry)
		flags = HalfCarry;
	regM = 1;
}

//Not sure why this is a duplicate of the above, above may be wrong...
void Z80::SubtractRegistersAndCarry(uint8_t sub, uint8_t &from)
{
	int a = from;
	from -= sub;
	flags = (from < 0) ? 0x50 : Operation;
	if(!from)
		flags |= Zero;
	if((from ^ sub ^ a) & Carry)
		flags |= HalfCarry;
	regM = 1;
}

void Z80::AndRegisters(uint8_t from, uint8_t &against)
{
	against &= from;
	against &= 0xFF;
	flags = against ? 0 : Zero;
	regM = 1;
}

void Z80::XorRegisters(uint8_t from, uint8_t &against)
{
	against ^= from;
	against &= 0xFF;
	flags = against ? 0 : Zero;
	regM = 1;
}

void Z80::OrRegisters(uint8_t from, uint8_t &against)
{
	against |= from;
	against &= 0xFF;
	flags = against ? 0 : Zero;
	regM = 1;
}

void Z80::CompareRegisters(uint8_t reg, uint8_t against)
{
	int i = against;
	i -= reg;
	flags = (i < 0) ? 0x50:Operation;
	if(!i)
		flags |= Zero;
	if((reg ^ against ^ i) & Carry)
		flags |= HalfCarry;

	regM = 1;
}


void Z80::ExecuteCBInstruction(uint8_t instruction)
{
	//High five bits are opcode while lower 3 denote register.
	uint8_t opCode = instruction & 0xF8;
	uint8_t regIndex = instruction & 0x7;

	uint8_t &reg = RemapIndexToRegister(regIndex);

	switch(opCode)
	{
		/* Rotate with Carry */
	case 0x0:
		RotateWithCarry(reg);		
		break;
		/* Rotate Right with Carry */
	case 0x8:
		RotateRightWithCarry(reg);
		break;
		/* Rotate Left */
	case 0x10:
		RotateLeft(reg);
		break;
		/* Rotate Right */
	case 0x18:
		RotateRight(reg);
		break;
		/* Shift Left Preserving Sign */
	case 0x20:
		ShiftLeftPreservingSign(reg);
		break;
		/* Shift Right Preserving Sign */
	case 0x28:
		ShiftRightPreservingSign(reg);
		break;
		/* Swap nybbles */
	case 0x30:
		SwapNybbles(reg);
		break;
		/* Shift Right */
	case 0x38:
		ShiftRight(reg);
		break;
		/* Test Bit 0*/
	case 0x40:
		TestBit(reg, 0x01);
		break;
		/* Test Bit 1*/
	case 0x48:
		TestBit(reg, 0x02);
		break;
		/* Test Bit 2*/
	case 0x50:
		TestBit(reg, 0x04);
		break;
		/* Test Bit 3*/
	case 0x58:
		TestBit(reg, 0x08);
		break;
		/* Test Bit 4*/
	case 0x60:
		TestBit(reg, 0x10);
		break;
		/* Test Bit 5*/
	case 0x68:
		TestBit(reg, 0x20);
		break;
		/* Test Bit 6*/
	case 0x70:
		TestBit(reg, 0x40);
		break;
		/* Test Bit 7*/
	case 0x78:
		TestBit(reg, 0x80);
		break;
		/* Clear Bit 0*/
	case 0x80:
		ClearBit(reg, 0xFE);
		break;
		/* Clear Bit 1*/
	case 0x88:
		ClearBit(reg, 0xFD);
		break;
		/* Clear Bit 2*/
	case 0x90:
		ClearBit(reg, 0xFB);
		break;
		/* Clear Bit 3*/
	case 0x98:
		ClearBit(reg, 0xF7);
		break;
		/* Clear Bit 4*/
	case 0xA0:
		ClearBit(reg, 0xEF);
		break;
		/* Clear Bit 5*/
	case 0xA8:
		ClearBit(reg, 0xDF);
		break;
		/* Clear Bit 6*/
	case 0xB0:
		ClearBit(reg, 0xBF);
		break;
		/* Clear Bit 7*/
	case 0xB8:
		ClearBit(reg, 0x7F);
		break;
		/* Set Bit 0 */
	case 0xC0:
		SetBit(reg, 0x01);
		break;
		/* Set Bit 1 */
	case 0xC8:
		SetBit(reg, 0x02);
		break;
		/* Set Bit 2 */
	case 0xD0:
		SetBit(reg, 0x04);
		break;
		/* Set Bit 3 */
	case 0xD8:
		SetBit(reg, 0x08);
		break;
		/* Set Bit 4 */
	case 0xE0:
		SetBit(reg, 0x10);
		break;
		/* Set Bit 5 */
	case 0xE8:
		SetBit(reg, 0x20);
		break;
		/* Set Bit 6 */
	case 0xF0:
		SetBit(reg, 0x40);
		break;
		/* Set Bit 7 */
	case 0xF8:
		SetBit(reg, 0x80);
		break;
	default:
		printf("[Unhandled] Unknown CB opcode %X\n", instruction);
		break;
	}
}

uint8_t &Z80::RemapIndexToRegister(uint8_t index)
{
	switch(index)
	{
	case 0:
		return regB;
	case 1:
		return regC;
	case 2:
		return regD;
	case 3:
		return regE;
	case 4:
		return regH;
	case 5:
		return regL;
	case 6:
		//return pMem->Read8((regH << 8) + regL);
		printf("to do...");
		break;
	case 7:
		return regA;
	}

	printf("[Error] failed to remap index.");
	uint8_t dummy;
	return dummy;
}

void Z80::RotateWithCarry(uint8_t &reg)
{
	uint8_t ci = reg & Zero ? 1 : 0;
	uint8_t co = reg & Zero ? Carry : 0;
	reg = (reg << 1) + ci;
	reg &= 0xFF;
	flags = reg ? 0 : Zero;
	flags = (flags & 0xEF)+ co;
	regM = 2;
}

void Z80::RotateRightWithCarry(uint8_t &reg)
{
	uint8_t ci = reg & 1 ? Zero : 0;
	uint8_t co = reg & 1 ? Carry : 0;
	reg = (reg >> 1) + ci;
	reg &= 0xFF;
	flags = (reg) ? 0 : Zero;
	flags = (flags & 0xEF) + co;
	regM = 2;
}

void Z80::RotateLeft(uint8_t &reg)
{
	uint8_t ci = flags & Carry ? 1 : 0;
	uint8_t co = reg & Zero ? Carry : 0;
	reg = (reg << 1) + ci;
	reg &= 0xFF;
	flags = reg ? 0 : Zero;
	flags = (flags & 0xEF) + co;
	regM = 2;
}

void Z80::RotateRight(uint8_t &reg)
{
	uint8_t ci = flags & Carry ? Zero : 0;
	uint8_t co = reg & 1 ? Carry : 0;
	reg = (reg >> 1) + ci;
	reg &= 0xFF;
	flags = reg ? 0 : Zero;
	flags = (flags & 0xEF) + co;
	regM = 2;
}

void Z80::ShiftLeftPreservingSign(uint8_t &reg)
{
	uint8_t co = reg & Zero ? Carry : 0;
	reg = (reg << 1) & 0xFF;
	flags = reg ? 0 : Zero;
	flags = (flags & 0xEF) + co;
	regM = 2;
}

void Z80::ShiftRightPreservingSign(uint8_t &reg)
{
	uint8_t ci = reg & Zero;
	uint8_t co = reg & 1 ? Carry : 0;
	reg = ((reg >> 1) + ci) & 0xFF;
	flags = reg ? 0 : Zero;
	flags = (flags & 0xEF) + co;
	regM = 2;
}

void Z80::SwapNybbles(uint8_t &reg)
{
	uint8_t tr = reg;
	reg = ((tr & 0xF) << 4)|((tr&0xF0)>>4);
	flags = reg ? 0 : Zero;
	regM = 1;
}

void Z80::ShiftRight(uint8_t &reg)
{
	uint8_t co = reg & 1 ? Carry : 0;
	reg = (reg >> 1) & 0xFF;
	flags = reg ? 0 : Zero;
	flags = (flags & 0xEF) + co;
	regM = 2;
}

void Z80::TestBit(uint8_t &reg, uint8_t bit)
{
	flags &= 0x1F;
	flags |= HalfCarry;
	flags = (reg & bit) ? 0 : Zero;
	regM = 2;
}

void Z80::ClearBit(uint8_t &reg, uint8_t bit)
{
	reg &= bit;
	regM = 2;
}

void Z80::SetBit(uint8_t &reg, uint8_t bit)
{
	reg |= bit;
	regM = 2;
}