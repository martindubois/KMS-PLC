
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      TRiLOGI_Build/Builder.cpp

#include "Component.h"

// ===== C++ ================================================================
#include <fstream>

// ===== Local ==============================================================
#include "../Common/PLC/PLC.h"
#include "../Common/TRiLOGI/PC6.h"

#include "TRiLOGI_Builder.h"

using namespace KMS;

using namespace TRiLOGI;

// Constants
// //////////////////////////////////////////////////////////////////////////

static const wchar_t* END_OF_LINE = L"\r\n";

static const char* TO_COMPILE_PC6 = "PLC_ToCompile.PC6";

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

void Write_DEFINE(std::wofstream& aOut, const PLC::Define& aDefine);

void WriteElement(std::wofstream& aOut, const PLC::Element& aElement);

void WriteElements(std::wofstream& aOut, const PLC::Element_Map& aElements);

// Public
// //////////////////////////////////////////////////////////////////////////

TRiLOGI_Builder::TRiLOGI_Builder() {}

// ===== Builder ============================================================

TRiLOGI_Builder::~TRiLOGI_Builder() {}

// Protected
// //////////////////////////////////////////////////////////////////////////

// ===== PLC::Builder =======================================================

void TRiLOGI_Builder::Write()
{
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

    WriteElements(lOut, mInputs);
    WriteElements(lOut, mOutputs);
    WriteElements(lOut, mRelays);
    Write_TIMER(lOut);
    Write_SEQUENCE(lOut);

    // Circuit
    std::wstring lC;

    while (getline(lCircuits, lC))
    {
        lOut << lC << L"\n";
    }

    Write_FUNCTION(lOut);
    Write_FUNCTION_LABEL(lOut);

    // Tag
    lOut << PC6_END_TAG << END_OF_LINE;

    Write_DEFINE(lOut);

    // Breakpoint
    lOut << PC6_END_BREAKPOINT << END_OF_LINE;

    // Last IP Addr
    lOut << PC6_END_LAST_IP_ADDR << END_OF_LINE;
}

// Private
// //////////////////////////////////////////////////////////////////////////

void TRiLOGI_Builder::Write_DEFINE(std::wofstream& aOut)
{
    auto lIt = mDefines_Auto.begin();

    uint16_t lCurrent = 0;

    for (const auto& lD : mDefines)
    {
        auto lIndex = lD.second.GetIndex();
        assert(PLC::Element::INVALID_INDEX != lIndex);

        while ((lCurrent < lIndex) && (mDefines_Auto.end() != lIt))
        {
            // An index is available and we have auto define to write

            lIt->SetIndex(lCurrent);

            ::Write_DEFINE(aOut, *lIt);

            lCurrent++;
            lIt++;
        }

        ::Write_DEFINE(aOut, lD.second);

        lCurrent = lIndex + 1;
    }

    while (mDefines_Auto.end() != lIt)
    {
        lIt->SetIndex(lCurrent);

        ::Write_DEFINE(aOut, *lIt);

        lCurrent++;
        lIt++;
    }

    aOut << PC6_END_DEFINE << END_OF_LINE;

    assert(mDefines_Auto.end() == lIt);
}

void TRiLOGI_Builder::Write_FUNCTION(std::wofstream& aOut)
{
    wchar_t lLine[LINE_LENGTH];

    for (const auto& lF : mFunctions)
    {
        assert(nullptr != lF.second);

        if (!lF.second->IsNameEmpty())
        {
            auto lIndex = lF.second->GetIndex();
            assert(PLC::Function::INVALID_INDEX != lIndex);

            aOut << PC6_BEGIN_FUNCTION << END_OF_LINE;
            aOut << "Fn#" << lIndex << "," << lF.second->GetSize() << END_OF_LINE;

            for (auto& lL : lF.second->mLines)
            {
                auto lRet = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, lL.c_str(), -1, lLine, sizeof(lLine) / sizeof(lLine[0]));
                assert(0 < lRet);

                aOut << lLine << "\n";
            }
        }
    }

    aOut << PC6_END_FUNCTION << END_OF_LINE;
}

void TRiLOGI_Builder::Write_FUNCTION_LABEL(std::wofstream& aOut)
{
    for (const auto& lF : mFunctions)
    {
        assert(nullptr != lF.second);

        if (!lF.second->IsNameEmpty())
        {
            WriteElement(aOut, *lF.second);
        }
    }

    aOut << PC6_END_FUNCTION_LABEL << END_OF_LINE;
}

void TRiLOGI_Builder::Write_SEQUENCE(std::wofstream& aOut)
{
    wchar_t lName[NAME_LENGTH];

    for (const auto& lS : mSequences)
    {
        auto lIndex = lS.second.GetIndex();
        assert(PLC::Sequence::INVALID_INDEX != lIndex);

        lS.second.GetName(lName, sizeof(lName));

        aOut << lIndex << "," << lName << " " << lS.second.GetValue() << END_OF_LINE;
    }

    aOut << PC6_END << END_OF_LINE;
}

void TRiLOGI_Builder::Write_TIMER(std::wofstream& aOut)
{
    wchar_t lName[NAME_LENGTH];

    for (const auto& lT : mTimers)
    {
        auto lIndex = lT.second.GetIndex();
        assert(PLC::Timer::INVALID_INDEX != lIndex);

        lT.second.GetName(lName, sizeof(lName));

        aOut << lIndex << "," << lName << " " << lT.second.GetValue() << END_OF_LINE;
    }

    aOut << PC6_END << END_OF_LINE;
}

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

// TODO  Verify we do not reach the maximum number of define

void Write_DEFINE(std::wofstream& aOut, const PLC::Define& aDefine)
{
    auto lIndex = aDefine.GetIndex();
    assert(PLC::Define::INVALID_INDEX != lIndex);

    wchar_t lName [NAME_LENGTH];
    wchar_t lValue[NAME_LENGTH];

    aDefine.GetName (lName , sizeof(lName ));
    aDefine.GetValue(lValue, sizeof(lValue));

    aOut << lIndex << "," << lName << "," << lValue << END_OF_LINE;
}

void WriteElement(std::wofstream& aOut, const PLC::Element& aElement)
{
    auto lIndex = aElement.GetIndex();
    assert(PLC::Element::INVALID_INDEX != lIndex);

    wchar_t lName[NAME_LENGTH];

    aElement.GetName(lName, sizeof(lName));

    aOut << lIndex << "," << lName << END_OF_LINE;
}

void WriteElements(std::wofstream& aOut, const PLC::Element_Map& aElements)
{
    for (const auto& lE : aElements)
    {
        WriteElement(aOut, lE.second);
    }

    aOut << PC6_END << END_OF_LINE;
}
