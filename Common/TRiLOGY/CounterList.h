
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/TRiLOGY/CounterList.h

#pragma once

// ===== Local ==============================================================
#include "ObjectList.h"

namespace TRiLOGY
{

    class Counter;

    class CounterList : public ObjectList
    {

    public:

        CounterList();

        void Display_ByIndex(FILE* aOut) const;
        void Display_ByName (FILE* aOut) const;

        bool Import(const char* aName, unsigned int aInit);

    protected:

        // ===== ObjectList =================================================
        virtual void AddObject(const wchar_t* aLine, unsigned int aLineNo, unsigned int aFlags);

    private:

        CounterList(const CounterList&);

        const CounterList& operator = (const CounterList&);

    };

}
