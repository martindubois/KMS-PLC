
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/PLC/Define.h

#pragma once

// ===== Local ==============================================================
#include "../Common/PLC/Element.h"

namespace PLC
{

    class Define final : public Element
    {
    
    public:

        Define();

        void GetValue(wchar_t* aOut, unsigned int aOutSize_byte) const;

        void SetIndexNameAndValue(const std::smatch& aMatch);

        void SetNameAndValue(const std::smatch& aMatch);

    private:

        std::string mValue;

    };

    typedef std::list<Define>          Define_List;
    typedef std::map<uint16_t, Define> Define_Map;

}
