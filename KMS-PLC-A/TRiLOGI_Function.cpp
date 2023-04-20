
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI_Function.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "TRiLOGI/Function.h"

namespace TRiLOGI
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
