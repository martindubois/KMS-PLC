
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC/KMS-PLC.cpp

#include <KMS/Base.h>

// ===== Import/Includes ====================================================
#include <KMS/Banner.h>

// ===== Common =============================================================
#include "../Common/Version.h"

#include "../Common/System.h" 

// Entry point
// //////////////////////////////////////////////////////////////////////////

int main(int aCount, const char** aVector)
{
    KMS_BANNER("KMS-PLC", "KMS-PLC");

    return System::Main(aCount, aVector);
}
