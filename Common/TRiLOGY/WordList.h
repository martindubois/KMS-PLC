
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// Product   KMS-PLC
// File      Common/TRiLOGY/WordList.h

#pragma once

// ===== C++ ================================================================
#include <map>
#include <regex>
#include <string>

// ===== Local ==============================================================
#include "../Address.h"

namespace TRiLOGY
{

    class Object;
    class Word;

    class WordList
    {

    public:

        WordList();

        Object* AddWord(const char* aName, unsigned int aIndex, unsigned int aLineNo, unsigned int aOffset);

        void GetAddresses(const std::regex& aRegEx, AddressList* aOut) const;

        unsigned int GetCount() const;

        void Display_ByName  (FILE* aOut) const;
        void Display_ByOffset(FILE* aOut) const;

        const Word* Find_ByOffset(unsigned int aOffset) const;

        void Verify() const;

    private:

        typedef std::map<std::string , Word*> ByName;
        typedef std::map<unsigned int, Word*> ByOffset;

        WordList(const WordList&);

        const WordList& operator = (const WordList&);

        ByName   mWords_ByName;
        ByOffset mWords_ByOffset;

    };

}
