
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

// ===== Local ==============================================================
#include "../Address.h"

namespace TRiLOGI
{

    class Object;
    class Word;

    class WordList
    {

    public:

        WordList();

        Object* AddWord(const char* aName, unsigned int aIndex, unsigned int aLineNo, const char* aComment, unsigned int aFlags);

        void AddWord(Word* aWord);

        void Clear();

        void GetAddresses(const std::regex& aRegEx, AddressList* aOut) const;

        unsigned int GetCount() const;

        void Display_ByName  (FILE* aOut) const;
        void Display_ByOffset(FILE* aOut) const;

        const Word* Find_ByOffset(unsigned int aOffset) const;

        void Verify() const;

    private:

        typedef std::map<std::string , Word*> ByName;
        typedef std::map<unsigned int, Word*> ByOffset;

        NO_COPY(WordList);

        ByName   mWords_ByName;
        ByOffset mWords_ByOffset;

    };

}
