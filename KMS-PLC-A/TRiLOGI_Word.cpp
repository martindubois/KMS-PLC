
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI_Word.cpp

// TEST COVERAGE  2023-08-08  KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== Local ==============================================================
#include "TRiLOGI/Word.h"

namespace TRiLOGI
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    Word::Word(const char* aName, unsigned int aIndex, unsigned int aLineNo, unsigned int aOffset, const char* aComment, unsigned int aFlags)
        : Object(aName, aIndex, aLineNo, aFlags | FLAG_SINGLE_USE_INFO), mComment(aComment), mOffset(aOffset)
    {}

    unsigned int Word::GetOffset() const { return mOffset; }

    // ===== Object =========================================================

    Word::~Word() {}

    void Word::GetLine(wchar_t* aOut, unsigned int aOutSize_byte) const
    {
        assert(NULL != aOut);

        unsigned int lOutLen = aOutSize_byte / sizeof(wchar_t);

        if (mComment.empty())
        {
            swprintf_s(aOut SizeInfoV(lOutLen), L"%u,%S,DM[%u]", GetIndex(), GetName(), mOffset);
        }
        else
        {
            // NOT TESTED
            swprintf_s(aOut SizeInfoV(lOutLen), L"%u,%S,DM[%u],%S", GetIndex(), GetName(), mOffset, mComment.c_str());
        }
    }

}
