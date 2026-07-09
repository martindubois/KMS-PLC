
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      EBPro_Build_Address/EBPro_Build_Address.cpp

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KMS/Banner.h>

// ===== Local ==============================================================
#include "../Common/Version.h"

#include "Builder_Address.h"

// Entry point
// //////////////////////////////////////////////////////////////////////////

int main(int aCount, const char** aVector)
{
    KMS_BANNER("KMS-PLC", "EBPro_Build_Address");

    assert(nullptr != aVector);

    int lResult = 0;

    try
    {
        Builder_Address lBuilder;

        lBuilder.AddSources(aCount - 1, aVector + 1);

        lBuilder.Build();
    }
    KMS_CATCH_RESULT(lResult);

    return 0;
}
