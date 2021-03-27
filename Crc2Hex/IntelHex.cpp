#include "IntelHex.h"
#include "lib.h"

using namespace std;

// Definition Record-Type of a IntelHEx line
#define DATA_LINE   0
#define END_OF_FILE 1
#define EXT_SEG_ADR 2
#define EXT_LIN_ADR 4

IntelHex::IntelHex(void)
{
	Reset();
}

void IntelHex::Reset(void)
{
	ulExtLinAdr = 0;
	ulExtSegAdr = 0;
	ulMaxAdr = 0;
	bFindLastLine = false;
}

int IntelHex::writeToMem(char* mem, uint32_t memSize, string hexLine)
{
	int iResult = INTHEX_OK;
	int iLength = hexLine.length();
	unsigned char ucLine[40];            //max length of Intelhex line in bytes
	int iLineLen;
	int iLineRecordType;
	uint32_t ulAdrOffset;

	if ((iLength >= 11) &&
		(iLength % 2) &&
		(hexLine[0] == ':'))
	{
		// each line must be longer than 11 characters, lngth shall be odd,  and shall begin with ':'

		iLength = (iLength - 1) / 2;
		hexLn2IntLn(&hexLine[1], ucLine, iLength);
		iLineLen = ucLine[0];
		iLineRecordType = ucLine[3];

		if (Add8ChkSum(ucLine, ucLine+iLength-1) == 0)
		{
			switch (iLineRecordType)
			{
			case DATA_LINE:
				if (iLineLen == iLength - 5)
				{
					ulAdrOffset = ((uint32_t)ucLine[1] << 8) + ucLine[2];
					uint32_t ulActHighAdr = ulExtLinAdr + ulExtSegAdr + ulAdrOffset + iLineLen - 1;
					if (ulActHighAdr > ulMaxAdr)
						ulMaxAdr = ulActHighAdr;
					if (ulActHighAdr < memSize)
						memcpy(mem + ulExtLinAdr + ulExtSegAdr + ulAdrOffset, &ucLine[4], iLineLen);
					else
						iResult = INTHEX_MEM_ERR;
				}
				else
				{
					iResult = INTHEX_LENGTH_ERR;
				}
				break;
			case END_OF_FILE: // End of File
				bFindLastLine = true;
				if (iLineLen != 0)
					iResult = INTHEX_LENGTH_ERR;
				break;
			case EXT_SEG_ADR: // Extended Segnemt Adress Record
				// Segment-Address = 16 * value (0 ... 65535) -> 0 ... 1048576 (0xFFFF0)
				ulExtSegAdr = ((uint32_t)(ucLine[4] << 8) + ucLine[5]) << 4;
				if (iLineLen != 2)
					iResult = INTHEX_LENGTH_ERR;
				break;
			case EXT_LIN_ADR: // Extended Linear Address Record
				// Linear Address = 65536 * value (0 ...  65535) -> 0 ... 4294901760 (0xFFFF0000)
				ulExtLinAdr = (((uint32_t)ucLine[4] << 8) + ucLine[5]) << 16;
				if (iLineLen != 2)
					iResult = INTHEX_LENGTH_ERR;
				break;
			default:
				break;
			}
			if (bFindLastLine && iLineRecordType != END_OF_FILE)
				iResult = INTHEX_TXT_AFTER_END_ERR;
		}
		else
		{
			iResult = INTHEX_CHECKSUM_ERR;
		}
	}
	else
	{
		iResult = INTHEX_SYNTAX_ERR;
	}
	return iResult;
}

uint32_t IntelHex::MaxAddress(void)
{
	return ulMaxAdr;
}

unsigned char IntelHex::Add8ChkSum(unsigned char* pStart, unsigned char* pEnd)
{
	unsigned char chkSum = 0;
	while (pStart <= pEnd)
	{
		chkSum += *pStart++;
	}
	return chkSum;
}
