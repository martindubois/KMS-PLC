
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

        Timer(const std::smatch& aMatch);

        uint16_t GetValue() const;

    private:

        uint16_t mValue;

    };

    typedef std::map<unsigned int, Timer> Timer_List;

    // Public
    // //////////////////////////////////////////////////////////////////////

    inline Timer::Timer(const std::smatch& aMatch)
        : Element(aMatch)
        , mValue(KMS::Convert::ToUInt16(aMatch[3].str().c_str()))
    {}

    inline uint16_t Timer::GetValue() const { return mValue; }

}
