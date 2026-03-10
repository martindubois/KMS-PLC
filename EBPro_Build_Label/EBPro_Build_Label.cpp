
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      EBPro_Build_Label/EBPro_Build_Label.cpp

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

static void DisplayUsage();

// Entry point
// //////////////////////////////////////////////////////////////////////////

int main(int aCount, const char** aVector)
{
    KMS_BANNER("KMS-PLC", "EBPro_Build_Label");

    assert(nullptr != aVector);

    int lResult = 0;

    try
    {
        Builder lBuilder;

        switch (aCount)
        {
        case 1:
            lBuilder.Build();
            break;

        default:
            std::cout << Console::Color::RED;
            std::cout << "Invalid commmand line\n";
            std::cout << Console::Color::WHITE;
            std::cout << std::endl;
            DisplayUsage();
            lResult = __LINE__;
        }
    }
    KMS_CATCH_RESULT(lResult);

    return 0;
}

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

static void DisplayUsage()
{
    std::cout << "Usage: EBPro_Build_Label.exe" << std::endl;
}
