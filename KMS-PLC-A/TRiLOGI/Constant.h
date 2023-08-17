
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI/Constant.h

#pragma once

// ====== Local =============================================================
#include "Object.h"

namespace TRiLOGI
{

    class Constant : public Object
    {

    public:

        Constant(const char* aName, unsigned int aIndex, const char* aValue, const char* aComment);

        const char* GetValue() const;

        bool SetValue(const char* aValue);

        void Verify();

        // ===== Object =====================================================
        virtual ~Constant();
        virtual void GetLine(wchar_t* aOut, unsigned int aOutSize_byte) const;

    private:

        std::string mComment;
        std::string mValue;

    };

}
