
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI/Counter.h

#pragma once

// ===== Local ==============================================================
#include "Object.h"

namespace TRiLOGI
{

    class Counter : public Object
    {

    public:

        Counter(const char* aName, unsigned int aIndex, unsigned int aInit);

        unsigned int GetInit () const;

        bool SetInit(unsigned int aInit);

        // ===== Object =====================================================
        virtual ~Counter();
        virtual void GetLine(wchar_t* aOut, unsigned int aOutSize_byte) const;


    private:

        unsigned int mInit;

    };

}
