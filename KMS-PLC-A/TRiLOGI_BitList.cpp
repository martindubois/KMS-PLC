
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI_BitList.cpp

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
        assert(NULL != aOut);

        for (auto lVT : mObjects_ByIndex)
        {
            auto lBit = lVT.second;
            assert(NULL != lBit);

            if ((!lBit->TestFlag(Object::FLAG_NOT_USED)) && std::regex_match(lBit->GetName(), aRegEx))
            {
                aOut->push_back(Address(lBit->GetName(), AddressType::MODBUS_RTU_1X, mStartAddress + lBit->GetIndex()));
            }
        }
    }

    bool BitList::Import(const char* aName)
    {
        auto lObject = FindObject_ByName(aName);
        bool lResult = NULL == lObject;
        if (lResult)
        {
            ::Console::Change("New", GetElementName(), aName);

            unsigned int lIndex;
            unsigned int lLineNo;

            FindIndexAndLineNo(&lIndex, &lLineNo);

            lObject = new Object(aName, lIndex, lLineNo, Object::FLAG_TO_INSERT);

            AddObject(lObject);
        }

        return lResult;
    }

    bool BitList::Import(const char* aName, unsigned int aIndex)
    {
        auto lObject = FindObject_ByName(aName);
        if (NULL == lObject)
        {
            lObject = FindObject_ByIndex(aIndex);
            if (NULL == lObject)
            {
                ::Console::Change("New", GetElementName(), aName);

                auto lLineNo = FindLineNo(aIndex);

                lObject = new Object(aName, aIndex, lLineNo, Object::FLAG_TO_INSERT);

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
                << "The name " << aName << " is already used for another index";
            ::Console::Error_End();
        }

        return false;
    }

}
