
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      PLC/Element.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "../Common/PLC/Element.h"

namespace PLC
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    Element::Element(const std::smatch& aMatch)
        : mIndex(KMS::Convert::ToUInt16(aMatch[1].str().c_str()))
        , mName(aMatch[2].str())
    {}

    void Element::GetName(wchar_t* aOut, unsigned int aOutSize_byte) const
    {
        assert(nullptr != aOut);
        assert(sizeof(wchar_t) < aOutSize_byte);

        auto lRet = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, mName.c_str(), -1, aOut, aOutSize_byte / sizeof(wchar_t));
        assert(0 < lRet);
    }

}
