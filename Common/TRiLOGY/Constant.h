
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/TRiLOGY/Constant.h

#pragma once

// ====== Import/Includes ===================================================
#include <KMS/Text/File_UTF16.h>

// ====== Local =============================================================
#include "Object.h"

namespace TRiLOGY
{

    class Constant : public Object
    {

    public:

        Constant(const char* aName, unsigned int aIndex, unsigned int aLineNo, const char* aValue, const char* aComment, unsigned int aFlags);

        const char* GetValue() const;

        bool SetValue(const char* aValue, KMS::Text::File_UTF16* aFile_PC6);

        void Verify();

        // ===== Object =====================================================
        virtual ~Constant();
        virtual void GetLine(wchar_t* aOut, unsigned int aOutSize_byte) const;

    private:

        std::string mComment;
        std::string mValue;

    };

}
