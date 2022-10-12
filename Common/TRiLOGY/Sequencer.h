
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// Product   KMS-PLC
// File      Common/TRiLOGY/Sequencer.h

#pragma once

// ===== Local ==============================================================
#include "Object.h"

namespace TRiLOGY
{

    class Sequencer : public Object
    {

    public:

        Sequencer(const char* aName, unsigned int aIndex, unsigned int aLineNo, unsigned int aInit);

        unsigned int GetInit () const;

        // ===== Object =====================================================
        ~Sequencer();

    private:

        unsigned int mInit;

    };

}
