
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      TriLogi_Convert/TriLogi_Convert.cpp

#include <KMS/Base.h>

// ===== C ==================================================================
#include <fcntl.h>

// ===== C++ ================================================================
#include <fstream>
#include <regex>

// ===== Windows ============================================================
#include <Windows.h>

// ===== Import/Includes ====================================================
#include <KMS/Banner.h>
#include <KMS/Exception.h>

// ===== Local ==============================================================
#include "../Common/Display.h"
#include "../Common/PLC/PLC.h"
#include "../Common/TRiLOGI/PC6.h"
#include "../Common/Version.h"

using namespace KMS;
using namespace TRiLOGI;

// Data type
// //////////////////////////////////////////////////////////////////////////

// --> INIT
//     |
//     +--> INPUT
//          |
//          +--> OUTPUT
//               |
//               +--> RELAY
//                    |
//                    +--> TIMER
//                         |
//                         +--> SEQUENCE
//                              |
//                              +--> CIRCUIT
//                                   |
//                                   +--> FUNCTION <--+
//                                        | |         |
//                                        | +--> FUNCTION_CODE --+
//                                        |                      |
//                                        +--> FUNCTION_LABEL <--+
//                                             |
//                                             +--> TAG
//                                                  |
//                                                  +--> DEFINE
//                                                       |
//                                                       +--> BREAKPOINT
//                                                            |
//                                                            +-> IP_ADDR
//                                                                |
//                                                                +--> END
enum class State
{
    INIT,
    INPUT,
    OUTPUT,
    RELAY,
    TIMER,
    SEQUENCE,
    CIRCUIT,
    FUNCTION,
    FUNCTION_CODE,
    FUNCTION_LABEL,
    TAG,
    DEFINE,
    BREAKPOINT,
    IP_ADDR,
    END,
};

// Constants
// //////////////////////////////////////////////////////////////////////////

static const char* END_OF_LINE = "\n";

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

static void Convert(const char* aInName);

static void Convert_Initial(const char* aInName);

static State Convert(const std::wstring& aLine, const wchar_t* aMark, State aCurrent, State aNext);

static State Convert_Index_Name      (const std::wstring& aLine, const wchar_t* aMark, State aCurrent, State aNext, std::ofstream& aMain, const char* aType);
static State Convert_Index_Name_Value(const std::wstring& aLine, const wchar_t* aMark, State aCurrent, State aNext, std::ofstream& aMain, const char* aType);

static State Convert_CIRCUIT(const std::wstring& aLine, std::wofstream& aCircuits);

static State Convert_DEFINE        (const std::wstring& aLine, std::ofstream& aMain);
static State Convert_FUNCTION      (const std::wstring& aLine, std::ofstream& aMain);
static State Convert_FUNCTION_CODE (const std::wstring& aLine, std::ofstream& aMain);
static State Convert_FUNCTION_LABEL(const std::wstring& aLine, std::ofstream& aMain);
static State Convert_INPUT         (const std::wstring& aLine, std::ofstream& aMain);
static State Convert_OUTPUT        (const std::wstring& aLine, std::ofstream& aMain);
static State Convert_RELAY         (const std::wstring& aLine, std::ofstream& aMain);
static State Convert_SEQUENCE      (const std::wstring& aLine, std::ofstream& aMain);
static State Convert_TIMER         (const std::wstring& aLine, std::ofstream& aMain);

static void DisplayUsage();

static void ToASCII(const std::wstring& aIn, char* aOut, unsigned int aOutSize_byte);

// Entry point
// //////////////////////////////////////////////////////////////////////////

