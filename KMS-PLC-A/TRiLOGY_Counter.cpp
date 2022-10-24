
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGY_Counter.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "../Common/TRiLOGY/Counter.h"

namespace TRiLOGY
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    Counter::Counter(const char* aName, unsigned int aIndex, unsigned int aLineNo, unsigned int aInit, unsigned int aFlags)
        : Object(aName, aIndex, aLineNo, aFlags), mInit(aInit)
    {
        assert(NULL != aName);
    }

    bool Counter::SetInit(unsigned int aInit, KMS::Text::File_UTF16* aFile_PC6)
    {
        bool lResult = mInit != aInit;
        if (lResult)
        {
            mInit = aInit;

            Update(aFile_PC6);
        }

        return lResult;
    }

    // ===== Object =========================================================

    Counter::~Counter() {}

    void Counter::GetLine(wchar_t* aOut, unsigned int aOutSize_byte) const
    {
        wchar_t lLine[LINE_LENGTH];

        swprintf_s(lLine SizeInfoV(aOutSize_byte / sizeof(wchar_t)), L"%u,%S %u", GetIndex(), GetName(), mInit);
    }

}
