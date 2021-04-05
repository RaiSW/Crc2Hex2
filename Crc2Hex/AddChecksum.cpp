#include "AddChecksum.h"

AddChecksum::AddChecksum(void)
{
	ulSum = 0;
}

uint32_t AddChecksum::Add(uint8_t* pucStart, uint8_t* pucEnd)
{
	while (pucStart <= pucEnd)
			ulSum = (ulSum + *(pucStart++)) & 0xff;
	return (ulSum);
}

uint32_t AddChecksum::Add(uint16_t* puiStart, uint16_t* puiEnd)
{
	while (puiStart <= puiEnd)
		ulSum = (ulSum + *(puiStart++)) & 0xffff;
	return (ulSum);
}

uint32_t AddChecksum::Add(uint32_t* pulStart, uint32_t* pulEnd)
{
	while (pulStart <= pulEnd)
		ulSum += *(pulStart++);
	return (ulSum);
}