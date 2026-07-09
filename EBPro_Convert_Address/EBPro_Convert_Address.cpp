
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      EBPro_Convert_Address/EBPro_Convert_Address.cpp

#include <KMS/Base.h>

// ===== C++ ================================================================
#include <fstream>
#include <regex>

// ===== Import/Includes ====================================================
#include <KMS/Banner.h>
#include <KMS/Exception.h>

// ===== Local ==============================================================
#include "../Common/Display.h"
#include "../Common/HMI/HMI.h"
#include "../Common/Version.h"

using namespace KMS;

// Configurations
// //////////////////////////////////////////////////////////////////////////

static const char* END_OF_LINE = "\r\n";

static const char* EXPORTED_ADDRESS_CSV = "EBPro_Exported_Address.csv";

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

static void Convert_Addresses(const char* aInName);

static void Convert_LOCAL_EM0        (const std::smatch& aMatch, std::ofstream& aOut);
static void Convert_LOCAL_LB         (const std::smatch& aMatch, std::ofstream& aOut);
static void Convert_LOCAL_LW         (const std::smatch& aMatch, std::ofstream& aOut);
static void Convert_LOCAL_LW_BIT     (const std::smatch& aMatch, std::ofstream& aOut);
static void Convert_LOCAL_LW_UINT16  (const std::smatch& aMatch, std::ofstream& aOut);
static void Convert_LOCAL_RW         (const std::smatch& aMatch, std::ofstream& aOut);
static void Convert_LOCAL_RW_A       (const std::smatch& aMatch, std::ofstream& aOut);
static void Convert_LOCAL_RW_A_BIT   (const std::smatch& aMatch, std::ofstream& aOut);
static void Convert_LOCAL_RW_A_UINT16(const std::smatch& aMatch, std::ofstream& aOut);
static void Convert_MODBUS_1X        (const std::smatch& aMatch, std::ofstream& aOut);
static void Convert_MODBUS_4X        (const std::smatch& aMatch, std::ofstream& aOut);
static void Convert_MODBUS_4X_BIT    (const std::smatch& aMatch, std::ofstream& aOut);
static void Convert_MODBUS_4X_INT16  (const std::smatch& aMatch, std::ofstream& aOut);
static void Convert_MODBUS_4X_UINT16 (const std::smatch& aMatch, std::ofstream& aOut);

static void DisplayUsage();

// Entry point
// //////////////////////////////////////////////////////////////////////////

int main(int aCount, const char** aVector)
{
    KMS_BANNER("KMS-PLC", "EBPro_Convert_Address");

    assert(nullptr != aVector);

    int lResult = 0;

    try
    {
        std::string lInputFileName(EXPORTED_ADDRESS_CSV);

        switch (aCount)
        {
        case 2:
            assert(nullptr != aVector[1]);

            lInputFileName = aVector[1]; break;
            // no break

        case 1:
            Convert_Addresses(lInputFileName.c_str());
            break;

        default:
            Display_Error("Invalid commmand line");
            DisplayUsage();
            lResult = __LINE__;
        }
    }
    KMS_CATCH_RESULT(lResult);

    return 0;
}

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

