
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI_FunctionList.cpp

#include "Component.h"

// ===== C++ ================================================================
#include <codecvt>

// ===== Local ==============================================================
#include "../Common/TRiLOGI/FunctionList.h"

#include "Console.h"
#include "Function.h"

#include "TRiLOGI/Function.h"

using namespace KMS;

namespace TRiLOGI
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    FunctionList::FunctionList() : ObjectList("function", 256) {}

    void FunctionList::Clear()
    {
        mLineNo_End_Code = 0;

        ObjectList::Clear();
    }

    void FunctionList::SetLineNo_End_Code(unsigned int aLineNo)
    {
        mLineNo_End_Code = aLineNo;
    }

    Function* FunctionList::Find_ByIndex(unsigned int aIndex)
    {
        return dynamic_cast<Function*>(FindObject_ByIndex(aIndex));
    }

    Function* FunctionList::Find_ByName(const char* aName)
    {
        return dynamic_cast<Function*>(FindObject_ByName(aName));
    }

    bool FunctionList::Apply_Code()
    {
        auto lFile   = GetFile_PC6();
        auto lResult = false;

        for (auto lIt = mObjects_ByIndex.rbegin(); lIt != mObjects_ByIndex.rend(); lIt++)
        {
            Function* lFunction = dynamic_cast<Function*>(lIt->second);

            if (lIt->second->TestFlag(Object::FLAG_TO_APPLY))
            {
                lResult = true;

                lFunction->Apply(lFile);
            }

            if (lIt->second->TestFlag(Object::FLAG_TO_INSERT))
            {
                lResult = true;

                lFunction->Insert(lFile);
            }
        }

        return lResult;
    }

    bool FunctionList::Import(const char* aName, Text::File_ASCII* aFile, Text::File_ASCII::Internal::iterator* aIt)
    {
        unsigned int lCodeBegin;
        unsigned int lCodeEnd;
        unsigned int lFlags;
        unsigned int lIndex;
        unsigned int lLineNo;

        auto lFunction = Find_ByName(aName);
        if (NULL == lFunction)
        {
            lCodeBegin = mLineNo_End_Code;
            lCodeEnd   = mLineNo_End_Code;
            lFlags     = Object::FLAG_TO_INSERT;
            FindIndexAndLineNo(&lIndex, &lLineNo);
        }
        else
        {
            lCodeBegin = lFunction->GetLineNo_Code_Begin();
            lCodeEnd   = lFunction->GetLineNo_Code_End  ();
            lFlags     = Object::FLAG_TO_APPLY;
            lIndex     = lFunction->GetIndex ();
            lLineNo    = lFunction->GetLineNo();
        }

        auto lNew = new Function(aName, lIndex, lLineNo, lFlags);

        lNew->SetLineNo_Code(lCodeBegin, lCodeEnd);

        try
        {
            lNew->Parse(aFile, aIt);
        }
        catch (...)
        {
            delete lNew;
            throw;
        }

        bool lResult = true;

        if (NULL == lFunction)
        {
            Console::Change("New function", aName);

            AddObject(lNew);
        }
        else
        {
            if (*lNew != *lFunction)
            {
                Console::Change("Function changed", aName);

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
        assert(NULL != aFile_PC6);
        assert(0 < aLineNo);

        unsigned int lBegin;
        Function   * lFunction  = NULL;
        auto         lLineCount = aFile_PC6->GetLineCount();
        unsigned int lIndex;
        auto         lLineNo    = aLineNo;
        char         lMsg[64];

        for (; lLineNo < lLineCount; lLineNo++)
        {
            unsigned int lLength;

            auto lLine = aFile_PC6->GetLine(lLineNo);
            assert(NULL != lLine);

            if (0 == wcscmp(L"~END_CUSTFN~", lLine)) { mLineNo_End_Code = lLineNo; lLineNo++; break; }

            if (0 == wcscmp(L"\xc8", lLine))
            {
                if (NULL != lFunction)
                {
                    lFunction->SetLineNo_Code(lBegin, lLineNo);

                    auto lBI = mObjects_ByIndex.insert(ByIndex::value_type(lIndex, lFunction));

                    sprintf_s(lMsg, "Line %u  A function already exist at index %u", lLineNo, lIndex);
                    KMS_EXCEPTION_ASSERT(lBI.second, APPLICATION_ERROR, lMsg, "");

                    lFunction = NULL;
                }
            }
            else if (2 == swscanf_s(lLine, L"Fn#%u,%u", &lIndex, &lLength))
            {
                sprintf_s(lMsg, "Line %u  A function is malformed", lLineNo);
                KMS_EXCEPTION_ASSERT(NULL == lFunction, APPLICATION_ERROR, lMsg, "");

                lBegin    = lLineNo;
                lFunction = new Function(lIndex, lLength);
            }
            else
            {
                if (NULL != lFunction)
                {
                    char lLine8[LINE_LENGTH];

                    sprintf_s(lLine8, "%S", lLine);

                    lFunction->AddLine(lLine8);
                }
            }
        }

        if (NULL != lFunction)
        {
            lFunction->SetLineNo_Code(lBegin, lLineNo);

            auto lBI = mObjects_ByIndex.insert(ByIndex::value_type(lIndex, lFunction));

            sprintf_s(lMsg, "Line %u  A function already exist at index %u", aLineNo, lIndex);
            KMS_EXCEPTION_ASSERT(lBI.second, APPLICATION_ERROR, lMsg, "");
        }

        Console::Stats(GetCount(), "functions");

        return lLineNo;
    }

    unsigned int FunctionList::Parse_Name(Text::File_UTF16* aFile_PC6, unsigned int aLineNo)
    {
        assert(NULL != aFile_PC6);
        assert(0 < aLineNo);

        auto lLineCount = aFile_PC6->GetLineCount();
        auto lLineNo    = aLineNo;

        for (; lLineNo < lLineCount; lLineNo++)
        {
            auto lLine = aFile_PC6->GetLine(lLineNo);
            if (0 == wcscmp(L"~END_CUSTFNLABEL~", lLine)) { SetFile(aFile_PC6, lLineNo); lLineNo++; break; }

            unsigned int lIndex;
            char         lMsg[64 + NAME_LENGTH];
            char         lName[NAME_LENGTH];

            auto lRet = swscanf_s(lLine, L"%u,%S", &lIndex, lName SizeInfo(lName));

            sprintf_s(lMsg, "Line %u  Corrupted PC6 file", aLineNo);
            KMS_EXCEPTION_ASSERT(2 == lRet, APPLICATION_ERROR, lMsg, lRet);

            auto lFunction = Find_ByIndex(lIndex);
            if (NULL == lFunction)
            {
                lFunction = new Function(lIndex, 0);

                auto lBI = mObjects_ByIndex.insert(ByIndex::value_type(lIndex, lFunction));

                sprintf_s(lMsg, "Line %u  A function already exist at index %u", aLineNo, lIndex);
                KMS_EXCEPTION_ASSERT(lBI.second, APPLICATION_ERROR, lMsg, "");
            }

            lFunction->SetName(lName, lLineNo);

            auto lBI = mObjects_ByName.insert(ByName::value_type(lName, lFunction));

            sprintf_s(lMsg, "Line %u  A function named \"%s\" already exist", lLineNo, lName);
            KMS_EXCEPTION_ASSERT(lBI.second, APPLICATION_ERROR, lMsg, "");
        }

        Console::Stats(GetCount(), "function names");

        return lLineNo;
    }

    void FunctionList::Verify(const Text::File_UTF16& aFile_PC6)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> lConverter;

        unsigned int lCount = 0;

        for (auto& lVT : mObjects_ByIndex)
        {
            auto lFunction = lVT.second;
            assert(NULL != lFunction);

            auto lName = lFunction->GetName();
            assert(NULL != lName);

            switch (aFile_PC6.CountOccurrence(lConverter.from_bytes(lName).c_str()))
            {
            case 0: assert(false);

            case 1:
                lCount++;
                lFunction->AddFlags(Object::FLAG_NOT_USED);

                Console::Warning_Begin(lFunction->GetLineNo());
                std::cout << "The function named \"" << lName << "\" (" << lFunction->GetIndex() << ") is not used";
                Console::Warning_End();
                break;
            }
        }

        if (0 < lCount)
        {
            Console::Warning_Begin();
            std::cout << lCount << " unused functions";
            Console::Warning_End();
        }
    }

}
