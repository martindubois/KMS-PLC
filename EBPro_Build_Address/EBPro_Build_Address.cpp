
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      EBPro_Build_Address/EBPro_Build_Address.cpp

#include <KMS/Base.h>

// ===== Import/Includes ====================================================
#include <KMS/Banner.h>
#include <KMS/Exception.h>

// ===== Local ==============================================================
#include "../Common/Version.h"

// Configurations
// //////////////////////////////////////////////////////////////////////////

#define EBPRO_ADDRESSES_TO_IMPORT_CSV "EBPro_Addresses_ToImport.csv"

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

static void DisplayUsage();

// Entry point
// //////////////////////////////////////////////////////////////////////////

int main(int aCount, const char** aVector)
{
    KMS_BANNER("KMS-PLC", "EBPro_Build_Address");

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
    std::cout << "Usage: EBPro_Build_Address.exe [SourceFiles]" << std::endl;
}
