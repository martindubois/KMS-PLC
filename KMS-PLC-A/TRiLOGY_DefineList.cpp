
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGY_DefineList.cpp

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KMS/Console/Color.h>

// ===== Local ==============================================================
#include "../Common/TRiLOGY/Object.h"
#include "../Common/TRiLOGY/Constant.h"
#include "../Common/TRiLOGY/Word.h"

#include "../Common/TRiLOGY/DefineList.h"

namespace TRiLOGY
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    DefineList::DefineList() : ObjectList("define", L"~END_DEFINES~\r", 2000) {}

    void DefineList::Clear()
    {
        mConstants.Clear();
        mWords    .Clear();

        ObjectList::Clear();
    }

    bool DefineList::ImportConstant(const char* aName, const char* aValue)
    {
        Object* lObject = FindObject_ByName(aName);
        if (NULL == lObject)
        {
            unsigned int lIndex;
            unsigned int lLineNo;

            FindIndexAndLineNo(&lIndex, &lLineNo);

            Constant* lConstant = new Constant(aName, lIndex, lLineNo, aValue, "", Object::FLAG_TO_INSERT);

            mConstants.AddConstant(lConstant);

            ObjectList::AddObject(lConstant);

            return true;
        }

        Constant* lConstant = dynamic_cast<Constant*>(lObject);
        if (NULL == lConstant)
        {
            std::cout << KMS::Console::Color::RED;
            std::cout << "ERROR  The name " << aName << " is already used for a word";
            std::cout << KMS::Console::Color::WHITE << std::endl;
            return false;
        }

        return lConstant->SetValue(aValue, GetFile_PC6());
    }

    bool DefineList::ImportWord(const char* aName)
    {
        Object* lObject = FindObject_ByName(aName);
        if (NULL == lObject)
        {
            unsigned int lIndex;
            unsigned int lLineNo;

            FindIndexAndLineNo(&lIndex, &lLineNo);

            lObject = mWords.AddWord(aName, lIndex, lLineNo, "", Object::FLAG_TO_INSERT);

            ObjectList::AddObject(lObject);

            return true;
        }

        Word* lWord = dynamic_cast<Word*>(lObject);
        if (NULL == lWord)
        {
            std::cout << KMS::Console::Color::RED;
            std::cout << "ERROR  The name " << aName << " is already used for a constant";
            std::cout << KMS::Console::Color::WHITE << std::endl;
            return false;
        }

        return true;
    }

    bool DefineList::ImportWord(const char* aName, unsigned int aOffset)
    {
        Object* lObject = FindObject_ByName(aName);
        if (NULL == lObject)
        {
            const Word* lWord = mWords.Find_ByOffset(aOffset);
            if (NULL != lWord)
            {
                std::cout << KMS::Console::Color::RED;
                std::cout << "ERROR  The offset " << aOffset << " is already used";
                std::cout << KMS::Console::Color::WHITE << std::endl;
                return false;
            }

            unsigned int lIndex;
            unsigned int lLineNo;

            FindIndexAndLineNo(&lIndex, &lLineNo);

            Word* lNewWord = new Word(aName, lIndex, lLineNo, aOffset, "", Object::FLAG_TO_INSERT);

            mWords.AddWord(lNewWord);

            ObjectList::AddObject(lNewWord);

            return true;
        }

        Word* lWord = dynamic_cast<Word*>(lObject);
        if (NULL == lWord)
        {
            std::cout << KMS::Console::Color::RED;
            std::cout << "ERROR  The name " << aName << " is already used for a constant";
            std::cout << KMS::Console::Color::WHITE << std::endl;
            return false;
        }

        if (lWord->GetOffset() != aOffset)
        {
            std::cout << KMS::Console::Color::RED;
            std::cout << "ERROR  The name " << aName << " is already uses witch another offset";
            std::cout << KMS::Console::Color::WHITE << std::endl;
            return false;
        }

        return true;
    }

    unsigned int DefineList::Parse(KMS::Text::File_UTF16* aFile_PC6, unsigned int aLineNo)
    {
        assert(NULL != aFile_PC6);

        unsigned int lResult = ObjectList::Parse(aFile_PC6, aLineNo);

        std::cout << "    " << mConstants.GetCount() << " constants\n";
        std::cout << "    " << mWords    .GetCount() << " words" << std::endl;

        return lResult;
    }

    void DefineList::Verify(const KMS::Text::File_UTF16& aFile_PC6)
    {
        ObjectList::Verify(aFile_PC6);

        mConstants.Verify();
    }

    // Protected
    // //////////////////////////////////////////////////////////////////////

    void DefineList::AddObject(const wchar_t* aLine, unsigned int aLineNo)
    {
        assert(NULL != aLine);

        unsigned int lIndex;
        char         lText[LINE_LENGTH];

        int lRet = swscanf_s(aLine, L"%u,%S", &lIndex, lText SizeInfo(lText));
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
            Word* lWord = new Word(lName, lIndex, aLineNo, lOffset, lComment, 0);

            mWords.AddWord(lWord);

            lDefine = lWord;
        }
        else if (1 <= sscanf_s(lText, "%[^,],%[^,],%[^\n\r\t]", &lName SizeInfo(lName), lValue SizeInfo(lValue), lComment SizeInfo(lComment)))
        {
            Constant* lConstant = new Constant(lName, lIndex, aLineNo, lValue, lComment, 0);

            mConstants.AddConstant(lConstant);

            lDefine = lConstant;
        }
        else
        {
            KMS_EXCEPTION(APPLICATION_ERROR, "Invalid define line", "");
        }

        ObjectList::AddObject(lDefine);
    }

}