int main(int aCount, const char** aVector)
{
    KMS_BANNER("KMS-PLC", "TriLogi_Convert");

    assert(nullptr != aVector);

    int lResult = 0;

    try
    {
        bool lInitial = false;

        switch (aCount)
        {
        case 3:
            assert(nullptr != aVector[2]);

            if (0 != _stricmp("Initial", aVector[2]))
            {
                Display_Error("Invalid commmand line", aVector[2]);
                DisplayUsage();
                lResult = __LINE__;
                break;
            }

            lInitial = true;

        case 2:
            assert(nullptr != aVector[1]);

            if (lInitial)
            {
                Convert_Initial(aVector[1]);
            }
            else
            {
                Convert(aVector[1]);
            }
            break;

        default:
            Display_Error("Invalid commmand line");
            DisplayUsage();
            lResult = __LINE__;
        }
    }
    KMS_CATCH_RESULT(lResult);

    return lResult;
}

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

void Convert(const char* aInName)
{
    assert(nullptr != aInName);

    // Open input file

    std::wifstream lIn(aInName, std::ios::binary);
    KMS_EXCEPTION_ASSERT(lIn.is_open(), RESULT_OPEN_FAILED, "Cannot open input file", aInName);

    ConfigStream(lIn);

    // Open output file

    std::wofstream lCircuits(PLC::CIRCUITS_TXT, std::ios::binary);
    KMS_EXCEPTION_ASSERT(lCircuits.is_open(), RESULT_OPEN_FAILED, "Cannot open output file (NOT_TESTED)", PLC::CIRCUITS_TXT);

    ConfigStream(lCircuits);

    // Process input file

    std::wstring lLine;

    State lState = State::INIT;

    while (getline(lIn, lLine))
    {
        switch (lState)
        {
        case State::INIT    : lState = Convert(lLine, PC6_HEADER, lState, State::INPUT   ); break;
        case State::INPUT   : lState = Convert(lLine, PC6_END_R , lState, State::OUTPUT  ); break;
        case State::OUTPUT  : lState = Convert(lLine, PC6_END_R , lState, State::RELAY   ); break;
        case State::RELAY   : lState = Convert(lLine, PC6_END_R , lState, State::TIMER   ); break;
        case State::TIMER   : lState = Convert(lLine, PC6_END_R , lState, State::SEQUENCE); break;
        case State::SEQUENCE: lState = Convert(lLine, PC6_END_R , lState, State::CIRCUIT ); break;

        case State::CIRCUIT: lState = Convert_CIRCUIT(lLine, lCircuits); break;

        case State::FUNCTION      : lState = Convert(lLine, PC6_END_FUNCTION_R      , lState, State::FUNCTION_LABEL); break;
        case State::FUNCTION_LABEL: lState = Convert(lLine, PC6_END_FUNCTION_LABEL_R, lState, State::TAG           ); break;
        case State::TAG           : lState = Convert(lLine, PC6_END_TAG_R           , lState, State::DEFINE        ); break;
        case State::DEFINE        : lState = Convert(lLine, PC6_END_DEFINE_R        , lState, State::BREAKPOINT    ); break;
        case State::BREAKPOINT    : lState = Convert(lLine, PC6_END_BREAKPOINT_R    , lState, State::IP_ADDR       ); break;
        case State::IP_ADDR       : lState = Convert(lLine, PC6_END_LAST_IP_ADDR_R  , lState, State::END           ); break;

        case State::END: break;

        default: assert(false);
        }
    }
}

