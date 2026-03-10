
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      EBPro_Convert_Address/EBPro_Convert_Address.cpp

#include <KMS/Base.h>

// ===== Import/Includes ====================================================
#include <KMS/Banner.h>
#include <KMS/Exception.h>

// ===== Local ==============================================================
#include "../Common/Version.h"

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

static void DisplayUsage();

// Entry point
// //////////////////////////////////////////////////////////////////////////

int main(int aCount, const char** aVector)
{
    KMS_BANNER("KMS-PLC", "EBPro_Convert_Address");

    assert(nullptr != aVector);

    int lResult = 0;

    try
    {
    }
    KMS_CATCH_RESULT(lResult);

    return 0;
}

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

static void DisplayUsage()
{
    std::cout << "Usage: EBPro_Convert_Address.exe < EBPro_Exported.csv" << std::endl;
}
