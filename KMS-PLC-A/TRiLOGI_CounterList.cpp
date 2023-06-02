
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI_CounterList.cpp

#include "Component.h"

// ===== C++ ================================================================
#include <codecvt>

// ===== Import/Includes ====================================================
#include <KMS/Console/Color.h>

// ===== Local ==============================================================
#include "../Common/TRiLOGI/CounterList.h"

#include "TRiLOGI/Counter.h"

namespace TRiLOGI
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    CounterList::CounterList() : ObjectList("counter", 256) {}

    // Protected
    // //////////////////////////////////////////////////////////////////////

    // ===== ObjectList =====================================================

    void CounterList::AddObject(const wchar_t* aLine, unsigned int aLineNo, unsigned int aFlags)
    {
        assert(NULL != aLine);

        unsigned int lIndex;
        unsigned int lInit;
        char         lName[NAME_LENGTH];

        auto lRet = swscanf_s(aLine, L"%u,%S %u", &lIndex, lName SizeInfo(lName), &lInit);

        char lMsg[64];
        sprintf_s(lMsg, "Line %u  Invalid timer line", aLineNo);
        KMS_EXCEPTION_ASSERT(3 == lRet, APPLICATION_ERROR, lMsg, lRet);

        auto lCounter = new Counter(lName, lIndex, aLineNo, lInit, aFlags);

        ObjectList::AddObject(lCounter);
    }

    bool CounterList::Import(const char* aName, unsigned int aInit)
    {
        Counter* lCounter;

        auto lObject = FindObject_ByName(aName);
        if (NULL == lObject)
        {
            unsigned int lIndex;
            unsigned int lLineNo;

            FindIndexAndLineNo(&lIndex, &lLineNo);

            lCounter = new Counter(aName, lIndex, lLineNo, aInit, Object::FLAG_TO_INSERT);

            ObjectList::AddObject(lCounter);
            return true;
        }

        lCounter = dynamic_cast<Counter*>(lObject);
        assert(NULL != lCounter);

        return lCounter->SetInit(aInit, GetFile_PC6());
    }

}