void Convert_Addresses(const char* aInName)
{
    static const std::regex REGEX_LOCAL_EM0        ("^(\\w[\\w -]*),Local HMI,EM0,(\\d+),(?:.*),Undesignated\r$");
    static const std::regex REGEX_LOCAL_LB         ("^(\\w[\\w -]*),Local HMI,LB,(\\d+),(?:.*),Undesignated\r$");
    static const std::regex REGEX_LOCAL_LW         ("^(\\w[\\w -]*),Local HMI,LW,(\\d+),(?:.*),Undesignated\r$");
    static const std::regex REGEX_LOCAL_LW_BIT     ("^(\\w[\\w -]*),Local HMI,LW_Bit,\"(\\d+),(\\d+)\",(?:.*),Undesignated\r$");
    static const std::regex REGEX_LOCAL_LW_UINT16  ("^(\\w[\\w -]*),Local HMI,LW,(\\d+),(?:.*),16-bit Unsigned\r$");
    static const std::regex REGEX_LOCAL_RW         ("^(\\w[\\w -]*),Local HMI,RW,(\\d+),(?:.*),Undesignated\r$");
    static const std::regex REGEX_LOCAL_RW_A       ("^(\\w[\\w -]*),Local HMI,RW_A,(\\d+),(?:.*),Undesignated\r$");
    static const std::regex REGEX_LOCAL_RW_A_BIT   ("^(\\w[\\w -]*),Local HMI,RW_A_Bit,(\\d+\\.[0-9A-F]+),(?:.*),Undesignated\r$");
    static const std::regex REGEX_LOCAL_RW_A_UINT16("^(\\w[\\w -]*),Local HMI,RW_A,(\\d+),(?:.*),16-bit Unsigned\r$");
    static const std::regex REGEX_MODBUS_1X        ("^(\\w[\\w -]*),MODBUS RTU,1x,(\\d+),(?:.*),Undesignated\r$");
    static const std::regex REGEX_MODBUS_4X        ("^(\\w[\\w -]*),MODBUS RTU,4x,(\\d+),(?:.*),Undesignated\r$");
    static const std::regex REGEX_MODBUS_4X_BIT    ("^(\\w[\\w -]*),MODBUS RTU,4x_Bit,(\\d+\\.\\d+),(?:.*),Undesignated\r$");
    static const std::regex REGEX_MODBUS_4X_INT16  ("^(\\w[\\w -]*),MODBUS RTU,4x,(\\d+),(?:.*),16-bit Signed\r$");
    static const std::regex REGEX_MODBUS_4X_UINT16 ("^(\\w[\\w -]*),MODBUS RTU,4x,(\\d+),(?:.*),16-bit Unsigned\r$");

    assert(nullptr != aInName);

    // Open input file

    std::ifstream lIn(aInName, std::ios::binary);
    KMS_EXCEPTION_ASSERT(lIn.is_open(), RESULT_OPEN_FAILED, "Cannot open input file", aInName);

    // Open output file

    std::ofstream lOut(HMI::ADDRESSES_TXT, std::ios::binary);
    KMS_EXCEPTION_ASSERT(lOut.is_open(), RESULT_OPEN_FAILED, "Cannot open output file (NOT_TESTED)", HMI::ADDRESSES_TXT);

    lOut << END_OF_LINE;
    lOut << "# Converted by KMS-PLC - EBPro_Convert.exe from " << aInName << END_OF_LINE;
    lOut << END_OF_LINE;

    // Process input file

    std::string lLine;

    while (getline(lIn, lLine))
    {
        std::smatch lMatch;

        if      (std::regex_match(lLine, lMatch, REGEX_LOCAL_EM0        )) { Convert_LOCAL_EM0        (lMatch, lOut); }
        else if (std::regex_match(lLine, lMatch, REGEX_LOCAL_LB         )) { Convert_LOCAL_LB         (lMatch, lOut); }
        else if (std::regex_match(lLine, lMatch, REGEX_LOCAL_LW         )) { Convert_LOCAL_LW         (lMatch, lOut); }
        else if (std::regex_match(lLine, lMatch, REGEX_LOCAL_LW_BIT     )) { Convert_LOCAL_LW_BIT     (lMatch, lOut); }
        else if (std::regex_match(lLine, lMatch, REGEX_LOCAL_LW_UINT16  )) { Convert_LOCAL_LW_UINT16  (lMatch, lOut); }
        else if (std::regex_match(lLine, lMatch, REGEX_LOCAL_RW         )) { Convert_LOCAL_RW         (lMatch, lOut); }
        else if (std::regex_match(lLine, lMatch, REGEX_LOCAL_RW_A       )) { Convert_LOCAL_RW_A       (lMatch, lOut); }
        else if (std::regex_match(lLine, lMatch, REGEX_LOCAL_RW_A_BIT   )) { Convert_LOCAL_RW_A_BIT   (lMatch, lOut); }
        else if (std::regex_match(lLine, lMatch, REGEX_LOCAL_RW_A_UINT16)) { Convert_LOCAL_RW_A_UINT16(lMatch, lOut); }
        else if (std::regex_match(lLine, lMatch, REGEX_MODBUS_1X        )) { Convert_MODBUS_1X        (lMatch, lOut); }
        else if (std::regex_match(lLine, lMatch, REGEX_MODBUS_4X        )) { Convert_MODBUS_4X        (lMatch, lOut); }
        else if (std::regex_match(lLine, lMatch, REGEX_MODBUS_4X_BIT    )) { Convert_MODBUS_4X_BIT    (lMatch, lOut); }
        else if (std::regex_match(lLine, lMatch, REGEX_MODBUS_4X_INT16  )) { Convert_MODBUS_4X_INT16  (lMatch, lOut); }
        else if (std::regex_match(lLine, lMatch, REGEX_MODBUS_4X_UINT16 )) { Convert_MODBUS_4X_UINT16 (lMatch, lOut); }
        else
        {
            Display_Error("Invalid input file line", lLine.c_str());
            KMS_EXCEPTION(RESULT_INVALID_FORMAT, "Invalid input file line", lLine.c_str());
        }
    }
}

