
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGY_ConstantList.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "../Common/TRiLOGY/ConstantList.h"

#include "TRiLOGY/Constant.h"

namespace TRiLOGY
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    void ConstantList::AddConstant(Constant* aConstant)
    {
        assert(NULL != aConstant);

        std::pair<ByName::iterator, bool> lBN = mConstants_ByName.insert(ByName::value_type(aConstant->GetName(), aConstant));

        char lMsg[64 + NAME_LENGTH];
        sprintf_s(lMsg, "A constant named \"%s\" already exist", aConstant->GetName());
        KMS_EXCEPTION_ASSERT(lBN.second, APPLICATION_ERROR, lMsg, "");
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