void Convert_Initial(const char* aInName)
{
    assert(nullptr != aInName);

    // Open input file

    std::wifstream lIn(aInName, std::ios::binary);
    KMS_EXCEPTION_ASSERT(lIn.is_open(), RESULT_OPEN_FAILED, "Cannot open input file", aInName);

    ConfigStream(lIn);

    // Open output files

    std::wofstream lCircuits(PLC::CIRCUITS_TXT, std::ios::binary);
    std::ofstream  lMain    (PLC::MAIN_TXT);

    KMS_EXCEPTION_ASSERT(lCircuits.is_open(), RESULT_OPEN_FAILED, "Cannot open output file (NOT TESTED)", PLC::CIRCUITS_TXT);
    KMS_EXCEPTION_ASSERT(lMain    .is_open(), RESULT_OPEN_FAILED, "Cannot open output file (NOT TESTED)", PLC::MAIN_TXT);

    ConfigStream(lCircuits);

    lMain << END_OF_LINE;
    lMain << "# Converted by KMS-PLC - TriLogi_Convert.exe from " << aInName << END_OF_LINE;
    lMain << END_OF_LINE;

    // Process input file

    std::wstring lLine;

    State lState = State::INIT;

    while (getline(lIn, lLine))
    {
        switch (lState)
        {
        case State::INIT: lState = Convert(lLine, PC6_HEADER, State::INIT, State::INPUT); break;

        case State::INPUT   : lState = Convert_INPUT   (lLine, lMain); break;
        case State::OUTPUT  : lState = Convert_OUTPUT  (lLine, lMain); break;
        case State::RELAY   : lState = Convert_RELAY   (lLine, lMain); break;
        case State::TIMER   : lState = Convert_TIMER   (lLine, lMain); break;
        case State::SEQUENCE: lState = Convert_SEQUENCE(lLine, lMain); break;

        case State::CIRCUIT: lState = Convert_CIRCUIT(lLine, lCircuits); break;

        case State::FUNCTION      : lState = Convert_FUNCTION      (lLine, lMain); break;
        case State::FUNCTION_CODE : lState = Convert_FUNCTION_CODE (lLine, lMain); break;
        case State::FUNCTION_LABEL: lState = Convert_FUNCTION_LABEL(lLine, lMain); break;

        case State::TAG: lState = Convert(lLine, PC6_END_TAG_R, lState, State::DEFINE); break;

        case State::DEFINE: lState = Convert_DEFINE(lLine, lMain); break;

        case State::BREAKPOINT: lState = Convert(lLine, PC6_END_BREAKPOINT_R  , lState, State::IP_ADDR); break;
        case State::IP_ADDR   : lState = Convert(lLine, PC6_END_LAST_IP_ADDR_R, lState, State::END    ); break;

        case State::END: break;

        default: assert(false);
        }
    }
}

State Convert(const std::wstring& aLine, const wchar_t* aMark, State aCurrent, State aNext)
{
    assert(nullptr != aMark);
    assert(aCurrent != aNext);

    return (aMark == aLine) ? aNext : aCurrent;
}

State Convert_Index_Name(const std::wstring& aLine, const wchar_t* aMark, State aCurrent, State aNext, std::ofstream& aMain, const char* aType)
{
    static const std::wregex REGEX(L"^(\\d+),(\\w+)\r$");

    assert(nullptr != aMark);
    assert(aCurrent != aNext);
    assert(nullptr != aType);

    std::wsmatch lMatch;

    auto lResult = aCurrent;

    if (aMark == aLine)
    {
        aMain << END_OF_LINE;

        lResult = aNext;
    }
    else if (std::regex_match(aLine, lMatch, REGEX))
    {
        char lIndex[NAME_LENGTH];
        char lName [NAME_LENGTH];

        ToASCII(lMatch[1].str(), lIndex, sizeof(lIndex));
        ToASCII(lMatch[2].str(), lName , sizeof(lName ));

        aMain << aType << " " << lIndex << " " << lName << END_OF_LINE;
    }
    else
    {
        Display_Error("Invalid index or name (NOT TESTED)", aLine.c_str());

        KMS_EXCEPTION(RESULT_INVALID_FORMAT, "Invalid format (NOT TESTED)", aType);
    }

    return lResult;
}

