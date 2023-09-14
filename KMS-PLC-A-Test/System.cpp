
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A-Test/System.cpp

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KMS/Cfg/Configurator.h>
#include <KMS/File/Folder.h>

// ===== Local ==============================================================
#include "../Common/Globals.h"
#include "../Common/System.h"

using namespace KMS;

// Data types
// //////////////////////////////////////////////////////////////////////////

typedef struct
{
    const char * mFolder;
    const char * mToDelete_Before;
    const char * mToDelete_After;
    unsigned int mExceptions[2];
}
Case_Normal;

typedef struct
{
    const char* mFolder;
}
Case_Exception;

// Constants
// //////////////////////////////////////////////////////////////////////////

#define KMS_PLC_CFG "KMS-PLC.cfg"

#define TESTS "Tests\\"

#define TEST_0 "Tests\\Test00"
#define TEST_1 "Tests\\Test01"

static const Case_Normal CASE_N[] =
{
    { TESTS "Test00", nullptr  , nullptr        , { 0, 0 } },
    { TESTS "Test01", "PLC.PC6", "PLC.PC6.*.bak", { 0, 0 } },
    { TESTS "Test02", nullptr  , nullptr        , { 2, 2 } },
    { TESTS "Test06", "PLC.PC6", "PLC.PC6.*.bak", { 0, 0 } },
    { TESTS "Test07", "PLC.PC6", nullptr        , { 0, 0 } },
    { TESTS "Test08", nullptr  , nullptr        , { 2, 2 } },
    { TESTS "Test09", nullptr  , nullptr        , { 2, 2 } },
    { TESTS "Test10", nullptr  , nullptr        , { 0, 0 } },
};

#define CASE_N_QTY (sizeof(CASE_N) / sizeof(CASE_N[0]))

static const Case_Exception CASE_E[] =
{
    { TESTS "Test03" },
    { TESTS "Test04" },
    { TESTS "Test05" },
};

#define CASE_E_QTY (sizeof(CASE_E) / sizeof(CASE_E[0]))

KMS_TEST(System_Base, "Auto", sTest_Base)
{
    gConsole.Set_Null();

    // Constructor
    System lS;

    // Clean

    // Edit_PC6

    // Edit_emtp

    // Export

    // Import

    // Read

    // Parse

    // Verify

    // Write

    // ===== CLI::Tool ======================================================

    // DisplayHelp
    lS.DisplayHelp(gConsole.OutputFile());

    // ExecuteCommand

    // Run
}

KMS_TEST(System_Case, "Auto", sTest_Case)
{
    gConsole.Set_Null();

    for (unsigned int i = 0; i < CASE_N_QTY; i++)
    {
        File::Folder lFolder(File::Folder::CURRENT, CASE_N[i].mFolder);

        char lConfigFile[PATH_LENGTH];

        lFolder.GetPath(KMS_PLC_CFG, lConfigFile, sizeof(lConfigFile));

        if ((nullptr != CASE_N[i].mToDelete_Before) && lFolder.DoesFileExist(CASE_N[i].mToDelete_Before))
        {
            lFolder.Delete(CASE_N[i].mToDelete_Before);
        }

        char lArg[LINE_LENGTH];

        sprintf_s(lArg, "ConfigFiles+=%s", lConfigFile);

        const char* lV[1] = { lArg };

        for (unsigned int j = 0; j < 2; j++)
        {
            std::cout << "    Case " << i << " (" << CASE_N[i].mFolder << "), Execution " << j << "..." << std::endl;

            Cfg::Configurator lC;
            System            lS;

            lC.AddConfigurable(&lS);
            lC.ParseArguments(1, lV);

            if (0 != CASE_N[i].mExceptions[j])
            {
                Dbg::gLog.SetHideCount(Dbg::LogFile::Level::LEVEL_ERROR, CASE_N[i].mExceptions[j]);
            }

            KMS_TEST_COMPARE(lS.Run(), 0L);
        }

        if (nullptr != CASE_N[i].mToDelete_After)
        {
            lFolder.DeleteFiles(CASE_N[i].mToDelete_After);
        }
    }
}

KMS_TEST(System_Exception, "Auto", sTest_Exception)
{
    gConsole.Set_Null();

    for (unsigned int i = 0; i < CASE_E_QTY; i++)
    {
        File::Folder lFolder(File::Folder::CURRENT, CASE_E[i].mFolder);

        char lConfigFile[PATH_LENGTH];

        lFolder.GetPath(KMS_PLC_CFG, lConfigFile, sizeof(lConfigFile));

        char lArg[LINE_LENGTH];

        sprintf_s(lArg, "ConfigFiles+=%s", lConfigFile);

        const char* lV[1] = { lArg };

        std::cout << "    Case " << i << " (" << CASE_E[i].mFolder << ") ..." << std::endl;

        Cfg::Configurator lC;
        System            lS;

        lC.AddConfigurable(&lS);
        lC.ParseArguments(1, lV);

        try
        {
            Dbg::gLog.SetHideCount(Dbg::LogFile::Level::LEVEL_ERROR, 2);
            lS.Validate();
            KMS_TEST_ASSERT(false);
        }
        KMS_TEST_CATCH(RESULT_INVALID_CONFIG)
    }
}

KMS_TEST(System_Main, "Auto", sTest_Main)
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
