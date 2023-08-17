
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI_BitList.cpp

// TEST COVERAGE  2023-08-15  KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== Local ==============================================================
#include "../Common/TRiLOGI/BitList.h"

#include "TRiLOGI/Object.h"

using namespace KMS;

namespace TRiLOGI
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    BitList::BitList(const char* aElementName, unsigned int aStartAddress, unsigned int aMaxQty)
        : ObjectList(aElementName, aMaxQty), mStartAddress(aStartAddress)
    {}

    void BitList::GetAddresses(const std::regex& aRegEx, AddressList* aOut) const
    {
        assert(nullptr != aOut);

        for (auto lVT : mObjects_ByIndex)
        {
            auto lBit = lVT.second;
            assert(nullptr != lBit);

            auto lN = lBit->GetName();

            if ((!lBit->TestFlag(Object::FLAG_NOT_USED)) && std::regex_match(lN, aRegEx))
            {
                aOut->Add(lN, AddressType::MODBUS_RTU_1X, mStartAddress + lBit->GetIndex());
            }
        }
    }

    bool BitList::Import(const char* aName)
    {
        auto lObject = FindObject_ByName(aName);

        bool lResult = nullptr == lObject;
        if (lResult)
        {
            if (IsProjectLegacy())
            {
                ::Console::Change("New", GetElementName(), aName);
            }

            auto lIndex = FindFreeIndex();

            lObject = new Object(aName, lIndex, 0);

            AddObject(lObject);
        }

        return lResult;
    }

    bool BitList::Import(const char* aName, unsigned int aIndex)
    {
        assert(nullptr != aName);

        auto lObject = FindObject_ByName(aName);
        if (nullptr == lObject)
        {
            lObject = FindObject_ByIndex(aIndex);
            if (nullptr == lObject)
            {
                if (IsProjectLegacy())
                {
                    ::Console::Change("New", GetElementName(), aName);
                }

                lObject = new Object(aName, aIndex, 0);

                AddObject(lObject);
                return true;
            }

            ::Console::Error_Begin()
                << "The index " << aIndex << " is already used";
            ::Console::Error_End();
            return false;
        }

        if (aIndex != lObject->GetIndex())
        {
            ::Console::Error_Begin()
                << "The name " << aName << " is already used for another index (NOT TESTED)";
            ::Console::Error_End();
        }

        return false;
    }

}
