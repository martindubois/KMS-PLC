
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A-Test/KMS-PLC-A-Test.cpp

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KMS/Banner.h>
#include <KMS/Test/TestManager.h>

// ===== Local ==============================================================
#include "../Common/Version.h"

using namespace KMS;

// Entry point
// //////////////////////////////////////////////////////////////////////////

int main(int aCount, const char** aVector)
{
    KMS_BANNER("KMS-PLC", "KMS-PLC-A-Test");

    return Test::TestManager::Main(aCount, aVector);
}
