
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI_CounterList.cpp

// TEST COVERAGE  2023-08-15  KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== Local ==============================================================
#include "../Common/TRiLOGI/CounterList.h"

#include "TRiLOGI/Counter.h"

using namespace KMS;

namespace TRiLOGI
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    CounterList::CounterList() : ObjectList("counter", 256) {}

    // Protected
    // //////////////////////////////////////////////////////////////////////

    // ===== ObjectList =====================================================

    void CounterList::AddObject(const wchar_t* aLine, unsigned int)
    {
        assert(nullptr != aLine);

        unsigned int lIndex;
        unsigned int lInit;
        char         lName[NAME_LENGTH];

        auto lRet = swscanf_s(aLine, L"%u,%S %u", &lIndex, lName SizeInfo(lName), &lInit);
        KMS_EXCEPTION_ASSERT(3 == lRet, RESULT_INVALID_FORMAT, "Invalid timer line (NOT TESTED)", "");

        auto lCounter = new Counter(lName, lIndex, lInit);

        ObjectList::AddObject(lCounter);
    }

    bool CounterList::Import(const char* aName, unsigned int aInit)
    {
        auto lObject = FindObject_ByName(aName);
        if (nullptr == lObject)
        {
            if (IsProjectLegacy())
            {
                ::Console::Change("New counter (NOT TESTED) - ", aName);
            }

            auto lIndex = FindFreeIndex();

            auto lCounter = new Counter(aName, lIndex, aInit);

            ObjectList::AddObject(lCounter);
            return true;
        }

        // NOT TESTED
        auto lCounter = dynamic_cast<Counter*>(lObject);
        assert(nullptr != lCounter);

        return lCounter->SetInit(aInit);
    }

}
