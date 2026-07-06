
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/PLC/Timer.h

#pragma once

// ===== Local ==============================================================
#include "../Common/PLC/Element.h"

namespace PLC
{

    class Timer final : public Element
    {
    
    public:

        static const uint16_t INVALID_VALUE = 0xffff;

        Timer();

        uint16_t GetValue() const;

        void SetIndexNameAndValue(const std::smatch& aMatch);

        void SetNameAndValue(const std::smatch& aMatch);

    private:

        uint16_t mValue;

    };

    typedef std::map<unsigned int, Timer> Timer_Map;

    // Public
    // //////////////////////////////////////////////////////////////////////

    inline Timer::Timer() : mValue(INVALID_VALUE) {}

    inline void Timer::SetIndexNameAndValue(const std::smatch& aMatch)
    {
        SetIndexAndName(aMatch);

        mValue = KMS::Convert::ToUInt16(aMatch[3].str().c_str());
    }

    inline void Timer::SetNameAndValue(const std::smatch& aMatch)
    {
        SetName(aMatch);

        mValue = KMS::Convert::ToUInt16(aMatch[2].str().c_str());
    }

    inline uint16_t Timer::GetValue() const { return mValue; }

}
