
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI/Timer.h

#pragma once

// ===== Local ==============================================================
#include "Object.h"

namespace TRiLOGI
{

    class Timer : public Object
    {

    public:

        Timer(const char* aName, unsigned int aIndex, unsigned int aLineNo, unsigned int aInit, unsigned int aFlags);

        unsigned int GetInit () const;

        bool SetInit(unsigned int aInit, KMS::Text::File_UTF16* aFile_PC6);

        // ===== Object =====================================================
        virtual ~Timer();
        virtual void GetLine(wchar_t* aOut, unsigned int aOutSize_byte) const;

    private:

        unsigned int mInit;

    };

}