State Convert_Index_Name_Value(const std::wstring& aLine, const wchar_t* aMark, State aCurrent, State aNext, std::ofstream& aMain, const char* aType)
{
    static const std::wregex REGEX(L"^(\\d+),(\\w+) (\\d+)\r$");

    assert(nullptr != aMark);
    assert(aCurrent != aNext);
    assert(nullptr != aType);

    std::wsmatch lMatch;

    auto lResult = aCurrent;

    if (aMark == aLine)
    {
        aMain << END_OF_LINE;

        lResult = aNext;
    }
    else if (std::regex_match(aLine, lMatch, REGEX))
    {
        char lIndex[NAME_LENGTH];
        char lName [NAME_LENGTH];
        char lValue[NAME_LENGTH];

        ToASCII(lMatch[1].str(), lIndex, sizeof(lIndex));
        ToASCII(lMatch[2].str(), lName , sizeof(lName ));
        ToASCII(lMatch[3].str(), lValue, sizeof(lValue));

        aMain << aType << " " << lIndex << " " << lName << " " << lValue << END_OF_LINE;
    }
    else
    {
        Display_Error("Invalid index, name or value (NOT TESTED)", aLine.c_str());

        KMS_EXCEPTION(RESULT_INVALID_FORMAT, "Invalid format (NOT TESTED)", aType);
    }

    return lResult;
}

State Convert_CIRCUIT(const std::wstring& aLine, std::wofstream& aCircuits)
{
    static const std::wregex REGEX(L"^(.+)\r$");

    std::wsmatch lMatch;

    auto lResult = State::CIRCUIT;

    if (std::regex_match(aLine, lMatch, REGEX))
    {
        aCircuits << lMatch[1].str() << L"\r\n";

        if (PC6_END_CIRCUIT == lMatch[1].str())
        {
            lResult = State::FUNCTION;
        }
    }
    else
    {
        Display_Warning("Ignored circuit line", aLine.c_str());
    }

    return lResult;
}

State Convert_DEFINE(const std::wstring& aLine, std::ofstream& aMain)
{
    static const std::wregex REGEX_0(L"^(\\d+),(\\w+),(\\d+),.*\r$");
    static const std::wregex REGEX_1(L"^(\\d+),(\\w+),(\\w+)\r$");
    static const std::wregex REGEX_2(L"^(\\d+),(\\w+),(&h[0-9A-Fa-f]+)\r$");
    static const std::wregex REGEX_3(L"^(\\d+),(\\w+),(&h[0-9A-Fa-f]+),.*\r$");
    static const std::wregex REGEX_4(L"^(\\d+),(\\w+),(DM\\[\\d+\\])\r$");
    static const std::wregex REGEX_5(L"^(\\d+),(\\w+),(DM\\[\\d+\\]),.*\r$");

    static const std::wregex sRegexA(L"^(\\d+),,\r$");

    std::wsmatch lMatch;

    auto lResult = State::DEFINE;

    if (PC6_END_DEFINE_R == aLine)
    {
        aMain << END_OF_LINE;

        lResult = State::BREAKPOINT;
    }
    else if (std::regex_match(aLine, lMatch, REGEX_0)
        ||   std::regex_match(aLine, lMatch, REGEX_1)
        ||   std::regex_match(aLine, lMatch, REGEX_2)
        ||   std::regex_match(aLine, lMatch, REGEX_3)
        ||   std::regex_match(aLine, lMatch, REGEX_4)
        ||   std::regex_match(aLine, lMatch, REGEX_5))
    {
        char lIndex[NAME_LENGTH];
        char lName [NAME_LENGTH];
        char lValue[NAME_LENGTH];

        ToASCII(lMatch[1].str(), lIndex, sizeof(lIndex));
        ToASCII(lMatch[2].str(), lName , sizeof(lName ));
        ToASCII(lMatch[3].str(), lValue, sizeof(lValue));

        aMain << "DEFINE " << lIndex << " " << lName << " " << lValue << END_OF_LINE;
    }
    else if (std::regex_match(aLine, lMatch, sRegexA))
    {
        // The define is empty, we just drop it.
    }
    else
    {
        Display_Error("Invalid define line (NOT TESTED)", aLine.c_str());

        KMS_EXCEPTION(RESULT_INVALID_FORMAT, "Invalid DEFINE format (NOT TESTED)", "");
    }

    return lResult;
}

