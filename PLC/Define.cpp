
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      PLC/Define.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "../Common/PLC/Define.h"

namespace PLC
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    Define::Define(const std::smatch& aMatch)
        : Element(aMatch)
        , mValue(aMatch[3].str())
    {}

    void Define::GetValue(wchar_t* aOut, unsigned int aOutSize_byte) const
    {
        assert(nullptr != aOut);
        assert(sizeof(wchar_t) < aOutSize_byte);

        auto lRet = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, mValue.c_str(), -1, aOut, aOutSize_byte / sizeof(wchar_t));
        assert(0 < lRet);
    }

}
