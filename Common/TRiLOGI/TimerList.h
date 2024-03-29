
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/TRiLOGI/TimerList.h
// Status    TEST

#pragma once

// ===== Local ==============================================================
#include "ObjectList.h"

namespace TRiLOGI
{

    class TimerList : public ObjectList
    {

    public:

        TimerList();

        void Display_ByName (FILE* aOut) const;
        void Display_ByIndex(FILE* aOut) const;

        bool Import(const char* aName, unsigned int aInit);

    protected:

        // ===== ObjectList =================================================
        virtual void AddObject(const wchar_t* aLine, unsigned int aFlags);

    private:

        NO_COPY(TimerList);

    };

}
