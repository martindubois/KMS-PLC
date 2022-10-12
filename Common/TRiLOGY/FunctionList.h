
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// Product   KMS-PLC
// File      Common/TRiLOGY/FunctionList.h

#pragma once

// ===== Import/Includes ====================================================
#include <KMS/Text/File_UTF16.h>

// ===== Local ==============================================================
#include "Function.h"
#include "ObjectList.h"

namespace TRiLOGY
{

    class FunctionList : public ObjectList
    {

    public:

        FunctionList();

        void AddFunction(Function* aFunction);

        unsigned int SetEndLine_Code (unsigned int aEndLine);

        void Display_ByIndex(FILE* aOut) const;
        void Display_ByName (FILE* aOut) const;

        Function* Find_ByIndex(unsigned int aIndex);

        unsigned int Parse_Code(const KMS::Text::File_UTF16& aFile_PC6, unsigned int aLineNo);
        unsigned int Parse_Name(const KMS::Text::File_UTF16& aFile_PC6, unsigned int aLineNo);

        void Verify(const KMS::Text::File_UTF16& aFile_PC6);

    private:

        FunctionList(const FunctionList&);

        const FunctionList& operator = (const FunctionList&);

        unsigned int mLineNo_End_Code;

    };

}
