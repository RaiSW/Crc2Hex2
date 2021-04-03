#pragma once
#include <iostream>

using namespace std;

class CRC16
{
private:
#define GENERATOR_POLYNOM ((0x8005 / 4) | 0x8000)

public:
	uint16_t uiSum;
	CRC16(void);
	CRC16(uint16_t);
	uint16_t Add(uint8_t* pucStart, uint8_t* pucEnd);
};

