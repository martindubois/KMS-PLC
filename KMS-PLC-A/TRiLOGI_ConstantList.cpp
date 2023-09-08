
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI_ConstantList.cpp

// TEST COVERAGE  2023-08-08  KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== Local ==============================================================
#include "../Common/TRiLOGI/ConstantList.h"

#include "TRiLOGI/Constant.h"

KMS_RESULT_STATIC(RESULT_ALREADY_EXIST);

namespace TRiLOGI
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    void ConstantList::AddConstant(Constant* aConstant)
    {
        assert(nullptr != aConstant);

        auto lBN = mConstants_ByName.insert(ByName::value_type(aConstant->GetName(), aConstant));
        if (!lBN.second)
        {
            char lMsg[64 + NAME_LENGTH];
            sprintf_s(lMsg, "A constant named \"%s\" already exist (NOT TESTED)", aConstant->GetName());
            KMS_EXCEPTION(RESULT_ALREADY_EXIST, lMsg, "");
        }
    }

    void ConstantList::ClearList() { mConstants_ByName.clear(); }

    unsigned int ConstantList::GetCount() const { return static_cast<unsigned int>(mConstants_ByName.size()); }

    void ConstantList::Verify() const
    {
        for (auto& lVT : mConstants_ByName)
        {
            assert(nullptr != lVT.second);

            lVT.second->Verify();
        }
    }

}
