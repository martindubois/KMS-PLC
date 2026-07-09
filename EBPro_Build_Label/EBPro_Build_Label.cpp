
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      EBPro_Build_Label/EBPro_Build_Label.cpp

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KMS/Banner.h>

// ===== Local ==============================================================
#include "../Common/Version.h"

#include "Builder_Label.h"

using namespace KMS;

// Entry point
// //////////////////////////////////////////////////////////////////////////

int main(int aCount, const char** aVector)
{
    KMS_BANNER("KMS-PLC", "EBPro_Build_Label");

    assert(nullptr != aVector);

    int lResult = 0;

    try
    {
        Builder_Label lBuilder;

        lBuilder.AddSources(aCount - 1, aVector + 1);

        lBuilder.Build();
    }
    KMS_CATCH_RESULT(lResult);

    return 0;
}
