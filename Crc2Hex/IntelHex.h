#pragma once
#include <iostream>
#include <string>

class IntelHex
{
private:
	uint32_t ulExtLinAdr;
	uint32_t ulExtSegAdr;
	uint32_t ulMaxAdr;
	bool bFindLastLine;

	unsigned char Add8ChkSum(unsigned char* pStart, unsigned char* pEnd);

public:
	IntelHex(void);
	void Reset(void);
	uint32_t MaxAddress(void);
	int writeToMem(char* mem, uint32_t memSize, std::string hexLine);
};
// Rückmeldungen der Funktion Read() und Write()
#define INTHEX_OK                 0
#define INTHEX_SYNTAX_ERR         1
#define INTHEX_CHECKSUM_ERR       2
#define INTHEX_LENGTH_ERR         3
#define INTHEX_TXT_AFTER_END_ERR  4
#define INTHEX_MEM_ERR            5
#define INTHEX_FILE_NOT_FOUND     6

