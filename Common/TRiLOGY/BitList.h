
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/TRiLOGY/BitList.h

#pragma once

// ===== C ==================================================================
#include <stdio.h>

// ===== Local ==============================================================
#include "../Address.h"
#include "ObjectList.h"

namespace TRiLOGY
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

        BitList(const BitList&);

        const BitList& operator = (const BitList&);

        unsigned int mStartAddress;

    };

}
