#include <iostream>
#include "lib.h"

using namespace std;

const string WHITESPACE = " \n\r\t\f\v";

string ltrim(const string& s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == string::npos) ? "" : s.substr(start);
}

string rtrim(const string& s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == string::npos) ? "" : s.substr(0, end + 1);
}

string trim(const string& s)
{
    return rtrim(ltrim(s));
}

// Convert String in Groﬂbuchstaben
string UCase(string s)
{
	for (unsigned int i = 0; i < s.length(); i++)
	{
		s[i] = toupper(s[i]);
	}
	return s;
}

// Convert one Hex-Character to char
char hex2char(char value)
{
	value = toupper(value);
	value -= '0';
	if (value > 9)
	{
		if ((value >= 'A' - '0') &&	(value <= 'F' - '0'))
			value -= ('A' - '0' - 10);
		else
			value = -1; // error
	}
	return value;
}

// Convert Hex-String to int
int hexLn2IntLn(char* cSource, unsigned char* cDest, int iLen)
{
	char cHigh, cLow;
	int iResult = 0; // no error
	for (int i = 0; i < iLen; i++)
	{
		cHigh = hex2char(*cSource++);
		cLow = hex2char(*cSource++);
		if (cHigh >= 0 && cLow >= 0)
		{
			*cDest++ = (cHigh << 4) + cLow;
		}
		else
		{
			iResult = -1; // error
			break;
		}
	}
	return iResult;
}
