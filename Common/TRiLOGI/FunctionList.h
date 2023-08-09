
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/TRiLOGI/FunctionList.h
// Status    TEST

#pragma once

// ===== Import/Includes ====================================================
#include <KMS/Text/File_ASCII.h>
#include <KMS/Text/File_UTF16.h>

// ===== Local ==============================================================
#include "ObjectList.h"

namespace TRiLOGI
{

    class Function;

    class FunctionList : public ObjectList
    {

    public:

        FunctionList();

        void Clear();

        void SetLineNo_End_Code(unsigned int aLineNo);

        void Display_ByIndex(FILE* aOut) const;
        void Display_ByName (FILE* aOut) const;

        Function* Find_ByIndex(unsigned int aIndex);
        Function* Find_ByName (const char* aName);

        bool Apply_Code();

        bool Import(const char* aName, KMS::Text::File_ASCII* aFile, KMS::Text::File_ASCII::Internal::iterator* aIt);

        unsigned int Parse_Code(KMS::Text::File_UTF16* aFile_PC6, unsigned int aLineNo);
        unsigned int Parse_Name(KMS::Text::File_UTF16* aFile_PC6, unsigned int aLineNo);

        void Verify(const KMS::Text::File_UTF16& aFile_PC6);

    private:

        NO_COPY(FunctionList);

        unsigned int mLineNo_End_Code;

    };

}
