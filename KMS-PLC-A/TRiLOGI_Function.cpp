
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI_Function.cpp

// TEST COVERAGE  2023-08-15  KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== C++ ================================================================
#include <codecvt>

// ===== Local ==============================================================
#include "TRiLOGI/PC6.h"

#include "TRiLOGI/Function.h"

#include "Utilities.h"

using namespace KMS;

namespace TRiLOGI
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    Function::Function(unsigned int aIndex, unsigned int aLength)
        : Object(aIndex)
        , mLength(aLength)
    {}

    Function::Function(const char* aName, unsigned int aIndex)
        : Object(aName, aIndex, 0)
        , mLength(0)
    {}

    void Function::AddCodeToFile(Text::File_UTF16* aFile_PC6)
    {
        assert(nullptr != aFile_PC6);

        aFile_PC6->AddLine(PC6_FUNCTION_MARK);

        wchar_t lLine[LINE_LENGTH];

        swprintf_s(lLine, L"Fn#%u,%u\r", GetIndex(), mLength);

        aFile_PC6->AddLine(lLine);

        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> lConverter;

        for (auto& lLine : mLines)
        {
            Utl_RemoveSpecialChar(&lLine);

            std::wstring lLineW(lConverter.from_bytes(lLine.c_str()));

            aFile_PC6->AddLine(lLineW.c_str());
        }
    }

    void Function::Parse(Text::File_ASCII* aFile, Text::File_ASCII::Internal::iterator* aIt)
    {
        assert(nullptr != aIt);

        (*aIt)++;

        char lLine[LINE_LENGTH];

        sprintf_s(lLine, "// Function %s", GetName());

        AddLine(lLine);

        AddLine("// Imported using KMS-PLC");

        ::Function::Parse(aFile, aIt, "FUNCTION END");

        mLength = GetLength();
    }

    // ===== Object =========================================================

    Function::~Function() {}

}
