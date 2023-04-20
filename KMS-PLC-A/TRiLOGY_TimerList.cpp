
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGY_TimerList.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "../Common/TRiLOGY/TimerList.h"

#include "TRiLOGY/Timer.h"

namespace TRiLOGY
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    TimerList::TimerList() : ObjectList("timer", L"~\r", 128) {}

    bool TimerList::Import(const char* aName, unsigned int aInit)
    {
        Object* lObject = FindObject_ByName(aName);
        if (NULL == lObject)
        {
            unsigned int lIndex;
            unsigned int lLineNo;

            FindIndexAndLineNo(&lIndex, &lLineNo);

            Timer* lTimer = new Timer(aName, lIndex, lLineNo, aInit, Object::FLAG_TO_INSERT);

            ObjectList::AddObject(lTimer);

            return true;
        }

        Timer* lTimer = dynamic_cast<Timer*>(lObject);
        assert(NULL != lTimer);

        return lTimer->SetInit(aInit, GetFile_PC6());
    }

    // Protected
    // //////////////////////////////////////////////////////////////////////

    // ===== ObjectList =====================================================

    void TimerList::AddObject(const wchar_t* aLine, unsigned int aLineNo, unsigned int aFlags)
    {
        assert(NULL != aLine);

        unsigned int lIndex;
        unsigned int lInit;
        char         lName[NAME_LENGTH];

        int lRet = swscanf_s(aLine, L"%u,%S %u", &lIndex, lName SizeInfo(lName), &lInit);

        char lMsg[64];
        sprintf_s(lMsg, "Line %u  Invalid timer line", aLineNo);
        KMS_EXCEPTION_ASSERT(3 == lRet, APPLICATION_ERROR, lMsg, lRet);

        Timer* lTimer = new Timer(lName, lIndex, aLineNo, lInit, aFlags);

        ObjectList::AddObject(lTimer);
    }

}
