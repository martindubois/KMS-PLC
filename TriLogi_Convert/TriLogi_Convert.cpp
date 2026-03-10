
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      TriLogi_Convert/TriLogi_Convert.cpp

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
    KMS_BANNER("KMS-PLC", "TriLogi_Convert");

    assert(nullptr != aVector);

    int lResult = 0;

    try
    {
    }
    KMS_CATCH_RESULT(lResult);

    return lResult;
}

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

static void DisplayUsage()
{
    std::cout << "Usage: TriLogi_Convert.exe < TriLogi.pc6" << std::endl;
}
