
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

    void ConstantList::AddConstant(Constant* aConstant)
    {
        assert(NULL != aConstant);

        std::pair<ByName::iterator, bool> lBN = mConstants_ByName.insert(ByName::value_type(aConstant->GetName(), aConstant));
        KMS_EXCEPTION_ASSERT(lBN.second, APPLICATION_ERROR, "A constant with same name already exist", aConstant->GetName());
    }

    void ConstantList::Clear() { mConstants_ByName.clear(); }

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
