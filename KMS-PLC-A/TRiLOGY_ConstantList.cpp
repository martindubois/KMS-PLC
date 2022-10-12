
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGY_ConstantList.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "../Common/TRiLOGY/Constant.h"

#include "../Common/TRiLOGY/ConstantList.h"

namespace TRiLOGY
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    Object* ConstantList::AddConstant(const char* aName, unsigned int aIndex, unsigned int aLineNo, const char* aValue)
    {
        assert(NULL != aName);

        Constant* lResult = new Constant(aName, aIndex, aLineNo, aValue);

        std::pair<ByName::iterator, bool> lBN = mConstants_ByName.insert(ByName::value_type(aName, lResult));
        KMS_EXCEPTION_ASSERT(lBN.second, APPLICATION, "A constant with same name already exist", aName);

        return lResult;
    }

    unsigned int ConstantList::GetCount() const { return static_cast<unsigned int>(mConstants_ByName.size()); }

    void ConstantList::Verify() const
    {
        for (const ByName::value_type& lVT : mConstants_ByName)
        {
            assert(NULL != lVT.second);

            lVT.second->Verify();
        }
    }

}
