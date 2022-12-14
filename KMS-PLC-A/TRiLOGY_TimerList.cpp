
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

    TimerList::TimerList() : ObjectList("timer", L"~\r", 128) {}

    // Protected
    // //////////////////////////////////////////////////////////////////////

    void TimerList::AddObject(const wchar_t* aLine, unsigned int aLineNo)
    {
        assert(NULL != aLine);

        unsigned int lIndex;
        unsigned int lInit;
        char         lName[NAME_LENGTH];

        int lRet = swscanf_s(aLine, L"%u,%S %u", &lIndex, lName SizeInfo(lName), &lInit);
        KMS_EXCEPTION_ASSERT(3 == lRet, APPLICATION_ERROR, "Invalid timer line", lRet);

        Timer* lTimer = new Timer(lName, lIndex, aLineNo, lInit, 0);

        ObjectList::AddObject(lTimer);
    }

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

}
