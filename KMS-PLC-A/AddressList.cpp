
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/AddressList.cpp

#include "Component.h"

// ===== Local ===============================================================
#include "../Common/AddressList.h"

// Public
// ///////////////////////////////////////////////////////////////////////////

void AddressList::Add(const char* aName, AddressType aType, uint16_t aAddr)
{
    push_back(Address(aName, aType, aAddr));
}

bool AddressList::DoesContain(const char* aName) const
{
    assert(nullptr != aName);

    auto lResult = false;

    for (auto& lAddr : *this)
    {
        if (0 == strcmp(lAddr.GetName(), aName))
        {
            lResult = true;
            break;
        }
    }

    return lResult;
}
