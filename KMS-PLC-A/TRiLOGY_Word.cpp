
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGY_Word.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "../Common/TRiLOGY/Word.h"

namespace TRiLOGY
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    Word::Word(const char* aName, unsigned int aIndex, unsigned int aLineNo, unsigned int aOffset)
        : Object(aName, aIndex, aLineNo), mOffset(aOffset)
    {
        assert(NULL != aName);
    }

    unsigned int Word::GetOffset() const { return mOffset; }

    // ===== Object =========================================================
    Word::~Word() {}

}
