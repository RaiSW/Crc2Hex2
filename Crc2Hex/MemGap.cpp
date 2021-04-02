#include "MemGap.h"

MemGap::MemGap(void)
{
}

void MemGap::Init(uint32_t ulStart, uint32_t ulEnd)
{
	fullMemRange.ulStart = ulStart;
	fullMemRange.ulEnd = ulEnd;
	vMemRangeList.clear();
	vMemRangeList.push_back(fullMemRange);
}

uint16_t MemGap::Size(void)
{
	return(vMemRangeList.size());
}

uint32_t MemGap::Start(uint16_t index)
{
	return(vMemRangeList[index].ulStart);
}

uint32_t MemGap::End(uint16_t index)
{
	return(vMemRangeList[index].ulStart);
}

void MemGap::Add(uint32_t ulStart, uint32_t ulEnd)
{
    sMemRange memEintrag;
    uint16_t i;
    uint32_t ulTemp;

    // limit new Gap to full memory
    if (ulStart < fullMemRange.ulStart) ulStart = fullMemRange.ulStart;
    if (ulEnd > fullMemRange.ulEnd)   ulEnd = fullMemRange.ulEnd;

    for (i = 0; i < vMemRangeList.size(); i++)
    {
        // overlap of existing range ?
        if (ulStart <= vMemRangeList[i].ulStart && ulEnd >= vMemRangeList[i].ulEnd)
        {
            vMemRangeList.erase(vMemRangeList.begin() + i); // clear smaller existing range
            i--;
            continue; // next index
        }

        // overlap of low range
        if (ulStart <= vMemRangeList[i].ulStart && ulEnd >= vMemRangeList[i].ulStart)
        {
            vMemRangeList[i].ulStart = ulEnd + 1; // unteres Ende abschneiden
            //  Bereich löschen, wenn Größe < 0
            if (vMemRangeList[i].ulStart > vMemRangeList[i].ulEnd)
            {
                vMemRangeList.erase(vMemRangeList.begin() + i);
                i--;
            }
            continue;
        }

        // Überlappung am oberen Ende des Bereiches
        if (ulStart <= vMemRangeList[i].ulEnd && ulEnd >= vMemRangeList[i].ulEnd)
        {
            vMemRangeList[i].ulEnd = ulStart - 1; // oberes Ende abschneiden
            //  Bereich löschen, wenn Größe < 0
            if (vMemRangeList[i].ulStart > vMemRangeList[i].ulEnd)
            {
                vMemRangeList.erase(vMemRangeList.begin() + i);
                i--;
            }
            continue;
        }

        // Unterschneidung mit dem Bereich (Splitten)
        if (ulStart > vMemRangeList[i].ulStart && ulEnd < vMemRangeList[i].ulEnd)
        {
            ulTemp = vMemRangeList[i].ulEnd;
            vMemRangeList[i].ulEnd = ulStart - 1;
            memEintrag.ulStart = ulEnd + 1;
            memEintrag.ulEnd = ulTemp;
            vMemRangeList.insert(vMemRangeList.begin() + i + 1, memEintrag);
            break; // keine Überprüfung weiterer Bereiche notwendig
        }
    }
}