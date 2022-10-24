
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
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
        virtual ~Function();
        virtual void GetLine(wchar_t* aOut, unsigned int aOutSize_byte) const;

    private:

        unsigned int mLength;
        unsigned int mLineNo_Code_Begin;
        unsigned int mLineNo_Code_End;

    };

}
