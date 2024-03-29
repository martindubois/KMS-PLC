
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI_Timer.cpp

// TEST COVERAGE  2023-08-08  KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== Local ==============================================================
#include "TRiLOGI/Timer.h"

using namespace KMS;

namespace TRiLOGI
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    Timer::Timer(const char* aName, unsigned int aIndex, unsigned int aInit)
        : Object(aName, aIndex, FLAG_SINGLE_USE_WARNING), mInit(aInit)
    {}

    bool Timer::SetInit(unsigned int aInit)
    {
        bool lResult = mInit != aInit;
        if (lResult)
        {
            ::Console::Change("Timer changed (NOT TESTED)", GetName());

            mInit = aInit;
        }

        return lResult;
    }

    // ===== Object =========================================================

    Timer::~Timer() {}

    void Timer::GetLine(wchar_t* aOut, unsigned int aOutSize_byte) const
    {
        assert(nullptr != aOut);

        swprintf_s(aOut SizeInfoV(aOutSize_byte / sizeof(wchar_t)), L"%u,%S %u", GetIndex(), GetName(), mInit);
    }

}
