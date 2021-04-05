#include <fstream>
#include "IntelHex.h"
#include "lib.h"

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
	this->mem = mem;
	this->memSize = memSize;
	return evalHexLine(hexLine, true);
}

int IntelHex::writeToFile(string filename, uint32_t ulAdr, uint8_t val, bool highByteFirst)
{
	return writeCommon(filename, ulAdr, &val, 1);
}

int IntelHex::writeToFile(string filename, uint32_t ulAdr, uint16_t val, bool highByteFirst)
{
	uint8_t ucVal[2];
	if (highByteFirst)
	{
		ucVal[0] = val / 256;
		ucVal[1] = val & 0xff;
	}
	else
	{
		ucVal[1] = val / 256;
		ucVal[0] = val & 0xff;
	}
	return writeCommon(filename, ulAdr, ucVal, 2);
}

int IntelHex::writeToFile(string filename, uint32_t ulAdr, uint32_t val, bool highByteFirst)
{
	uint8_t ucVal[4];
	if (highByteFirst)
	{
		ucVal[0] = (val / 0x1000000) & 0xff;
		ucVal[1] = (val / 0x10000) & 0xff;
		ucVal[2] = (val / 0x100) & 0xff;
		ucVal[3] = val & 0xff;
	}
	else
	{
		ucVal[3] = (val / 0x1000000) & 0xff;
		ucVal[2] = (val / 0x10000) & 0xff;
		ucVal[1] = (val / 0x100) & 0xff;
		ucVal[0] = val & 0xff;
	}
	return writeCommon(filename, ulAdr, ucVal, 4);
}

uint32_t IntelHex::MaxAddress(void)
{
	return ulMaxAdr;
}

int IntelHex::evalHexLine(string hexLine, bool bStoreToMem)
{
	int16_t iResult = INTHEX_OK;
	int16_t iLength = (int16_t)hexLine.length();
	uint8_t ucLineRecordType;
	uint32_t ulActHighAdr;

	bFindValidDataLine = false;

	if ((iLength >= 11) &&
		(iLength % 2) &&
		(hexLine[0] == ':'))
	{
		// each line must be longer than 11 characters, length shall be odd, and shall begin with ':'

		iLength = (iLength - 1) / 2; // no of data bytes in line w/o start character ':'
		hexLn2IntLn(&hexLine[1], ucLine, iLength);
		uiLineLen = ucLine[0];
		ucLineRecordType = ucLine[3];

		if (uiLineLen == iLength - 5) // no of data bytes shall be the same as length of line - header - checksum
		{
			if (Add8ChkSum(ucLine, ucLine + iLength - 1) == 0)
			{
				switch (ucLineRecordType)
				{
				case DATA_LINE:
					ulAdrOffset = ((uint32_t)ucLine[1] << 8) + ucLine[2];
					ulActHighAdr = ulExtLinAdr + ulExtSegAdr + ulAdrOffset + uiLineLen - 1;
					if (ulActHighAdr > ulMaxAdr)
						ulMaxAdr = ulActHighAdr;
					if (ulActHighAdr < memSize)
					{
						if (bStoreToMem)
							memcpy(mem + ulExtLinAdr + ulExtSegAdr + ulAdrOffset, &ucLine[4], uiLineLen);
					}
					else
						iResult = INTHEX_MEM_ERR;
					if (iResult == INTHEX_OK)
					    bFindValidDataLine = true;
					break;

				case END_OF_FILE: // End of File
					bFindLastLine = true;
					if (uiLineLen != 0)
						iResult = INTHEX_LENGTH_ERR;
					break;

				case EXT_SEG_ADR: // Extended Segnemt Adress Record
					// Segment-Address = 16 * value (0 ... 65535) -> 0 ... 1048576 (0xFFFF0)
					ulExtSegAdr = ((uint32_t)(ucLine[4] << 8) + ucLine[5]) << 4;
					if (uiLineLen != 2)
						iResult = INTHEX_LENGTH_ERR;
					break;

				case EXT_LIN_ADR: // Extended Linear Address Record
					// Linear Address = 65536 * value (0 ...  65535) -> 0 ... 4294901760 (0xFFFF0000)
					ulExtLinAdr = (((uint32_t)ucLine[4] << 8) + ucLine[5]) << 16;
					if (uiLineLen != 2)
						iResult = INTHEX_LENGTH_ERR;
					break;

				default:
					iResult = INTHEX_UNVALID_RECORD;
					break;
				}
				if (bFindLastLine && ucLineRecordType != END_OF_FILE)
					iResult = INTHEX_TXT_AFTER_END_ERR;
			}
			else
			{
				iResult = INTHEX_CHECKSUM_ERR;
			}
		}
		else
		{
			iResult = INTHEX_LENGTH_ERR;
		}
	}
	else
	{
		iResult = INTHEX_SYNTAX_ERR;
	}
	return iResult;
}


int IntelHex::writeCommon(string filename, uint32_t ulAdr, uint8_t* pucVal, uint8_t ucLen)
{
	fstream file;
	string sLine;
	string outLine;
	int iResult = INTHEX_OK;
	uint32_t ulLineAdr;
	bool bFound;
	int64_t lPos = 0;
	char cCheck;

	// Klasse zurücksetzen
	Reset();

	file.open(filename, ios_base::in | ios_base::out);
	if (file)
	{
		while (getline(file, sLine) && ucLen)
		{
			if ((iResult = evalHexLine(sLine, false)) == INTHEX_OK)
			{
				if (bFindValidDataLine)
				{
					ulLineAdr = ulExtLinAdr + ulExtSegAdr + ulAdrOffset;
					bFound = false;
					while (ucLen)
					{
						if ((ulAdr >= ulLineAdr) && ulAdr < (ulLineAdr + (uint32_t)uiLineLen))
						{
							ucLen--;
							ucLine[ulAdr - ulLineAdr + 4] = *(pucVal + ucLen);
							ulAdr++;
							bFound = true;
						}
						else
							break; // Address not in this line
					}
					if (bFound)
					{
						ucLine[uiLineLen + 4] = (uint8_t)(0x100 - Add8ChkSum(ucLine, ucLine + uiLineLen + 3));
						outLine = ":" + intLn2HexLn(ucLine, uiLineLen + 5);
						file.seekp(lPos);
						bFound = false;
						for (uint64_t i = 0; i < 10; i++)
						{
							file.seekp(lPos - i);
							file.read(&cCheck, 1);
							if (cCheck == ':')
							{
								bFound = true;
								file.seekp(lPos - i);
								break;
							}
						}
						if (bFound)
						{
							file.write(outLine.c_str(), 2*((uint64_t)uiLineLen + 5) + 1);
						}
					}
				}
				lPos = file.tellg();
			}
		}
		file.close();
	}
	else
		iResult = INTHEX_FILE_NOT_FOUND;
	return iResult;
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
