
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      TriLogi_Build/TriLogi_Build.cpp

#include <KMS/Base.h>

// ===== Import/Includes ====================================================
#include <KMS/Banner.h>
#include <KMS/Exception.h>

// ===== Local ==============================================================
#include "../Common/Version.h"

// Configurations
// //////////////////////////////////////////////////////////////////////////

#define TO_COMPILE_PC6 "TriLogi_ToCompile.pc6"

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

static void DisplayUsage();

// Entry point
// //////////////////////////////////////////////////////////////////////////

int main(int aCount, const char** aVector)
{
    KMS_BANNER("KMS-PLC", "TriLogi_Build");

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
    std::cout << "Usage: TriLogi_Build.exe [SourceFiles]" << std::endl;
}
