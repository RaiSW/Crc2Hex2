#pragma once
#include <iostream>

std::string ltrim(const std::string& s);
std::string rtrim(const std::string& s);
std::string trim(const std::string& s);
std::string UCase(const std::string s);
char hex2char(char value);
int hexLn2IntLn(char* cSource, unsigned char* cDest, int iLen);