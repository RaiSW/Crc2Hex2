#pragma once
#include <iostream>
#include <string>

using namespace std;

class IntelHex
{
private:
	uint32_t ulExtLinAdr;
	uint32_t ulExtSegAdr;
	uint32_t ulMaxAdr;
	uint32_t ulAdrOffset;
	bool bFindLastLine;
	bool bFindValidDataLine;
	uint8_t ucLine[40];      //max length of Intelhex line in bytes
	uint16_t uiLineLen;
	char* mem;
	uint32_t memSize;

	unsigned char Add8ChkSum(unsigned char* pStart, unsigned char* pEnd);
	int writeCommon(string filename, uint32_t ulAdr, uint8_t* pucVal, uint8_t ucLen);
	int evalHexLine(string hexLine, bool bStoreToMem);
public:
	IntelHex(void);
	void Reset(void);
	uint32_t MaxAddress(void);
	int writeToMem(char* mem, uint32_t memSize, string hexLine);
	int writeToFile(string filename, uint32_t ulAdr, uint8_t val, bool highByteFirst);
	int writeToFile(string filename, uint32_t ulAdr, uint16_t val, bool highByteFirst);
	int writeToFile(string filename, uint32_t ulAdr, uint32_t val, bool highByteFirst);
};

// Rückmeldungen der Funktion Read() und Write()
#define INTHEX_OK                 0
#define INTHEX_SYNTAX_ERR         1
#define INTHEX_CHECKSUM_ERR       2
#define INTHEX_LENGTH_ERR         3
#define INTHEX_TXT_AFTER_END_ERR  4
#define INTHEX_MEM_ERR            5
#define INTHEX_FILE_NOT_FOUND     6
#define INTHEX_UNVALID_RECORD     7
// Reihenfolge, wie Words abgespeichert werden
#define LOW_BYTE_FIRST   0 // Low-Byte  an Low-Adress
#define HIGH_BYTE_FIRST  1 // High-Byte an Low-Adress

