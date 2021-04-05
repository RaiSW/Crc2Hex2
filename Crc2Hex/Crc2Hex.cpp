#include <iostream>
#include <string>
#include <fstream>
#include "lib.h"
#include "IntelHex.h"
#include "memgap.h"
#include "CRC16.h"
#include "AddChecksum.h"

using namespace std;

#define PRG_VERSION     2
#define SUB_VERSION     1
#define SUB_SUB_VERSION 0

#define LIC_1 "Bosch_Rexroth_AG_Hannover_Marine"
#define LIC_2 "Bosch_Rexroth_AG_Hannover_Mar_02"

#define FLASH_SIZE           0x100000
#define OFFS_FLASH_START     56
#define OFFS_FLASH_END       60
#define OFFS_FLASH_GAP_START 64
#define OFFS_FLASH_GAP_END   68
#define OFFS_ADD32           72
#define OFFS_CRC16           76

struct sIntHexData {
    int32_t  dataAdr     = 0;  // Address of data structure in memory
    uint16_t uiCRC16     = 0;  // CRC16 checksum of HEX-File
    uint32_t ulAdd32     = 0;  // Add32 checksum of HEX-File
    int16_t noOfLicenses = 0;
    string licenses[3]{};      // License list
    int16_t foundLicNo   = -1; // founded license string
    MemGap memList;            // GAP list
    char* mem { NULL };
    int32_t memSize = 0;
    int32_t maxFlashAdr = 0;
};


void writeHelp(void);
bool findLicense(sIntHexData &data);

int main(int argc, char** argv)
{
    int result = 1;
    bool bQuiet = false;
    bool bHelp = false;
    string fileName = "";
    string cfgFile = "";
    string s;
    IntelHex iHex;
    sIntHexData data;
    CRC16 crc16;
    AddChecksum chksum16;
    
    data.noOfLicenses = 2;
    data.licenses[0] = LIC_1;
    data.licenses[1] = LIC_2;
    data.memSize = FLASH_SIZE;

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
            /* read Intel-Hex file */
            ifstream ifile;
            ifile.open(fileName);
            if (ifile)
            {
                data.mem = new char[data.memSize];
                memset(data.mem, 0xff, data.memSize);
                int err = 0;
                int line = 0;
                while (getline(ifile, s))
                {
                    err = iHex.writeToMem(data.mem, data.memSize, s);
                    line++;
                    if (err != INTHEX_OK)
                    {
                        cout << "Error: corrupt Intel-Hex-File. Error result " << err << endl;
                        cout << "in line: " << s << endl;
                        break;
                    }
                }
                ifile.close();
                if (err == INTHEX_OK)
                {
                    data.maxFlashAdr = iHex.MaxAddress();
                    data.dataAdr = 0;
                    // search for licence
                    if (findLicense(data))
                    {
                        // generateAdrList
                        data.memList.Init(*(uint32_t*)(data.mem + data.dataAdr + OFFS_FLASH_START),
                                          *(uint32_t*)(data.mem + data.dataAdr + OFFS_FLASH_END));
                        data.memList.Add(*(uint32_t*)(data.mem + data.dataAdr + OFFS_FLASH_GAP_START),
                                         *(uint32_t*)(data.mem + data.dataAdr + OFFS_FLASH_GAP_END));
                        data.memList.Add(data.dataAdr + OFFS_ADD32, data.dataAdr + OFFS_ADD32 + 5);
                        
                        // Calculate checksum over all elements of address list
                        for (uint16_t i = 0; i < data.memList.Size(); i++)
                        {
                            // CRC16-Summe bilden
                            data.uiCRC16 = crc16.Add((uint8_t *)(data.mem + data.memList.Start(i)),
                                (uint8_t*)(data.mem + data.memList.End(i)));
                            // ADD16-Summe bilden
                            data.ulAdd32 = chksum16.Add((uint16_t*)(data.mem + data.memList.Start(i)),
                                (uint16_t*)(data.mem + data.memList.End(i)));
                        }

                        cout << "calculated checksums" << endl;
                        cout << "  CRC16:  0x" << hex << uppercase << data.uiCRC16 << endl;
                        cout << "  ADD32:  0x" << hex << uppercase << data.ulAdd32 << endl;
                        cout << "  MaxAdr: " << dec << iHex.MaxAddress() << endl;
                    }
                    else
                    {
                        cout << "Error: Licence key in source file is missing!" << endl;
                    }
                }
                
                delete[] data.mem; // release memory
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

bool findLicense(sIntHexData &data)
{
    bool bFound = false;
    int32_t licenseNo = data.noOfLicenses-1;
    int32_t len;
    int32_t p;
    while (!bFound && licenseNo >= 0)
    {
        len = (int32_t)data.licenses[licenseNo].length();
        for (p = 0; p < (data.maxFlashAdr - len); p++)
        {
            if (!memcmp(data.mem + p, (char*)data.licenses[licenseNo].c_str(), len))
            {
                bFound = true;
                break;
            }
        }
        licenseNo--;
    }
    if (bFound)
    {
        data.dataAdr = p;
        data.foundLicNo = licenseNo + 1;
    }
    else
        data.foundLicNo = -1;

    return bFound;
}
