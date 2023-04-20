
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI/Word.h

#pragma once

// ====== Local =============================================================
#include "Object.h"

namespace TRiLOGI
{

    class Word : public Object
    {

    public:

        Word(const char* aName, unsigned int aIndex, unsigned int aLineNo, unsigned int aOffset, const char* aComment, unsigned int aFlags);

        unsigned int GetOffset() const;

        // ===== Object =====================================================
        virtual ~Word();
        virtual void GetLine(wchar_t* aOut, unsigned int aOutSize_byte) const;

    private:

        std::string  mComment;
        unsigned int mOffset;

    };

}
