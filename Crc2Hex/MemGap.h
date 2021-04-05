#pragma once
#include <iostream>
#include <vector>

using namespace std;

class MemGap
{
private:
	struct sMemRange
	{
		uint32_t ulStart = 0;
		uint32_t ulEnd = 0;
	};
	sMemRange fullMemRange;
	vector<sMemRange> vMemRangeList;
public:
	MemGap(void);
	void Init(uint32_t ulStart, uint32_t ulEnd);
	uint32_t Size(void);
	uint32_t Start(uint16_t index);
	uint32_t End(uint16_t index);
	void Add(uint32_t ulStart, uint32_t ulEnd);
};

