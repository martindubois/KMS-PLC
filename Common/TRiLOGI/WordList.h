
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/TRiLOGI/WordList.h
// Status    TEST

#pragma once

// ===== C++ ================================================================
#include <map>
#include <regex>
#include <string>

// ===== Import =============================================================
#include <KMS/DI/Dictionary.h>
#include <KMS/DI/UInt.h>

// ===== Local ==============================================================
#include "../Address.h"
#include "../AddressList.h"

namespace TRiLOGI
{

    class Object;
    class Word;

    class WordList : public KMS::DI::Dictionary
    {

    public:

        WordList();

        TRiLOGI::Object* AddWord(const char* aName, unsigned int aIndex, const char* aComment);

        void AddWord(Word* aWord);

        void ClearList();

        void GetAddresses(const std::regex& aRegEx, AddressList* aOut) const;

        unsigned int GetCount() const;

        void Display_ByName  (FILE* aOut) const;
        void Display_ByOffset(FILE* aOut) const;

        const Word* Find_ByOffset(unsigned int aOffset) const;

        void SetProjectType(ProjectType aPT);

        void ValidateConfig() const;

        // ===== Configurable attributes ====================================
        KMS::DI::UInt<uint16_t> mOffsetMax;
        KMS::DI::UInt<uint16_t> mOffsetNew;

    private:

        typedef std::map<std::string , Word*> ByName;
        typedef std::map<unsigned int, Word*> ByOffset;

        NO_COPY(WordList);

        unsigned int FindFreeOffset_Dec();
        unsigned int FindFreeOffset_Inc();

        ProjectType mProjectType;

        ByName   mWords_ByName;
        ByOffset mWords_ByOffset;

    };

}
