
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGY_BitList.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "../Common/TRiLOGY/Object.h"

#include "../Common/TRiLOGY/BitList.h"

namespace TRiLOGY
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    BitList::BitList(const char* aElementName, unsigned int aStartAddress)
        : ObjectList(aElementName, L"~\r"), mStartAddress(aStartAddress)
    {}

    void BitList::GetAddresses(const std::regex& aRegEx, AddressList* aOut) const
    {
        assert(NULL != aOut);

        for (ByIndex::value_type lVT : mObjects_ByIndex)
        {
            Object* lBit = lVT.second;
            assert(NULL != lBit);

            if ((!lBit->TestFlag(Object::FLAG_NOT_USED)) && std::regex_match(lBit->GetName(), aRegEx))
            {
                aOut->push_back(Address(lBit->GetName(), AddressType::MODBUS_RTU_1X, mStartAddress + lBit->GetIndex()));
            }
        }
    }

}
