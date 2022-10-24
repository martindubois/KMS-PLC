
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGY_Function.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "../Common/TRiLOGY/Function.h"

namespace TRiLOGY
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    Function::Function(unsigned int aIndex, unsigned int aLength) : Object(aIndex), mLength(aLength) {}

    // ===== Object =========================================================

    Function::~Function() {}

    void Function::GetLine(wchar_t* aOut, unsigned int aOutSize_byte) const
    {
        // TODO
    }

}
