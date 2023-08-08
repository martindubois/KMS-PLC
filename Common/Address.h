
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/System.h
// Status    PROD_READY

#pragma once

// ===== C++ ================================================================
#include <list>
#include <string>

// ===== Local ==============================================================
#include "Types.h"

class Address
{

public:

    Address(const char* aName, AddressType aType, uint16_t aAddr);

    unsigned int GetAddress() const;
    const char * GetName   () const;
    AddressType  GetType   () const;

private:

    unsigned int mAddr;
    std::string  mName;
    AddressType  mType;

};

typedef std::list<Address> AddressList;
