
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI_FunctionList.cpp

#include "Component.h"

// ===== C++ ================================================================
#include <codecvt>

// ===== Import/Includes ====================================================
#include <KMS/Console/Color.h>

// ===== Local ==============================================================
#include "../Common/TRiLOGI/FunctionList.h"

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

    Function* FunctionList::Find_ByIndex(unsigned int aIndex)
    {
        return dynamic_cast<Function*>(FindObject_ByIndex(aIndex));
    }

    unsigned int FunctionList::Parse_Code(const Text::File_UTF16& aFile_PC6, unsigned int aLineNo)
    {
        Function   * lFunction  = NULL;
        auto         lLineCount = aFile_PC6.GetLineCount();
        unsigned int lIndex;
        auto         lLineNo    = aLineNo;
        char         lMsg[64];

        for (; lLineNo < lLineCount; lLineNo++)
        {
            auto lLine = aFile_PC6.GetLine(lLineNo);
            assert(NULL != lLine);

            if (0 == wcscmp(L"~END_CUSTFN~\r", lLine)) { mLineNo_End_Code = lLineNo; lLineNo++; break; }

            if (0 == wcscmp(L"�\r", lLine))
            {
                if (NULL != lFunction)
                {
                    auto lBI = mObjects_ByIndex.insert(ByIndex::value_type(lIndex, lFunction));

                    sprintf_s(lMsg, "Line %u  A function already exist at index %u", lLineNo, lIndex);
                    KMS_EXCEPTION_ASSERT(lBI.second, APPLICATION_ERROR, lMsg, "");
                }

                lLineNo++;

                lLine = aFile_PC6.GetLine(lLineNo);
                assert(NULL != lLine);

                unsigned int lLength;

                auto lRet = swscanf_s(lLine, L"Fn#%u,%u", &lIndex, &lLength);

                sprintf_s(lMsg, "Line %u  Corrupted PC6 file", lLineNo);
                KMS_EXCEPTION_ASSERT(2 == lRet, APPLICATION_ERROR, lMsg, lRet);

                lFunction = new Function(lIndex, lLength);
            }
        }

        if (NULL != lFunction)
        {
            auto lBI = mObjects_ByIndex.insert(ByIndex::value_type(lIndex, lFunction));

            sprintf_s(lMsg, "Line %u  A function already exist at index %u", aLineNo, lIndex);
            KMS_EXCEPTION_ASSERT(lBI.second, APPLICATION_ERROR, lMsg, "");
        }

        std::cout << "    " << GetCount() << " " << "functions" << std::endl;

        return lLineNo;
    }

    unsigned int FunctionList::Parse_Name(const Text::File_UTF16& aFile_PC6, unsigned int aLineNo)
    {
        auto lLineCount = aFile_PC6.GetLineCount();
        auto lLineNo    = aLineNo;

        for (; lLineNo < lLineCount; lLineNo++)
        {
            auto lLine = aFile_PC6.GetLine(lLineNo);
            if (0 == wcscmp(L"~END_CUSTFNLABEL~\r", lLine)) { SetLineNo_End(lLineNo); lLineNo++; break; }

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

        std::cout << "    " << GetCount() << " " << "function names" << std::endl;

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

            switch (aFile_PC6.CountOccurrence(lConverter.from_bytes(lFunction->GetName()).c_str()))
            {
            case 0: assert(false);

            case 1:
                lCount++;
                lFunction->AddFlags(Object::FLAG_NOT_USED);
                std::cout << Console::Color::YELLOW << "    WARNING  Line " << lFunction->GetLineNo();
                std::cout << "  The function named \"" << lFunction->GetName();
                std::cout << "\" (" << lFunction->GetIndex() << ") is not used" << Console::Color::WHITE << std::endl;
                break;
            }
        }

        if (0 < lCount)
        {
            std::cout << Console::Color::YELLOW << "    WARNING  " << lCount << " unused functions" << Console::Color::WHITE << std::endl;
        }
    }

}
