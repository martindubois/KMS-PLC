
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI_DefineList.cpp

// TEST COVERAGE  2023-08-15  KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== Local ==============================================================
#include "../Common/TRiLOGI/DefineList.h"

#include "TRiLOGI/Constant.h"
#include "TRiLOGI/Word.h"

using namespace KMS;

namespace TRiLOGI
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    DefineList::DefineList() : ObjectList("define", 2000) {}

    void DefineList::ClearList()
    {
        mConstants.ClearList();
        mWords    .ClearList();

        ObjectList::ClearList();
    }

    bool DefineList::ImportConstant(const char* aName, const char* aValue)
    {
        assert(nullptr != aName);

        auto lObject = FindObject_ByName(aName);
        if (nullptr == lObject)
        {
            if (IsProjectLegacy())
            {
                ::Console::Change("New constant", aName);
            }

            auto lIndex = FindFreeIndex();

            auto lConstant = new Constant(aName, lIndex, aValue, "");

            mConstants.AddConstant(lConstant);

            ObjectList::AddObject(lConstant);

            return true;
        }

        auto lConstant = dynamic_cast<Constant*>(lObject);
        if (nullptr == lConstant)
        {
            ::Console::Error_Begin()
                << "The name " << aName << " is already used for a word (NOT TESTED)";
            ::Console::Error_End();
            return false;
        }

        return lConstant->SetValue(aValue);
    }

    bool DefineList::ImportWord(const char* aName)
    {
        assert(nullptr != aName);

        auto lObject = FindObject_ByName(aName);
        if (nullptr == lObject)
        {
            if (IsProjectLegacy())
            {
                ::Console::Change("New word", aName);
            }

            auto lIndex = FindFreeIndex();

            lObject = mWords.AddWord(aName, lIndex, "");

            ObjectList::AddObject(lObject);

            return true;
        }

        auto lWord = dynamic_cast<Word*>(lObject);
        if (nullptr == lWord)
        {
            ::Console::Error_Begin()
                << "The name " << aName << " is already used for a constant (NOT TESTED)";
            ::Console::Error_End();
        }

        return false;
    }

    bool DefineList::ImportWord(const char* aName, unsigned int aOffset)
    {
        assert(nullptr != aName);

        auto lObject = FindObject_ByName(aName);
        if (nullptr == lObject)
        {
            auto lWord = mWords.Find_ByOffset(aOffset);
            if (nullptr != lWord)
            {
                ::Console::Error_Begin()
                    << "The offset " << aOffset << " is already used";
                ::Console::Error_End();
                return false;
            }

            if (IsProjectLegacy())
            {
                ::Console::Change("New word", aName);
            }

            auto lIndex = FindFreeIndex();

            auto lNewWord = new Word(aName, lIndex, aOffset, "");

            mWords.AddWord(lNewWord);

            ObjectList::AddObject(lNewWord);

            return true;
        }

        auto lWord = dynamic_cast<Word*>(lObject);
        if (nullptr == lWord)
        {
            ::Console::Error_Begin()
                << "The name " << aName << " is already used for a constant (NOT TESTED)";
            ::Console::Error_End();
        }
        else if (lWord->GetOffset() != aOffset)
        {
            ::Console::Error_Begin()
                << "The name " << aName << " is already uses witch another offset (NOT TESTED)";
            ::Console::Error_End();
        }

        return false;
    }

    unsigned int DefineList::Parse(Text::File_UTF16* aFile_PC6, unsigned int aLineNo)
    {
        auto lResult = ObjectList::Parse(aFile_PC6, aLineNo, 0);

        ::Console::Stats(mConstants.GetCount(), "constants");
        ::Console::Stats(mWords    .GetCount(), "words");

        return lResult;
    }

    void DefineList::Verify(const Text::File_UTF16& aFile_PC6, const AddressList* aPublicAddresses)
    {
        ObjectList::Verify(aFile_PC6, aPublicAddresses);

        mConstants.Verify();
    }

    // ===== ObjectList =====================================================

    void DefineList::SetProjectType(ProjectType aPT)
    {
        ObjectList::SetProjectType(aPT);

        mWords.SetProjectType(aPT);
    }

    // Protected
    // //////////////////////////////////////////////////////////////////////

    // ===== ObjectList =====================================================

    void DefineList::AddObject(const wchar_t* aLine, unsigned int)
    {
        assert(nullptr != aLine);

        unsigned int lIndex;
        char         lText[LINE_LENGTH];

        auto lRet = swscanf_s(aLine, L"%u,%S", &lIndex, lText SizeInfo(lText));
        KMS_EXCEPTION_ASSERT(2 == lRet, APPLICATION_ERROR, "Invalid define line", lRet);

        char         lComment[NAME_LENGTH];
        Object     * lDefine;
        char         lName[NAME_LENGTH];
        unsigned int lOffset;
        char         lValue[NAME_LENGTH];

        memset(&lComment, 0, sizeof(lComment));
        memset(&lValue, 0, sizeof(lValue));

        if (2 <= sscanf_s(lText, "%[^,],DM[%u],%[^\n\r\t]", &lName SizeInfo(lName), &lOffset, lComment SizeInfo(lComment)))
        {
            auto lWord = new Word(lName, lIndex, lOffset, lComment);

            mWords.AddWord(lWord);

            lDefine = lWord;
        }
        else if (1 <= sscanf_s(lText, "%[^,],%[^,],%[^\n\r\t]", &lName SizeInfo(lName), lValue SizeInfo(lValue), lComment SizeInfo(lComment)))
        {
            auto lConstant = new Constant(lName, lIndex, lValue, lComment);

            mConstants.AddConstant(lConstant);

            lDefine = lConstant;
        }
        else
        {
            KMS_EXCEPTION(APPLICATION_ERROR, "Invalid define line (NOT TESTED)", "");
        }

        ObjectList::AddObject(lDefine);
    }

}
