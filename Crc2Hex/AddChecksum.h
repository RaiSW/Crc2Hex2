#pragma once
#include <iostream>

using namespace std;

class AddChecksum
{
public:
	uint32_t ulSum;
	AddChecksum(void);
	uint32_t Add(uint8_t* pucStart, uint8_t* pucEnd);
	uint32_t Add(uint16_t* puiStart, uint16_t* puiEnd);
	uint32_t Add(uint32_t* pulStart, uint32_t* pulEnd);
};

