#include "CRC16.h"

CRC16::CRC16(void)
{
	uiSum = 0;
}

CRC16::CRC16(uint16_t uiValue)
{
	CRC16();
	uiSum = uiValue;
}

uint16_t CRC16::Add(uint8_t* pucStart, uint8_t* pucEnd)
{
    uint8_t value;
    uint16_t i;

    while (pucStart <= pucEnd)
    {
        value = *((uint8_t*)pucStart);
        for (i = 0; i < 8; i++)
        {
            if ((uiSum ^ value) & 1)
            {
                uiSum = (uiSum >> 1) ^ GENERATOR_POLYNOM;
            }
            else
                uiSum >>= 1;
            value >>= 1;
        }
        ++pucStart;
    }
    return (uiSum);
}
