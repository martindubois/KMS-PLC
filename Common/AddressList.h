
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/AddressList.h
// Status    PROD_READY

#pragma once

// ===== C++ ================================================================
#include <list>

// ===== Local ==============================================================
#include "Address.h"

class AddressList : public std::list<Address>
{

public:

    void Add(const char* aName, AddressType aType, uint16_t aAddr);

    bool DoesContain(const char* aName) const;

};
