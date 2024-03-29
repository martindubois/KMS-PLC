
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI_Counter.cpp

// TEST COVERAGE  2023-08-15  KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== Local ==============================================================
#include "TRiLOGI/Counter.h"

using namespace KMS;

namespace TRiLOGI
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    Counter::Counter(const char* aName, unsigned int aIndex, unsigned int aInit)
        : Object(aName, aIndex, FLAG_SINGLE_USE_WARNING), mInit(aInit)
    {}

    bool Counter::SetInit(unsigned int aInit)
    {
        auto lResult = mInit != aInit;
        if (lResult)
        {
            ::Console::Change("Counter changed (NOT TESTED)", GetName());

            mInit = aInit;
        }

        return lResult;
    }

    // ===== Object =========================================================

    Counter::~Counter() {}

    void Counter::GetLine(wchar_t* aOut, unsigned int aOutSize_byte) const
    {
        assert(nullptr != aOut);

        swprintf_s(aOut SizeInfoV(aOutSize_byte / sizeof(wchar_t)), L"%u,%S %u", GetIndex(), GetName(), mInit);
    }

}
