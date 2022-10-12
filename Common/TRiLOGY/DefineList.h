
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// Product   KMS-PLC
// File      Common/TRiLOGY/DefineList.h

#pragma once

// ===== Local ==============================================================
#include "ConstantList.h"
#include "ObjectList.h"
#include "WordList.h"

namespace TRiLOGY
{

    class DefineList : public ObjectList
    {

    public:

        DefineList();

        void Display_ByIndex(FILE* aOut) const;
        void Display_ByName (FILE* aOut) const;

        unsigned int Parse(const KMS::Text::File_UTF16& aFile_PC6, unsigned int aLineNo);

        void Verify(const KMS::Text::File_UTF16& aFile_CP6);

        WordList mWords;

    protected:

        virtual void AddObject(const wchar_t* aLine, unsigned int aLineNo);

    private:

        DefineList(const DefineList&);

        const DefineList& operator = (const DefineList&);

        ConstantList mConstants;

    };

}
