
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI_TimerList.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "../Common/TRiLOGI/TimerList.h"

#include "Console.h"

#include "TRiLOGI/Timer.h"

using namespace KMS;

namespace TRiLOGI
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    TimerList::TimerList() : ObjectList("timer", 128) {}

    bool TimerList::Import(const char* aName, unsigned int aInit)
    {
        auto lObject = FindObject_ByName(aName);
        if (NULL == lObject)
        {
            ::Console::Change("New timer", aName);

            unsigned int lIndex;
            unsigned int lLineNo;

            FindIndexAndLineNo(&lIndex, &lLineNo);

            auto lTimer = new Timer(aName, lIndex, lLineNo, aInit, Object::FLAG_TO_INSERT);

            ObjectList::AddObject(lTimer);

            return true;
        }

        auto lTimer = dynamic_cast<Timer*>(lObject);
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

        auto lRet = swscanf_s(aLine, L"%u,%S %u", &lIndex, lName SizeInfo(lName), &lInit);

        char lMsg[64];
        sprintf_s(lMsg, "Line %u  Invalid timer line", aLineNo);
        KMS_EXCEPTION_ASSERT(3 == lRet, APPLICATION_ERROR, lMsg, lRet);

        auto lTimer = new Timer(lName, lIndex, aLineNo, lInit, aFlags);

        ObjectList::AddObject(lTimer);
    }

}
