
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// Product   KMS-PLC
// File      Common/TRiLOGY/Function.h

#pragma once

// ===== Local ==============================================================
#include "Object.h"

namespace TRiLOGY
{

    class Function : public Object
    {

    public:

        Function(unsigned int aIndex, unsigned int aLineNo);

        // ===== Object =====================================================
        ~Function();

    private:

        unsigned int mLength;
        unsigned int mLineNo_Code_Begin;
        unsigned int mLineNo_Code_End;

    };

}
