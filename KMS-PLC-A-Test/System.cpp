
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
    const char * mConfigFile;
    unsigned int mExceptions[2];
    int          mResults   [2];
}
Case_Normal;

typedef struct
{
    const char    * mConfigFile;
    Exception::Code mCode;
}
Case_Exception;

// Constants
// //////////////////////////////////////////////////////////////////////////

#define KMS_PLC_CFG "KMS-PLC.cfg"

#define CONFIG_FILE "ConfigFiles+=Tests\\"

#define TEST_0 "Tests\\Test00"
#define TEST_1 "Tests\\Test01"

static const Case_Normal CASE_N[] =
{
    { CONFIG_FILE "Test00\\" KMS_PLC_CFG, { 0, 0 }, { 0, 0 } },
    { CONFIG_FILE "Test01\\" KMS_PLC_CFG, { 0, 0 }, { 0, 0 } },
    { CONFIG_FILE "Test02\\" KMS_PLC_CFG, { 2, 2 }, { 0, 0 } },
};

#define CASE_N_QTY (sizeof(CASE_N) / sizeof(CASE_N[0]))

static const Case_Exception CASE_E[] =
{
    { CONFIG_FILE "Test03\\" KMS_PLC_CFG, Exception::Code::APPLICATION_USER_ERROR },
    { CONFIG_FILE "Test04\\" KMS_PLC_CFG, Exception::Code::APPLICATION_USER_ERROR },
    { CONFIG_FILE "Test05\\" KMS_PLC_CFG, Exception::Code::APPLICATION_USER_ERROR },
    { CONFIG_FILE "Test06\\" KMS_PLC_CFG, Exception::Code::APPLICATION_USER_ERROR },
};

#define CASE_E_QTY (sizeof(CASE_E) / sizeof(CASE_E[0]))

KMS_TEST(System_Base, "System_Base", "Auto", sTest_Base)
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

KMS_TEST(System_Case, "System_Case", "Auto", sTest_Case)
{
    gConsole.Set_Null();

    for (unsigned int i = 0; i < CASE_N_QTY; i++)
    {
        const char* lV[1] = { CASE_N[i].mConfigFile };

        for (unsigned int j = 0; j < 2; j++)
        {
            std::cout << "    Case " << i << ", Execution " << j << "..." << std::endl;

            Cfg::Configurator lC;
            System            lS;

            lC.AddConfigurable(&lS);
            lC.ParseArguments(1, lV);

            if (0 != CASE_N[i].mExceptions[j])
            {
                Dbg::gLog.SetHideCount(Dbg::LogFile::Level::LEVEL_ERROR, CASE_N[i].mExceptions[j]);
            }

            KMS_TEST_COMPARE(lS.Run(), CASE_N[i].mResults[j]);
        }
    }
}

KMS_TEST(System_Exception, "System_Exception", "Auto", sTest_Exception)
{
    gConsole.Set_Null();

    for (unsigned int i = 0; i < CASE_E_QTY; i++)
    {
        const char* lV[1] = { CASE_E[i].mConfigFile };

        std::cout << "    Exception " << i << "..." << std::endl;

        Cfg::Configurator lC;
        System            lS;

        lC.AddConfigurable(&lS);
        lC.ParseArguments(1, lV);

        try
        {
            Dbg::gLog.SetHideCount(Dbg::LogFile::Level::LEVEL_ERROR, 2);
            lS.Run();
            KMS_TEST_ASSERT(false);
        }
        catch (Exception eE)
        {
            KMS_TEST_COMPARE(eE.GetCode(), CASE_E[i].mCode);
        }
    }
}

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
