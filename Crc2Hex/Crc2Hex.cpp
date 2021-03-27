#include <iostream>
#include <string>
#include <fstream>
#include "lib.h"
#include "IntelHex.h"

using namespace std;

#define PRG_VERSION     2
#define SUB_VERSION     1
#define SUB_SUB_VERSION 0

#define FLASH_SIZE 0x100000
#define OFFS_FLASH_START     56
#define OFFS_FLASH_END       60
#define OFFS_FLASH_GAP_START 64
#define OFFS_FLASH_GAP_END   68
#define OFFS_ADD32           72
#define OFFS_CRC16           76

void writeHelp(void);

int main(int argc, char** argv)
{
    int result = 1;
    bool bQuiet = false;
    bool bHelp = false;
    string fileName = "";
    string cfgFile = "";
    string s;
    char* mem = NULL;
    IntelHex iHex;


    cout << "CRC Builder for Intel-Hex-Files V" << to_string(PRG_VERSION);
    cout << "." << to_string(SUB_VERSION) << "." << to_string(SUB_SUB_VERSION) << endl;
    cout << "(c) 2021 AVENTICS GmbH - R&D Electronics Marine" << endl;

    if (argc >= 2)
    {   // es muss mindestens 1 Argument übergeben werden
        for (int i = 1; i < argc; i++)
        {
            string cmd = UCase(argv[i]);
            if (cmd == "QUIET")
            {
                bQuiet = true;
            }
            else if (cmd == "HELP")
            {
                bHelp = true;
            }
            else if (cmd.length() >= 3 && cmd.substr(0, 2) == "C:")
            {
                cfgFile = argv[i];
                cfgFile = cfgFile.substr(2, cfgFile.length() - 2);
            }
            else
            {
                fileName = argv[i];
            }
        }
        if (bHelp)
        {
            // Ausgabe des Hilfetextes
            writeHelp();
        }
        else
        {
            /* File bearbeiten */
            ifstream ifile;
            ifile.open(fileName);
            if (ifile)
            {
                mem = new char[FLASH_SIZE];
                memset(mem, 0xff, FLASH_SIZE);
                int err = 0;
                int line = 0;
                while (getline(ifile, s))
                {
                    err = iHex.writeToMem(mem,FLASH_SIZE, s);
                    line++;
                    if (err != 0)
                    {
                        break;
                    }
                }
                ifile.close();
                if (err)
                {
                    cout << "Error: file \"" << fileName.c_str() << "\" Intel-Hex format corrupt" << endl;
                    cout << "in line: " << s << endl;
                }
                
            }
            else
            {
                cout << "Error: file \"" << fileName.c_str() << "\" don't exist!" << endl;
            }
        }


    }
    else
    {
        cout << "Error: please call \"Crc2Hex.exe <filename.hex> [cfgfile] [quiet] [help]\"" << endl;
    }
    
    cout << "MaxAdr: " << iHex.MaxAddress() << endl;
    
    return result;

}

void writeHelp(void)
{
    // Ausgabe der Helpinformationen
    cout << endl;
    cout << "call \"Crc2Hex.exe <filename.hex> [cfgfile] [quiet] [help]\"" << endl << endl;
    cout << "[cfgfile] configuration file" << endl;
    cout << "[quiet] no output of additional information like checksum and program length" << endl;
    cout << "[help]  output of this help" << endl;
}
