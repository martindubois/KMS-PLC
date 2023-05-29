
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A-Test/System.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "../Common/System.h"

KMS_TEST(System_Main, "System_Main", "Auto", sTest_Main)
{
    const char* lVector[2] = { "KMS-PLC-A-Test.exe", "ConfigFiles+=Tests\\Test0\\KMS-PLC.cfg"};

    KMS_TEST_COMPARE(System::Main(2, lVector), 0);
}
