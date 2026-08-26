
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      EBPro_Build_Address/EBPro_Builder.cpp

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KMS/Convert.h>

// ===== Local ==============================================================
#include "../Common/Display.h"
#include "../Common/HMI/HMI.h"
#include "../Common/Parser.h"
#include "../Common/Text.h"

#include "Builder_Address.h"

using namespace KMS;

// Configurations
// //////////////////////////////////////////////////////////////////////////

const char* EBPRO_ADDRESSES_TO_IMPORT_CSV = "EBPro_Addresses_ToImport.csv";

// Constants
// //////////////////////////////////////////////////////////////////////////

static constexpr unsigned int MONITOR_BIT_LOCAL_LB    = 0;
static constexpr unsigned int MONITOR_BIT_MODBUS_1X   = 1;
static constexpr unsigned int MONITOR_WORD_LOCAL_LW   = 2;
static constexpr unsigned int MONITOR_WORD_LOCAL_RW   = 3;
static constexpr unsigned int MONITOR_WORD_LOCAL_RW_A = 4;
static constexpr unsigned int MONITOR_WORD_MODBUS_4X  = 5;

// Public
// //////////////////////////////////////////////////////////////////////////

Builder_Address::Builder_Address() : mOut(nullptr)
{
    AddSource(HMI::ADDRESSES_TXT);

    mIndexMonitors[MONITOR_BIT_LOCAL_LB   ].Init("BIT Local HMI LB"   , 0,   9998);
    mIndexMonitors[MONITOR_BIT_MODBUS_1X  ].Init("BIT Modbus 1x"      , 1,   1536);
    mIndexMonitors[MONITOR_WORD_LOCAL_LW  ].Init("WORD Local HMI LW"  , 0,   9999);
    mIndexMonitors[MONITOR_WORD_LOCAL_RW  ].Init("WORD Local HMI RW"  , 0, 524287);
    mIndexMonitors[MONITOR_WORD_LOCAL_RW_A].Init("WORD Local HMI RW_A", 0,  65535);
    mIndexMonitors[MONITOR_WORD_MODBUS_4X ].Init("WORD Modbus 4x"     , 1,   5000);
}

// ===== Builder ============================================================

Builder_Address::~Builder_Address()
{
    if (nullptr != mOut)
    {
        delete mOut;
    }
}

void Builder_Address::Build()
{
    mOut = new std::ofstream(EBPRO_ADDRESSES_TO_IMPORT_CSV, std::ios::binary);
    KMS_EXCEPTION_ASSERT(mOut->is_open(), RESULT_OPEN_FAILED, "Cannot open output file", EBPRO_ADDRESSES_TO_IMPORT_CSV);

    Builder::Build();
}

// Protected
// //////////////////////////////////////////////////////////////////////////

