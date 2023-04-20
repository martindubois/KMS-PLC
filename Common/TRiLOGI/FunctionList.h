
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/TRiLOGI/FunctionList.h

#pragma once

// ===== Import/Includes ====================================================
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

        void AddFunction(Function* aFunction);

        void Clear();

        unsigned int SetEndLine_Code (unsigned int aEndLine);

        void Display_ByIndex(FILE* aOut) const;
        void Display_ByName (FILE* aOut) const;

        Function* Find_ByIndex(unsigned int aIndex);

        unsigned int Parse_Code(const KMS::Text::File_UTF16& aFile_PC6, unsigned int aLineNo);
        unsigned int Parse_Name(const KMS::Text::File_UTF16& aFile_PC6, unsigned int aLineNo);

        void Verify(const KMS::Text::File_UTF16& aFile_PC6);

    private:

        NO_COPY(FunctionList);

        unsigned int mLineNo_End_Code;

    };

}
