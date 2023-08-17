
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI_FunctionList.cpp

// TEST COVERAGE  2023-08-15  KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== C++ ================================================================
#include <codecvt>

// ===== Local ==============================================================
#include "../Common/TRiLOGI/FunctionList.h"

#include "TRiLOGI/PC6.h"
#include "TRiLOGI/Function.h"

using namespace KMS;

namespace TRiLOGI
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    FunctionList::FunctionList() : ObjectList("function", 256) {}

    Function* FunctionList::Find_ByIndex(unsigned int aIndex)
    {
        return dynamic_cast<Function*>(FindObject_ByIndex(aIndex));
    }

    Function* FunctionList::Find_ByName(const char* aName)
    {
        return dynamic_cast<Function*>(FindObject_ByName(aName));
    }

    void FunctionList::AddCodeToFile(Text::File_UTF16* aFile_PC6)
    {
        for (const auto& lEntry : mObjects_ByIndex)
        {
            auto* lFunction = dynamic_cast<Function*>(lEntry.second);
            assert(nullptr != lFunction);

            lFunction->AddCodeToFile(aFile_PC6);
        }
    }

    bool FunctionList::Import(const char* aName, Text::File_ASCII* aFile, Text::File_ASCII::Internal::iterator* aIt)
    {
        unsigned int lIndex;

        auto lFunction = Find_ByName(aName);
        if (nullptr == lFunction)
        {
            lIndex = FindFreeIndex();
        }
        else
        {
            lIndex = lFunction->GetIndex();
        }

        auto lNew = new Function(aName, lIndex);

        try
        {
            lNew->Parse(aFile, aIt);
        }
        catch (...)
        {
            delete lNew;
            throw;
        }

        auto lResult = true;

        if (nullptr == lFunction)
        {
            if (IsProjectLegacy())
            {
                ::Console::Change("New function", aName);
            }

            AddObject(lNew);
        }
        else
        {
            if (*lNew != *lFunction)
            {
                ::Console::Change("Function changed (NOT TESTED)", aName);

                Replace(lFunction, lNew);
            }
            else
            {
                delete lNew;
                lResult = false;
            }
        }

        return lResult;
    }

    unsigned int FunctionList::Parse_Code(Text::File_UTF16* aFile_PC6, unsigned int aLineNo)
    {
        assert(nullptr != aFile_PC6);
        assert(0 < aLineNo);

        Function   * lFunction  = nullptr;
        auto         lLineCount = aFile_PC6->GetLineCount();
        unsigned int lIndex;
        auto         lLineNo    = aLineNo;
        char         lMsg[64];

        for (; lLineNo < lLineCount; lLineNo++)
        {
            unsigned int lLength;

            auto lLine = aFile_PC6->GetLine(lLineNo);
            assert(nullptr != lLine);

            if (0 == wcscmp(PC6_SECTION_END_CUSTFN, lLine)) { lLineNo++; break; }

            if (0 == wcscmp(L"\xc8", lLine))
            {
                if (nullptr != lFunction)
                {
                    auto lBI = mObjects_ByIndex.insert(ByIndex::value_type(lIndex, lFunction));
                    if (!lBI.second)
                    {
                        sprintf_s(lMsg, "Line %u  A function already exist at index %u (NOT TESTED)", lLineNo, lIndex);
                        KMS_EXCEPTION(APPLICATION_ERROR, lMsg, "");
                    }

                    lFunction = nullptr;
                }
            }
            else if (2 == swscanf_s(lLine, L"Fn#%u,%u", &lIndex, &lLength))
            {
                if (nullptr != lFunction)
                {
                    sprintf_s(lMsg, "Line %u  A function is malformed (NOT TESTED)", lLineNo);
                    KMS_EXCEPTION(APPLICATION_ERROR, lMsg, "");
                }

                lFunction = new Function(lIndex, lLength);
            }
            else
            {
                if (nullptr != lFunction)
                {
                    char lLineASCII[LINE_LENGTH];

                    sprintf_s(lLineASCII, "%S", lLine);

                    lFunction->AddLine(lLineASCII);
                }
            }
        }

        if (nullptr != lFunction)
        {
            auto lBI = mObjects_ByIndex.insert(ByIndex::value_type(lIndex, lFunction));
            if (!lBI.second)
            {
                sprintf_s(lMsg, "Line %u  A function already exist at index %u (NOT TESTED)", aLineNo, lIndex);
                KMS_EXCEPTION(APPLICATION_ERROR, lMsg, "");
            }
        }

        ::Console::Stats(GetCount(), "functions");

        return lLineNo;
    }

    unsigned int FunctionList::Parse_Name(Text::File_UTF16* aFile_PC6, unsigned int aLineNo)
    {
        assert(nullptr != aFile_PC6);
        assert(0 < aLineNo);

        auto lLineCount = aFile_PC6->GetLineCount();
        auto lLineNo    = aLineNo;

        for (; lLineNo < lLineCount; lLineNo++)
        {
            auto lLine = aFile_PC6->GetLine(lLineNo);
            if (0 == wcscmp(PC6_SECTION_END_CUSTFNLABEL, lLine)) { lLineNo++; break; }

            unsigned int lIndex;
            char         lMsg[64 + NAME_LENGTH];
            char         lName[NAME_LENGTH];

            auto lRet = swscanf_s(lLine, L"%u,%S", &lIndex, lName SizeInfo(lName));
            if (2 != lRet)
            {
                sprintf_s(lMsg, "Line %u  Corrupted PC6 file (NOT TESTED)", aLineNo);
                KMS_EXCEPTION(APPLICATION_ERROR, lMsg, lRet);
            }

            auto lFunction = Find_ByIndex(lIndex);
            if (nullptr == lFunction)
            {
                // NOT TESTED
                lFunction = new Function(lIndex, 0);

                auto lBI = mObjects_ByIndex.insert(ByIndex::value_type(lIndex, lFunction));
                if (!lBI.second)
                {
                    sprintf_s(lMsg, "Line %u  A function already exist at index %u", aLineNo, lIndex);
                    KMS_EXCEPTION(APPLICATION_ERROR, lMsg, "");
                }
            }

            lFunction->SetName(lName);

            auto lBI = mObjects_ByName.insert(ByName::value_type(lName, lFunction));
            if (!lBI.second)
            {
                sprintf_s(lMsg, "Line %u  A function named \"%s\" already exist (NOT TESTED)", lLineNo, lName);
                KMS_EXCEPTION(APPLICATION_ERROR, lMsg, "");
            }
        }

        ::Console::Stats(GetCount(), "function names");

        return lLineNo;
    }

    void FunctionList::Verify(const Text::File_UTF16& aFile_PC6)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> lConverter;

        unsigned int lCount = 0;

        for (auto& lVT : mObjects_ByIndex)
        {
            auto lFunction = lVT.second;
            assert(nullptr != lFunction);

            auto lName = lFunction->GetName();
            assert(nullptr != lName);

            switch (aFile_PC6.CountOccurrence(lConverter.from_bytes(lName).c_str()))
            {
            case 0: assert(false);

            case 1:
                lCount++;
                lFunction->AddFlags(Object::FLAG_NOT_USED);

                ::Console::Warning_Begin()
                    << "The function named \"" << lName << "\" (" << lFunction->GetIndex() << ") is not used (NOT TESTED)";
                ::Console::Warning_End();
                break;
            }
        }

        if (0 < lCount)
        {
            ::Console::Warning_Begin()
                << lCount << " unused functions (NOT TESTED)";
            ::Console::Warning_End();
        }
    }

}