void Builder_Address::ReadSource(const char* aSource)
{
    char        lLine[LINE_LENGTH];
    std::smatch lMatch;
    Parser      lParser(aSource);

    while (lParser.GetNextLine(lLine, sizeof(lLine)))
    {
        static const std::regex REGEX_BIT_LOCAL_LB          ("^BIT (\\w[\\w -]*) LOCAL LB (\\d+)$");
        static const std::regex REGEX_BIT_LOCAL_LW          ("^BIT (\\w[\\w -]*) LOCAL LW (\\d+\\.\\d+)$");
        static const std::regex REGEX_BIT_LOCAL_RW_A        ("^BIT (\\w[\\w -]*) LOCAL RW_A (\\d+\\.[0-9A-F]+)$");
        static const std::regex REGEX_BIT_MODBUS_1X         ("^BIT (\\w[\\w -]*) MODBUS 1x (\\d+)$");
        static const std::regex REGEX_BIT_MODBUS_4X         ("^BIT (\\w[\\w -]*) MODBUS 4x (\\d+\\.\\d+)$");
        static const std::regex REGEX_WORD_LOCAL_EM0        ("^WORD (\\w[\\w -]*) LOCAL EM0 (\\d+)$");
        static const std::regex REGEX_WORD_LOCAL_LW         ("^WORD (\\w[\\w -]*) LOCAL LW (\\d+)$");
        static const std::regex REGEX_WORD_LOCAL_LW_UINT16  ("^WORD (\\w[\\w -]*) LOCAL LW (\\d+) uint16_t$");
        static const std::regex REGEX_WORD_LOCAL_RW         ("^WORD (\\w[\\w -]*) LOCAL RW (\\d+)$");
        static const std::regex REGEX_WORD_LOCAL_RW_A       ("^WORD (\\w[\\w -]*) LOCAL RW_A (\\d+)$");
        static const std::regex REGEX_WORD_LOCAL_RW_A_UINT16("^WORD (\\w[\\w -]*) LOCAL RW_A (\\d+) uint16_t$");
        static const std::regex REGEX_WORD_MODBUS_4X        ("^WORD (\\w[\\w -]*) MODBUS 4x (\\d+)$");
        static const std::regex REGEX_WORD_MODBUS_4X_INT16  ("^WORD (\\w[\\w -]*) MODBUS 4x (\\d+) int16_t$");
        static const std::regex REGEX_WORD_MODBUS_4X_UINT16 ("^WORD (\\w[\\w -]*) MODBUS 4x (\\d+) uint16_t$");

        std::string lLineStr(lLine);

        if      (std::regex_match(lLineStr, lMatch, REGEX_BIT_LOCAL_LB          )) { Write_BIT_LOCAL_LB          (lMatch); }
        else if (std::regex_match(lLineStr, lMatch, REGEX_BIT_LOCAL_LW          )) { Write_BIT_LOCAL_LW          (lMatch); }
        else if (std::regex_match(lLineStr, lMatch, REGEX_BIT_LOCAL_RW_A        )) { Write_BIT_LOCAL_RW_A        (lMatch); }
        else if (std::regex_match(lLineStr, lMatch, REGEX_BIT_MODBUS_1X         )) { Write_BIT_MODBUS_1X         (lMatch); }
        else if (std::regex_match(lLineStr, lMatch, REGEX_BIT_MODBUS_4X         )) { Write_BIT_MODBUS_4X         (lMatch); }
        else if (std::regex_match(lLineStr, lMatch, REGEX_WORD_LOCAL_EM0        )) { Write_WORD_LOCAL_EM0        (lMatch); }
        else if (std::regex_match(lLineStr, lMatch, REGEX_WORD_LOCAL_LW         )) { Write_WORD_LOCAL_LW         (lMatch); }
        else if (std::regex_match(lLineStr, lMatch, REGEX_WORD_LOCAL_LW_UINT16  )) { Write_WORD_LOCAL_LW_UINT16  (lMatch); }
        else if (std::regex_match(lLineStr, lMatch, REGEX_WORD_LOCAL_RW         )) { Write_WORD_LOCAL_RW         (lMatch); }
        else if (std::regex_match(lLineStr, lMatch, REGEX_WORD_LOCAL_RW_A       )) { Write_WORD_LOCAL_RW_A       (lMatch); }
        else if (std::regex_match(lLineStr, lMatch, REGEX_WORD_LOCAL_RW_A_UINT16)) { Write_WORD_LOCAL_RW_A_UINT16(lMatch); }
        else if (std::regex_match(lLineStr, lMatch, REGEX_WORD_MODBUS_4X        )) { Write_WORD_MODBUS_4X        (lMatch); }
        else if (std::regex_match(lLineStr, lMatch, REGEX_WORD_MODBUS_4X_INT16  )) { Write_WORD_MODBUS_4X_INT16  (lMatch); }
        else if (std::regex_match(lLineStr, lMatch, REGEX_WORD_MODBUS_4X_UINT16 )) { Write_WORD_MODBUS_4X_UINT16 (lMatch); }
        else
        {
            Display_Error("Invalid input line", lLine);
            KMS_EXCEPTION(RESULT_INVALID_FORMAT, "Invalid input line", lLine);
        }
    }
}

// Private
// //////////////////////////////////////////////////////////////////////////

#define BEGIN (*mOut) << aMatch[1].str() << "," <<

void Builder_Address::Write_BIT_LOCAL_LB(const std::smatch& aMatch)
{
    mIndexMonitors[MONITOR_BIT_LOCAL_LB].MarkUsed(Convert::ToUInt16(aMatch[2].str().c_str()));

    BEGIN "Local HMI,LB," << aMatch[2].str() << ",,Undesignated" << Text_EOL;
}

