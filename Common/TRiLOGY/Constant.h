
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// Product   KMS-PLC
// File      Common/TRiLOGY/Constant.h

#pragma once

// ====== Local =============================================================
#include "Object.h"

namespace TRiLOGY
{

    class Constant : public Object
    {

    public:

        Constant(const char* aName, unsigned int aIndex, unsigned int aLineNo, const char* aValue);

        const char* GetValue() const;

        void Verify();

        // ===== Object =====================================================
        ~Constant();

    private:

        std::string mValue;

    };

}
