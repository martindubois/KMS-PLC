
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// Product   KMS-PLC
// File      Common/TRiLOGY/Word.h

#pragma once

// ====== Local =============================================================
#include "Object.h"

namespace TRiLOGY
{

    class Word : public Object
    {

    public:

        Word(const char* aName, unsigned int aIndex, unsigned int aLineNo, unsigned int aOffset);

        unsigned int GetOffset() const;

        // ===== Object =====================================================
        ~Word();

    private:

        unsigned int mOffset;

    };

}
