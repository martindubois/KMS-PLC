
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      TriLogi_Build/Builder.cpp

#include "Component.h"

// ===== C++ ================================================================
#include <fstream>

// ===== Local ==============================================================
#include "../Common/PLC/PLC.h"
#include "../Common/TRiLOGI/PC6.h"

#include "Builder.h"

using namespace KMS;

using namespace TRiLOGI;

// Constants
// //////////////////////////////////////////////////////////////////////////

static const wchar_t* END_OF_LINE = L"\r\n";

static const char* TO_COMPILE_PC6 = "PLC_ToCompile.PC6";

// Public
// //////////////////////////////////////////////////////////////////////////

Builder::Builder() {}

void Builder::Write()
{
    wchar_t lLine [LINE_LENGTH];
    wchar_t lName [NAME_LENGTH];
    wchar_t lValue[NAME_LENGTH];

    // Open input file

    std::wifstream lCircuits(PLC::CIRCUITS_TXT, std::ios::binary);
    KMS_EXCEPTION_ASSERT(lCircuits.is_open(), RESULT_OPEN_FAILED, "Cannot open input file (NOT TESTED)", PLC::CIRCUITS_TXT)

    ConfigStream(lCircuits);

    // Open output file

    std::wofstream lOut(TO_COMPILE_PC6, std::ios::binary);
    KMS_EXCEPTION_ASSERT(lOut.is_open(), RESULT_OPEN_FAILED, "Cannot open output file (NOT_TESTED)", TO_COMPILE_PC6);

    ConfigStream(lOut);

    lOut << PC6_HEADER << "\n";

    // Write output file
    
    // Input
    for (const auto& lI : mInputs)
    {
        lI.second.GetName(lName, sizeof(lName));

        lOut << lI.second.GetIndex() << "," << lName << END_OF_LINE;
    }
    lOut << PC6_END << END_OF_LINE;

    // Output
    for (const auto& lO : mOutputs)
    {
        lO.second.GetName(lName, sizeof(lName));

        lOut << lO.second.GetIndex() << "," << lName << END_OF_LINE;
    }
    lOut << PC6_END << END_OF_LINE;

    // Relay
    for (const auto& lR : mRelays)
    {
        lR.second.GetName(lName, sizeof(lName));

        lOut << lR.second.GetIndex() << "," << lName << END_OF_LINE;
    }
    lOut << PC6_END << END_OF_LINE;

    // Timer
    for (const auto& lT : mTimers)
    {
        lT.second.GetName(lName, sizeof(lName));

        lOut << lT.second.GetIndex() << "," << lName << " " << lT.second.GetValue() << END_OF_LINE;
    }
    lOut << PC6_END << END_OF_LINE;

    // Sequence
    for (const auto& lS : mSequences)
    {
        lS.second.GetName(lName, sizeof(lName));

        lOut << lS.second.GetIndex() << "," << lName << " " << lS.second.GetValue() << END_OF_LINE;
    }
    lOut << PC6_END << END_OF_LINE;

    // Circuit
    std::wstring lC;

    while (getline(lCircuits, lC))
    {
        lOut << lC << L"\n";
    }

    // Function
    for (const auto& lF : mFunctions)
    {
        if (!lF.second->IsNameEmpty())
        {
            lOut << PC6_BEGIN_FUNCTION << END_OF_LINE;
            lOut << "Fn#" << lF.second->GetIndex() << "," << lF.second->GetSize() << END_OF_LINE;

            for (auto& lL : lF.second->mLines)
            {
                auto lRet = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, lL.c_str(), -1, lLine, sizeof(lLine) / sizeof(lLine[0]));
                assert(0 < lRet);

                lOut << lLine << "\n";
            }
        }
    }
    lOut << PC6_END_FUNCTION << END_OF_LINE;

    // Function label
    for (const auto& lF : mFunctions)
    {
        if (!lF.second->IsNameEmpty())
        {
            lF.second->GetName(lName, sizeof(lName));

            lOut << lF.second->GetIndex() << "," << lName << END_OF_LINE;
        }
    }
    lOut << PC6_END_FUNCTION_LABEL << END_OF_LINE;

    // Tag
    lOut << PC6_END_TAG << END_OF_LINE;

    // Define
    for (const auto& lD : mDefines)
    {
        lD.second.GetName (lName , sizeof(lName ));
        lD.second.GetValue(lValue, sizeof(lValue));

        lOut << lD.second.GetIndex() << "," << lName << "," << lValue << END_OF_LINE;
    }
    lOut << PC6_END_DEFINE << END_OF_LINE;

    // Breakpoint
    lOut << PC6_END_BREAKPOINT << END_OF_LINE;

    // Last IP Addr
    lOut << PC6_END_LAST_IP_ADDR << END_OF_LINE;
}

// ===== PLC::Builder =======================================================

Builder::~Builder() {}
