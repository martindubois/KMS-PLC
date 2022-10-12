
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/TRiLOGY/TimerList.h

#pragma once

// ===== Local ==============================================================
#include "ObjectList.h"

namespace TRiLOGY
{

    class Timer;

    class TimerList : public ObjectList
    {

    public:

        TimerList();

        void Display_ByName (FILE* aOut) const;
        void Display_ByIndex(FILE* aOut) const;

    protected:

        virtual void AddObject(const wchar_t* aLine, unsigned int aLineNo);

    private:

        TimerList(const TimerList&);

        const TimerList& operator = (const TimerList&);

    };

}
