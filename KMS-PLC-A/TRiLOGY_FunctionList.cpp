
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGY_FunctionList.cpp

#include "Component.h"

// ===== C++ ================================================================
#include <codecvt>

// ===== Import/Includes ====================================================
#include <KMS/Console/Color.h>

// ===== Local ==============================================================
#include "../Common/TRiLOGY/FunctionList.h"

namespace TRiLOGY
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    FunctionList::FunctionList() : ObjectList("function", L"~END_CUSTFNLABEL~\r") {}

    Function* FunctionList::Find_ByIndex(unsigned int aIndex)
    {
        return dynamic_cast<Function*>(FindObject_ByIndex(aIndex));
    }

    unsigned int FunctionList::Parse_Code(const KMS::Text::File_UTF16& aFile_PC6, unsigned int aLineNo)
    {
        Function   * lFunction  = NULL;
        unsigned int lLineCount = aFile_PC6.GetLineCount();
        unsigned int lIndex;
        unsigned int lLineNo    = aLineNo;

        for (; lLineNo < lLineCount; lLineNo++)
        {
            const wchar_t* lLine = aFile_PC6.GetLine(lLineNo);
            assert(NULL != lLine);

            if (0 == wcscmp(L"~END_CUSTFN~\r", lLine)) { mLineNo_End_Code = lLineNo; lLineNo++; break; }

            if (0 == wcscmp(L"�\r", lLine))
            {
                if (NULL != lFunction)
                {
                    std::pair<ByIndex::iterator, bool> lBI = mObjects_ByIndex.insert(ByIndex::value_type(lIndex, lFunction));
                    KMS_EXCEPTION_ASSERT(lBI.second, APPLICATION, "A function with the same index already exist", lIndex);
                }

                lLineNo++;

                lLine = aFile_PC6.GetLine(lLineNo);
                assert(NULL != lLine);

                unsigned int lLength;

                int lRet = swscanf_s(lLine, L"Fn#%u,%u", &lIndex, &lLength);
                KMS_EXCEPTION_ASSERT(2 == lRet, APPLICATION, "Corrupted PC6 file", lRet);

                lFunction = new Function(lIndex, lLength);
            }
        }

        if (NULL != lFunction)
        {
            std::pair<ByIndex::iterator, bool> lBI = mObjects_ByIndex.insert(ByIndex::value_type(lIndex, lFunction));
            KMS_EXCEPTION_ASSERT(lBI.second, APPLICATION, "A function with the same index already exist", lIndex);
        }

        std::cout << "    " << GetCount() << " " << "functions" << std::endl;

        return lLineNo;
    }

    unsigned int FunctionList::Parse_Name(const KMS::Text::File_UTF16& aFile_PC6, unsigned int aLineNo)
    {
        unsigned int lLineCount = aFile_PC6.GetLineCount();
        unsigned int lLineNo    = aLineNo;

        for (; lLineNo < lLineCount; lLineNo++)
        {
            const wchar_t* lLine = aFile_PC6.GetLine(lLineNo);
            if (0 == wcscmp(L"~END_CUSTFNLABEL~\r", lLine)) { SetLineNo_End(lLineNo); lLineNo++; break; }

            unsigned int lIndex;
            char         lName[NAME_LENGTH];

            int lRet = swscanf_s(lLine, L"%u,%S", &lIndex, lName SizeInfo(lName));
            KMS_EXCEPTION_ASSERT(2 == lRet, APPLICATION, "Corrupted PC6 file", lRet);

            Function* lFunction = Find_ByIndex(lIndex);
            if (NULL == lFunction)
            {
                lFunction = new Function(lIndex, 0);

                std::pair<ByIndex::iterator, bool> lBI = mObjects_ByIndex.insert(ByIndex::value_type(lIndex, lFunction));
                KMS_EXCEPTION_ASSERT(lBI.second, APPLICATION, "A function with the same index already exist", lIndex);
            }

            lFunction->SetName(lName, lLineNo);

            std::pair<ByName::iterator, bool> lBI = mObjects_ByName.insert(ByName::value_type(lName, lFunction));
            KMS_EXCEPTION_ASSERT(lBI.second, APPLICATION, "A function with the same name already exist", lName);
        }

        std::cout << "    " << GetCount() << " " << "function names" << std::endl;

        return lLineNo;
    }

    void FunctionList::Verify(const KMS::Text::File_UTF16& aFile_PC6)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> lConverter;

        unsigned int lCount = 0;

        for (ByIndex::value_type& lVT : mObjects_ByIndex)
        {
            Object* lFunction = lVT.second;
            assert(NULL != lFunction);

            switch (aFile_PC6.CountOccurrence(lConverter.from_bytes(lFunction->GetName()).c_str()))
            {
            case 0: assert(false);

            case 1:
                lCount++;
                lFunction->AddFlags(Object::FLAG_NOT_USED);
                std::cout << KMS::Console::Color::YELLOW << "    WARNING  The function named " << lFunction->GetName();
                std::cout << " (" << lFunction->GetIndex() << ") is not used" << KMS::Console::Color::WHITE << std::endl;
                break;
            }
        }

        if (0 < lCount)
        {
            std::cout << KMS::Console::Color::YELLOW << "    WARNING  " << lCount << " unused functions" << KMS::Console::Color::WHITE << std::endl;
        }
    }

}
