
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      TriLogi_Build/TriLogi_Build.cpp

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KMS/Banner.h>
#include <KMS/Console/Color.h>

// ===== Local ==============================================================
#include "../Common/Version.h"

#include "Builder.h"

using namespace KMS;

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

static void DisplayError(const char* aMsg);
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
        Builder lBuilder;

        lBuilder.Build();

        switch (aCount)
        {
        case 1:
            break;

        case 0:
            DisplayError("Invalid commmand line");
            DisplayUsage();
            lResult = __LINE__;

        default:
            for (int i = 2; i < aCount; i++)
            {
                assert(nullptr != aVector[i]);

                lBuilder.ReadSource(aVector[i]);
            }
        }

        lBuilder.Write();
    }
    KMS_CATCH_RESULT(lResult);

    return 0;
}

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

void DisplayError(const char* aMsg)
{
    assert(nullptr != aMsg);

    std::cout << Console::Color::RED;
    {
        std::cout << aMsg << "\n";
    }
    std::cout << Console::Color::WHITE;
    std::cout << std::endl;
}

void DisplayUsage()
{
    std::cout << "Usage: TriLogi_Build.exe [SourceFiles]" << std::endl;
}
