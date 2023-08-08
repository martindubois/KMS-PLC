
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI_DefineList.cpp

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

    void DefineList::Clear()
    {
        mConstants.Clear();
        mWords    .Clear();

        ObjectList::Clear();
    }

    bool DefineList::ImportConstant(const char* aName, const char* aValue)
    {
        auto lObject = FindObject_ByName(aName);
        if (NULL == lObject)
        {
            ::Console::Change("New constant", aName);

            unsigned int lIndex;
            unsigned int lLineNo;

            FindIndexAndLineNo(&lIndex, &lLineNo);

            auto lConstant = new Constant(aName, lIndex, lLineNo, aValue, "", Object::FLAG_TO_INSERT);

            mConstants.AddConstant(lConstant);

            ObjectList::AddObject(lConstant);

            return true;
        }

        auto lConstant = dynamic_cast<Constant*>(lObject);
        if (NULL == lConstant)
        {
            ::Console::Error_Begin()
                << "The name " << aName << " is already used for a word";
            ::Console::Error_End();
            return false;
        }

        return lConstant->SetValue(aValue, GetFile_PC6());
    }

    bool DefineList::ImportWord(const char* aName)
    {
        auto lObject = FindObject_ByName(aName);
        if (NULL == lObject)
        {
            ::Console::Change("New word", aName);

            unsigned int lIndex;
            unsigned int lLineNo;

            FindIndexAndLineNo(&lIndex, &lLineNo);

            lObject = mWords.AddWord(aName, lIndex, lLineNo, "", Object::FLAG_TO_INSERT);

            ObjectList::AddObject(lObject);

            return true;
        }

        auto lWord = dynamic_cast<Word*>(lObject);
        if (NULL == lWord)
        {
            ::Console::Error_Begin()
                << "The name " << aName << " is already used for a constant";
            ::Console::Error_End();
        }

        return false;
    }

    bool DefineList::ImportWord(const char* aName, unsigned int aOffset)
    {
        auto lObject = FindObject_ByName(aName);
        if (NULL == lObject)
        {
            auto lWord = mWords.Find_ByOffset(aOffset);
            if (NULL != lWord)
            {
                ::Console::Error_Begin()
                    << "The offset " << aOffset << " is already used";
                ::Console::Error_End();
                return false;
            }

            ::Console::Change("New word", aName);

            unsigned int lIndex;
            unsigned int lLineNo;

            FindIndexAndLineNo(&lIndex, &lLineNo);

            auto lNewWord = new Word(aName, lIndex, lLineNo, aOffset, "", Object::FLAG_TO_INSERT);

            mWords.AddWord(lNewWord);

            ObjectList::AddObject(lNewWord);

            return true;
        }

        auto lWord = dynamic_cast<Word*>(lObject);
        if (NULL == lWord)
        {
            ::Console::Error_Begin()
                << "The name " << aName << " is already used for a constant";
            ::Console::Error_End();
        }
        else if (lWord->GetOffset() != aOffset)
        {
            ::Console::Error_Begin()
                << "The name " << aName << " is already uses witch another offset";
            ::Console::Error_End();
        }

        return false;
    }

    unsigned int DefineList::Parse(Text::File_UTF16* aFile_PC6, unsigned int aLineNo)
    {
        assert(NULL != aFile_PC6);

        auto lResult = ObjectList::Parse(aFile_PC6, aLineNo, 0);

        ::Console::Stats(mConstants.GetCount(), "constants");
        ::Console::Stats(mWords    .GetCount(), "words");

        return lResult;
    }

    void DefineList::Verify(const Text::File_UTF16& aFile_PC6)
    {
        ObjectList::Verify(aFile_PC6);

        mConstants.Verify();
    }

    // Protected
    // //////////////////////////////////////////////////////////////////////

    // ===== ObjectList =====================================================

    void DefineList::AddObject(const wchar_t* aLine, unsigned int aLineNo, unsigned int aFlags)
    {
        assert(NULL != aLine);

        unsigned int lIndex;
        char         lMsg[64];
        char         lText[LINE_LENGTH];

        auto lRet = swscanf_s(aLine, L"%u,%S", &lIndex, lText SizeInfo(lText));
        if (2 != lRet)
        {
            sprintf_s(lMsg, "Line %u  Invalid define line", aLineNo);
            KMS_EXCEPTION(APPLICATION_ERROR, lMsg, lRet);
        }

        char         lComment[NAME_LENGTH];
        Object     * lDefine;
        char         lName[NAME_LENGTH];
        unsigned int lOffset;
        char         lValue[NAME_LENGTH];

        memset(&lComment, 0, sizeof(lComment));
        memset(&lValue, 0, sizeof(lValue));

        if (2 <= sscanf_s(lText, "%[^,],DM[%u],%[^\n\r\t]", &lName SizeInfo(lName), &lOffset, lComment SizeInfo(lComment)))
        {
            auto lWord = new Word(lName, lIndex, aLineNo, lOffset, lComment, aFlags);

            mWords.AddWord(lWord);

            lDefine = lWord;
        }
        else if (1 <= sscanf_s(lText, "%[^,],%[^,],%[^\n\r\t]", &lName SizeInfo(lName), lValue SizeInfo(lValue), lComment SizeInfo(lComment)))
        {
            auto lConstant = new Constant(lName, lIndex, aLineNo, lValue, lComment, aFlags);

            mConstants.AddConstant(lConstant);

            lDefine = lConstant;
        }
        else
        {
            KMS_EXCEPTION(APPLICATION_ERROR, lMsg, "");
        }

        ObjectList::AddObject(lDefine);
    }

}
