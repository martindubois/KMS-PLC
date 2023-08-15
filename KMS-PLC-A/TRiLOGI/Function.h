
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI/Function.h

#pragma once

// ===== Local ==============================================================
#include "../Function.h"

#include "Object.h"

namespace TRiLOGI
{

    class Function : public Object, public ::Function
    {

    public:

        Function(unsigned int aIndex, unsigned int aLength);

        Function(const char* aName, unsigned int aIndex, unsigned int aLineNo, unsigned int aFlags);

        unsigned int GetLineNo_Code_Begin() const;
        unsigned int GetLineNo_Code_End  () const;

        void SetLineNo_Code(unsigned int aBegin, unsigned int aEnd);

        void AddCodeToFile(KMS::Text::File_UTF16* aFile);

        void Apply(KMS::Text::File_UTF16* aFile);

        void Insert(KMS::Text::File_UTF16* aFile);

        void Parse(KMS::Text::File_ASCII* aFile, KMS::Text::File_ASCII::Internal::iterator* aIt);

        // ===== Object =====================================================
        virtual ~Function();

    private:

        void Init();

        unsigned int mLength;
        unsigned int mLineNo_Code_Begin;
        unsigned int mLineNo_Code_End;

    };

}
