
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/Address.cpp

// TEST COVERAGE  2023-08-06  KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== Local ===============================================================
#include "../Common/Address.h"

// Public
// ///////////////////////////////////////////////////////////////////////////

Address::Address(const char* aName, AddressType aType, uint16_t aAddr)
    : mAddr(aAddr), mName(aName), mType(aType)
{
    assert(nullptr != aName);
}

unsigned int Address::GetAddress() const { return mAddr; }
const char * Address::GetName   () const { return mName.c_str(); }
AddressType  Address::GetType   () const { return mType; }
