
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGY_SequencerList.cpp

#include "Component.h"

// ===== C++ ================================================================
#include <codecvt>

// ===== Import/Includes ====================================================
#include <KMS/Console/Color.h>

// ===== Local ==============================================================
#include "../Common/TRiLOGY/Sequencer.h"

#include "../Common/TRiLOGY/SequencerList.h"

namespace TRiLOGY
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    SequencerList::SequencerList() : ObjectList("sequencer", L"~\r") {}

    // Protected
    // //////////////////////////////////////////////////////////////////////

    void SequencerList::AddObject(const wchar_t* aLine, unsigned int aLineNo)
    {
        assert(NULL != aLine);

        unsigned int lIndex;
        unsigned int lInit;
        char         lName[NAME_LENGTH];

        int lRet = swscanf_s(aLine, L"%u,%S %u", &lIndex, lName SizeInfo(lName), &lInit);
        KMS_EXCEPTION_ASSERT(3 == lRet, APPLICATION, "Invalid timer line", lRet);

        Sequencer* lSequencer = new Sequencer(lName, lIndex, aLineNo, lInit);

        ObjectList::AddObject(lSequencer);
    }

}
