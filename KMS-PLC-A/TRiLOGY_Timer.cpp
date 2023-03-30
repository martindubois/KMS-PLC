
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGY_Timer.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "../Common/TRiLOGY/Timer.h"

using namespace KMS;

namespace TRiLOGY
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    Timer::Timer(const char* aName, unsigned int aIndex, unsigned int aLineNo, unsigned int aInit, unsigned int aFlags)
        : Object(aName, aIndex, aLineNo, aFlags | FLAG_SINGLE_USE_WARNING), mInit(aInit)
    {
        assert(NULL != aName);
    }

    bool Timer::SetInit(unsigned int aInit, Text::File_UTF16* aFile_PC6)
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

    Timer::~Timer() {}

    void Timer::GetLine(wchar_t* aOut, unsigned int aOutSize_byte) const
    {
        assert(NULL != aOut);

        swprintf_s(aOut SizeInfoV(aOutSize_byte / sizeof(wchar_t)), L"%u:%S:%u", GetIndex(), GetName(), mInit);
    }

}