void Builder_Address::Write_BIT_LOCAL_LW(const std::smatch& aMatch)
{
    BEGIN "Local HMI,LW_Bit," << aMatch[2].str() << ",,Undesignated" << Text_EOL;
}

void Builder_Address::Write_BIT_LOCAL_RW_A(const std::smatch& aMatch)
{
    BEGIN "Local HMI,RW_A_Bit," << aMatch[2].str() << ",,Undesignated" << Text_EOL;
}

void Builder_Address::Write_BIT_MODBUS_1X(const std::smatch& aMatch)
{
    mIndexMonitors[MONITOR_BIT_MODBUS_1X].MarkUsed(Convert::ToUInt16(aMatch[2].str().c_str()));

    BEGIN "MODBUS RTU,1x," << aMatch[2].str() << ",,Undesignated" << Text_EOL;
}

void Builder_Address::Write_BIT_MODBUS_4X(const std::smatch& aMatch)
{
    BEGIN "MODBUS RTU,4x_Bit," << aMatch[2].str() << ",,Undesignated" << Text_EOL;
}

void Builder_Address::Write_WORD_LOCAL_EM0(const std::smatch& aMatch)
{
    BEGIN "Local HMI,EM0," << aMatch[2].str() << ",,Undesignated" << Text_EOL;
}

void Builder_Address::Write_WORD_LOCAL_LW(const std::smatch& aMatch)
{
    mIndexMonitors[MONITOR_WORD_LOCAL_LW].MarkUsed(Convert::ToUInt16(aMatch[2].str().c_str()));

    BEGIN "Local HMI,LW," << aMatch[2].str() << ",,Undesignated" << Text_EOL;
}

void Builder_Address::Write_WORD_LOCAL_LW_UINT16(const std::smatch& aMatch)
{
    mIndexMonitors[MONITOR_WORD_LOCAL_LW].MarkUsed(Convert::ToUInt16(aMatch[2].str().c_str()));

    BEGIN "Local HMI,LW," << aMatch[2].str() << ",,16-bit Unsigned" << Text_EOL;
}

void Builder_Address::Write_WORD_LOCAL_RW(const std::smatch& aMatch)
{
    mIndexMonitors[MONITOR_WORD_LOCAL_RW].MarkUsed(Convert::ToUInt32(aMatch[2].str().c_str()));

    BEGIN "Local HMI,RW," << aMatch[2].str() << ",,Undesignated" << Text_EOL;
}

void Builder_Address::Write_WORD_LOCAL_RW_A(const std::smatch& aMatch)
{
    mIndexMonitors[MONITOR_WORD_LOCAL_RW_A].MarkUsed(Convert::ToUInt16(aMatch[2].str().c_str()));

    BEGIN "Local HMI,RW_A," << aMatch[2].str() << ",,Undesignated" << Text_EOL;
}

void Builder_Address::Write_WORD_LOCAL_RW_A_UINT16(const std::smatch& aMatch)
{
    mIndexMonitors[MONITOR_WORD_LOCAL_RW_A].MarkUsed(Convert::ToUInt16(aMatch[2].str().c_str()));

    BEGIN "Local HMI,RW_A," << aMatch[2].str() << ",,16-but Unsigned" << Text_EOL;
}

void Builder_Address::Write_WORD_MODBUS_4X(const std::smatch& aMatch)
{
    mIndexMonitors[MONITOR_WORD_MODBUS_4X].MarkUsed(Convert::ToUInt16(aMatch[2].str().c_str()));

    BEGIN "MODBUS RTU,4x," << aMatch[2].str() << ",,Undesignated" << Text_EOL;
}

void Builder_Address::Write_WORD_MODBUS_4X_INT16(const std::smatch& aMatch)
{
    mIndexMonitors[MONITOR_WORD_MODBUS_4X].MarkUsed(Convert::ToUInt16(aMatch[2].str().c_str()));

    BEGIN "MODBUS RTU,4x," << aMatch[2].str() << ",,16-bit Signed" << Text_EOL;
}

void Builder_Address::Write_WORD_MODBUS_4X_UINT16(const std::smatch& aMatch)
{
    mIndexMonitors[MONITOR_WORD_MODBUS_4X].MarkUsed(Convert::ToUInt16(aMatch[2].str().c_str()));

    BEGIN "MODBUS RTU,4x," << aMatch[2].str() << ",,16-bit Unsigned" << Text_EOL;
}
