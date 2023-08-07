
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A-Test/System.cpp

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KMS/File/Folder.h>

// ===== Local ==============================================================
#include "../Common/Globals.h"
#include "../Common/System.h"

using namespace KMS;

// Constants
// //////////////////////////////////////////////////////////////////////////

#define KMS_PLC_CFG "KMS-PLC.cfg"

#define TEST_0 "Tests\\Test0"
#define TEST_1 "Tests\\Test1"

KMS_TEST(System_Main, "System_Main", "Auto", sTest_Main)
{
    gConsole.Set_Null();

    const char* lVector[2] = { "KMS-PLC-A-Test.exe", "ConfigFiles+=" TEST_0 "\\" KMS_PLC_CFG};

    KMS_TEST_COMPARE(System::Main(2, lVector), 0);

    lVector[1] = "ConfigFiles+=" TEST_1 "\\" KMS_PLC_CFG;

    File::Folder lTest1(File::Folder::CURRENT, TEST_1);

    lTest1.DeleteFiles("PLC.PC6");

    KMS_TEST_COMPARE(System::Main(2, lVector), 0);
    KMS_TEST_COMPARE(System::Main(2, lVector), 0);
}
