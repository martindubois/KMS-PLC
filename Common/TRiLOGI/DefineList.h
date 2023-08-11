
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/TRiLOGI/DefineList.h
// Status    TEST

#pragma once

// ===== Local ==============================================================
#include "ConstantList.h"
#include "ObjectList.h"
#include "WordList.h"

namespace TRiLOGI
{

    class DefineList : public ObjectList
    {

    public:

        DefineList();

        void ClearList();

        void Display_ByIndex(FILE* aOut) const;
        void Display_ByName (FILE* aOut) const;

        bool ImportConstant(const char* aName, const char* aValue);

        bool ImportWord(const char* aName);
        bool ImportWord(const char* aName, unsigned int aOffset);

        unsigned int Parse(KMS::Text::File_UTF16* aFile_PC6, unsigned int aLineNo);

        void Verify(const KMS::Text::File_UTF16& aFile_PC6);

        WordList mWords;

    protected:

        // ===== ObjectList =================================================
        virtual void AddObject(const wchar_t* aLine, unsigned int aLineNo, unsigned int aFlags);

    private:

        NO_COPY(DefineList);

        ConstantList mConstants;

    };

}
