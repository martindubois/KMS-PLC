
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/PLC/Sequence.h

#pragma once

// ===== Local ==============================================================
#include "../Common/PLC/Element.h"

namespace PLC
{

    class Sequence final : public Element
    {
    
    public:

        static const uint16_t INVALID_VALUE = 0xffff;

        Sequence();

        uint16_t GetValue() const;

        void SetIndexNameAndValue(const std::smatch& aMatch);

        void SetNameAndValue(const std::smatch& aMatch);

    private:

        uint16_t mValue;

    };

    typedef std::map<unsigned int, Sequence> Sequence_Map;

    // Public
    // //////////////////////////////////////////////////////////////////////

    inline Sequence::Sequence() : mValue(INVALID_VALUE) {}

    inline void Sequence::SetIndexNameAndValue(const std::smatch& aMatch)
    {
        SetIndexAndName(aMatch);

        mValue = KMS::Convert::ToUInt16(aMatch[3].str().c_str());
    }

    inline void Sequence::SetNameAndValue(const std::smatch& aMatch)
    {
        SetName(aMatch);

        mValue = KMS::Convert::ToUInt16(aMatch[2].str().c_str());
    }

    inline uint16_t Sequence::GetValue() const { return mValue; }

}