State Convert_FUNCTION(const std::wstring& aLine, std::ofstream& aMain)
{
    static const std::wregex REGEX(L"^Fn#(\\d+),(\\d+)\r$");

    std::wsmatch lMatch;

    auto lResult = State::FUNCTION;

    if (PC6_END_FUNCTION_R == aLine)
    {
        lResult = State::FUNCTION_LABEL;
    }
    else if (PC6_BEGIN_FUNCTION_R == aLine)
    {
        // We simply ignore the begin mark.
    }
    else if (std::regex_match(aLine, lMatch, REGEX))
    {
        char lIndex[NAME_LENGTH];

        ToASCII(lMatch[1].str(), lIndex, sizeof(lIndex));

        aMain << "FUNCTION " << lIndex << END_OF_LINE;

        lResult = State::FUNCTION_CODE;
    }
    else
    {
        Display_Error("Invalid function line (NOT TESTED)", aLine.c_str());

        KMS_EXCEPTION(RESULT_INVALID_FORMAT, "Invalid FUNCTION format (NOT TESTED)", "");
    }

    return lResult;
}

State Convert_FUNCTION_CODE(const std::wstring& aLine, std::ofstream& aMain)
{
    auto lResult = State::FUNCTION_CODE;

    if (PC6_END_FUNCTION_R == aLine)
    {
        aMain << "FUNCTION_END" << END_OF_LINE;
        aMain << END_OF_LINE;

        lResult = State::FUNCTION_LABEL;
    }
    else if (PC6_BEGIN_FUNCTION_R == aLine)
    {
        aMain << "FUNCTION_END" << END_OF_LINE;
        aMain << END_OF_LINE;

        lResult = State::FUNCTION;
    }
    else
    {
        char lLine[LINE_LENGTH];

        ToASCII(aLine, lLine, sizeof(lLine));

        aMain << lLine << END_OF_LINE;
    }

    return lResult;
}

State Convert_FUNCTION_LABEL(const std::wstring& aLine, std::ofstream& aMain)
{
    return Convert_Index_Name(aLine, PC6_END_FUNCTION_LABEL_R, State::FUNCTION_LABEL, State::TAG, aMain, "FUNCTION_LABEL");
}

State Convert_INPUT(const std::wstring& aLine, std::ofstream& aMain)
{
    return Convert_Index_Name(aLine, PC6_END_R, State::INPUT, State::OUTPUT, aMain, "INPUT");
}

State Convert_OUTPUT(const std::wstring& aLine, std::ofstream& aMain)
{
    return Convert_Index_Name(aLine, PC6_END_R, State::OUTPUT, State::RELAY, aMain, "OUTPUT");
}

State Convert_RELAY(const std::wstring& aLine, std::ofstream& aMain)
{
    return Convert_Index_Name(aLine, PC6_END_R, State::RELAY, State::TIMER, aMain, "RELAY");
}

State Convert_SEQUENCE(const std::wstring& aLine, std::ofstream& aMain)
{
    return Convert_Index_Name_Value(aLine, PC6_END_R, State::SEQUENCE, State::CIRCUIT, aMain, "SEQUENCE");
}

State Convert_TIMER(const std::wstring& aLine, std::ofstream& aMain)
{
    return Convert_Index_Name_Value(aLine, PC6_END_R, State::TIMER, State::SEQUENCE, aMain, "TIMER");
}

void DisplayUsage()
{
    std::cout << "Usage: TriLogi_Convert.exe {TriLogi.PC6} [Initial]" << std::endl;
}

void ToASCII(const std::wstring& aIn, char* aOut, unsigned int aOutSize_byte)
{
    assert(nullptr != aOut);
    assert(0 < aOutSize_byte);
    
    auto lRet = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS | WC_NO_BEST_FIT_CHARS, aIn.c_str(), -1, aOut, aOutSize_byte, nullptr, nullptr);

    KMS_EXCEPTION_ASSERT(0 < lRet, RESULT_INVALID_FORMAT, "Invalid unicode string (NOT TESTED)", "");
}
