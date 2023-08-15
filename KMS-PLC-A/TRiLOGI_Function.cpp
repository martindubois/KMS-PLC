
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI_Function.cpp

// TEST COVERAGE  2023-08-08  KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== C++ ================================================================
#include <codecvt>

// ===== Local ==============================================================
#include "TRiLOGI/PC6.h"

#include "TRiLOGI/Function.h"

using namespace KMS;

namespace TRiLOGI
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    Function::Function(unsigned int aIndex, unsigned int aLength)
        : Object(aIndex)
        , mLength(aLength)
    {
        Init();
    }

    Function::Function(const char* aName, unsigned int aIndex, unsigned int aLineNo, unsigned int aFlags)
        : Object(aName, aIndex, aLineNo, aFlags)
        , mLength(0)
    {
        Init();
    }

    unsigned int Function::GetLineNo_Code_Begin() const { return mLineNo_Code_Begin; }
    unsigned int Function::GetLineNo_Code_End  () const { return mLineNo_Code_End  ; }

    void Function::SetLineNo_Code(unsigned int aBegin, unsigned int aEnd)
    {
        assert(aBegin <= aEnd);

        mLineNo_Code_Begin = aBegin;
        mLineNo_Code_End   = aEnd  ;
    }

    void Function::AddCodeToFile(Text::File_UTF16* aFile_PC6)
    {
        assert(nullptr != aFile_PC6);

        aFile_PC6->AddLine(PC6_FUNCTION_MARK);

        wchar_t lLine[LINE_LENGTH];

        swprintf_s(lLine, L"Fn#%u,%u", GetIndex(), mLength);

        aFile_PC6->AddLine(lLine);

        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> lConverter;

        for (const auto& lLine : mLines)
        {
            std::wstring lLineW(lConverter.from_bytes(lLine.c_str()));

            aFile_PC6->AddLine(lLineW.c_str());

        }
    }

    // NOT TESTED
    void Function::Apply(Text::File_UTF16* aFile)
    {
        assert(nullptr != aFile);

        assert(mLineNo_Code_Begin < mLineNo_Code_End);

        unsigned int lIndex    = 0;
        unsigned int lLineNo   = mLineNo_Code_Begin + 1;
        unsigned int lNewCount = GetLineCount();
        unsigned int lOldCount = mLineNo_Code_End - mLineNo_Code_Begin - 1;

        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> lConverter;

        while ((0 < lNewCount) && (0 < lOldCount))
        {
            std::wstring lLine(lConverter.from_bytes(::Function::GetLine(lIndex)));

            aFile->ReplaceLine(lLineNo, lLine.c_str()); lIndex++; lLineNo++; lNewCount--; lOldCount--;
        }

        while (0 < lNewCount)
        {
            std::wstring lLine(lConverter.from_bytes(::Function::GetLine(lIndex)));

            aFile->InsertLine(lLineNo, lLine.c_str()); lIndex++; lLineNo++; lNewCount--;
        }

        while (0 < lOldCount)
        {
            aFile->RemoveLines(lLineNo, 1); lOldCount--;
        }
    }

    void Function::Insert(Text::File_UTF16* aFile)
    {
        assert(nullptr != aFile);

        unsigned int lLineNo = mLineNo_Code_Begin;

        aFile->InsertLine(lLineNo, PC6_FUNCTION_MARK); lLineNo++;

        wchar_t lLine[LINE_LENGTH];

        swprintf_s(lLine, L"Fn#%u,%u", GetIndex(), mLength);

        aFile->InsertLine(lLineNo, lLine); lLineNo++;

        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> lConverter;

        unsigned int lIndex    = 0;
        unsigned int lNewCount = GetLineCount();

        while (0 < lNewCount)
        {
            std::wstring lLine(lConverter.from_bytes(::Function::GetLine(lIndex)));

            aFile->InsertLine(lLineNo, lLine.c_str()); lIndex++; lLineNo++; lNewCount--;
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

        AddLine("");

        mLength = GetLength();
    }

    // ===== Object =========================================================

    Function::~Function() {}

    // Private
    // //////////////////////////////////////////////////////////////////////

    void Function::Init()
    {
        mLineNo_Code_Begin = 0;
        mLineNo_Code_End   = 0;
    }

}
