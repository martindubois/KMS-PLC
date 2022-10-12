
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGY_Sequencer.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "../Common/TRiLOGY/Sequencer.h"

namespace TRiLOGY
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    Sequencer::Sequencer(const char* aName, unsigned int aIndex, unsigned int aLineNo, unsigned int aInit)
        : Object(aName, aIndex, aLineNo), mInit(aInit)
    {
        assert(NULL != aName);
    }

    // ===== Object =========================================================
    Sequencer::~Sequencer() {}

}
