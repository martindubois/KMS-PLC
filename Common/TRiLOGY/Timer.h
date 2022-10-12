
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/TRiLOGY/Sequencer.h

#pragma once

// ===== Local ==============================================================
#include "Object.h"

namespace TRiLOGY
{

    class Timer : public Object
    {

    public:

        Timer(const char* aName, unsigned int aIndex, unsigned int aLineNo, unsigned int aInit);

        unsigned int GetInit () const;

        // ===== Object =====================================================
        ~Timer();

    private:

        unsigned int mInit;

    };

}
