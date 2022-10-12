
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGY_TimerList.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "../Common/TRiLOGY/Timer.h"

#include "../Common/TRiLOGY/TimerList.h"

namespace TRiLOGY
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    TimerList::TimerList() : ObjectList("timer", L"~\r") {}

    // Protected
    // //////////////////////////////////////////////////////////////////////

    void TimerList::AddObject(const wchar_t* aLine, unsigned int aLineNo)
    {
        assert(NULL != aLine);

        unsigned int lIndex;
        unsigned int lInit;
        char         lName[NAME_LENGTH];

        int lRet = swscanf_s(aLine, L"%u,%S %u", &lIndex, lName SizeInfo(lName), &lInit);
        KMS_EXCEPTION_ASSERT(3 == lRet, APPLICATION, "Invalid timer line", lRet);

        Timer* lTimer = new Timer(lName, lIndex, aLineNo, lInit);

        ObjectList::AddObject(lTimer);
    }

}
