
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/TRiLOGY/SequencerList.h

#pragma once

// ===== Local ==============================================================
#include "ObjectList.h"

namespace TRiLOGY
{

    class Sequencer;

    class SequencerList : public ObjectList
    {

    public:

        SequencerList();

        void Display_ByIndex(FILE* aOut) const;
        void Display_ByName (FILE* aOut) const;

    protected:

        virtual void AddObject(const wchar_t* aLine, unsigned int aLineNo);

    private:

        SequencerList(const SequencerList&);

        const SequencerList& operator = (const SequencerList&);

    };

}