void Convert_LOCAL_EM0(const std::smatch& aMatch, std::ofstream& aOut)
{
    // WORD {Name} LOCAL EM0 {Addr} [Type]
    aOut << "WORD " << aMatch[1].str() << " LOCAL EM0 " << aMatch[2].str() << END_OF_LINE;
}

void Convert_LOCAL_LB(const std::smatch& aMatch, std::ofstream& aOut)
{
    // BIT {Name} LOCAL LB {Addr}
    aOut << "BIT " << aMatch[1].str() << " LOCAL LB " << aMatch[2].str() << END_OF_LINE;
}

void Convert_LOCAL_LW(const std::smatch& aMatch, std::ofstream& aOut)
{
    // WORD {Name} LOCAL LW {Addr} [Type]
    aOut << "WORD " << aMatch[1].str() << " LOCAL LW " << aMatch[2].str() << END_OF_LINE;
}

void Convert_LOCAL_LW_BIT(const std::smatch& aMatch, std::ofstream& aOut)
{
    // BIT {Name} LOCAL LW {Addr.Bit}
    aOut << "BIT " << aMatch[1].str() << " LOCAL LW " << aMatch[2].str() << "." << aMatch[3].str() << END_OF_LINE;
}

void Convert_LOCAL_LW_UINT16(const std::smatch& aMatch, std::ofstream& aOut)
{
    // WORD {Name} LOCAL LW {Addr} [Type]
    aOut << "WORD " << aMatch[1].str() << " LOCAL LW " << aMatch[2].str() << " uint16_t" << END_OF_LINE;
}

void Convert_LOCAL_RW(const std::smatch& aMatch, std::ofstream& aOut)
{
    // WORD {Name} LOCAL RW {Addr} [Type]
    aOut << "WORD " << aMatch[1].str() << " LOCAL RW " << aMatch[2].str() << END_OF_LINE;
}

void Convert_LOCAL_RW_A(const std::smatch& aMatch, std::ofstream& aOut)
{
    // WORD {Name} LOCAL RW_A {Addr} [Type]
    aOut << "WORD " << aMatch[1].str() << " LOCAL RW_A " << aMatch[2].str() << END_OF_LINE;
}

void Convert_LOCAL_RW_A_BIT(const std::smatch& aMatch, std::ofstream& aOut)
{
    // BIT {Name} LOCAL RW_A {Addr.Bit}
    aOut << "BIT " << aMatch[1].str() << " LOCAL RW_A " << aMatch[2].str() << END_OF_LINE;
}

void Convert_LOCAL_RW_A_UINT16(const std::smatch& aMatch, std::ofstream& aOut)
{
    // WORD {Name} LOCAL RW_A {Addr} [Type]
    aOut << "WORD " << aMatch[1].str() << " LOCAL RW_A " << aMatch[2].str() << " uint16_t" << END_OF_LINE;
}

void Convert_MODBUS_1X(const std::smatch& aMatch, std::ofstream& aOut)
{
    // BIT {Name} MODBUS 1x {Addr}
    aOut << "BIT " << aMatch[1].str() << " MODBUS 1x " << aMatch[2].str() << END_OF_LINE;
}

void Convert_MODBUS_4X(const std::smatch& aMatch, std::ofstream& aOut)
{
    // WORD {Name} MODBUS 4x {Addr} [Type]
    aOut << "WORD " << aMatch[1].str() << " MODBUS 4x " << aMatch[2].str() << END_OF_LINE;
}

void Convert_MODBUS_4X_BIT(const std::smatch& aMatch, std::ofstream& aOut)
{
    // BIT {Name} MODBUS 4x {Addr.Bit}
    aOut << "BIT " << aMatch[1].str() << " MODBUS 4x " << aMatch[2].str() << END_OF_LINE;
}

void Convert_MODBUS_4X_INT16(const std::smatch& aMatch, std::ofstream& aOut)
{
    // WORD {Name} MODBUS 4x {Addr} [Type]
    aOut << "WORD " << aMatch[1].str() << " MODBUS 4x " << aMatch[2].str() << " int16_t" << END_OF_LINE;
}

void Convert_MODBUS_4X_UINT16(const std::smatch& aMatch, std::ofstream& aOut)
{
    // WORD {Name} MODBUS 4x {Addr} [Type]
    aOut << "WORD " << aMatch[1].str() << " MODBUS 4x " << aMatch[2].str() << " uint16_t" << END_OF_LINE;
}

static void DisplayUsage()
{
    std::cout << "Usage: EBPro_Convert_Address.exe < EBPro_Exported.csv" << std::endl;
}
