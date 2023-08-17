
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/TRiLOGI/BitList.h
// Status    DEV

#pragma once

// ===== C ==================================================================
#include <stdio.h>

// ===== Local ==============================================================
#include "../Address.h"
#include "../AddressList.h"

#include "ObjectList.h"

namespace TRiLOGI
{

    class BitList : public ObjectList
    {

    public:

        BitList(const char* aElementName, unsigned int aStartAddress, unsigned int aMaxQty);

        void GetAddresses(const std::regex& aRegEx, AddressList* aOut) const;

        void Display_ByName (FILE* aOut, unsigned int aModbusBaseAddress) const;
        void Display_ByIndex(FILE* aOut, unsigned int aModbusBaseAddress) const;

        bool Import(const char* aName);
        bool Import(const char* aName, unsigned int aIndex);

    private:

        NO_COPY(BitList);

        unsigned int mStartAddress;

    };

}
