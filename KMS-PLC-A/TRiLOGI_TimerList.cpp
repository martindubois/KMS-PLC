
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI_TimerList.cpp

// TEST COVERAGE  2023-09-14  KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== Local ==============================================================
#include "../Common/TRiLOGI/TimerList.h"

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
        if (nullptr == lObject)
        {
            if (IsProjectLegacy())
            {
                ::Console::Change("New timer (NOT TESTED)", aName);
            }

            auto lIndex = FindFreeIndex();

            auto lTimer = new Timer(aName, lIndex, aInit);

            ObjectList::AddObject(lTimer);

            return true;
        }

        // NOT TESTED
        auto lTimer = dynamic_cast<Timer*>(lObject);
        assert(nullptr != lTimer);

        return lTimer->SetInit(aInit);
    }

    // Protected
    // //////////////////////////////////////////////////////////////////////

    // ===== ObjectList =====================================================

    void TimerList::AddObject(const wchar_t* aLine, unsigned int)
    {
        assert(nullptr != aLine);

        unsigned int lIndex;
        unsigned int lInit;
        char         lName[NAME_LENGTH];

        auto lRet = swscanf_s(aLine, L"%u,%S %u", &lIndex, lName SizeInfo(lName), &lInit);
        KMS_EXCEPTION_ASSERT(3 == lRet, RESULT_INVALID_FORMAT, "Invalid timer line", lRet);

        auto lTimer = new Timer(lName, lIndex, lInit);

        ObjectList::AddObject(lTimer);
    }

}
