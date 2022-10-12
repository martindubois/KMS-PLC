
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
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

}
