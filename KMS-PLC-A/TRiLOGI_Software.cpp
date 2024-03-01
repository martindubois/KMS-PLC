
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI_Software.cpp

// TEST COVERAGE  2023-08-08  KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== Windows ============================================================
#include <Windows.h>

// ===== Import/Includes ====================================================
#include <KMS/Console/HumanScript.h>
#include <KMS/Proc/Process.h>

// ===== Local ==============================================================
#include "../Common/TRiLOGI/Software.h"

using namespace KMS;

#define FILE_EXT_EXE ".exe"

// Configuration
// //////////////////////////////////////////////////////////////////////////

#define CO5_UPLOADER_FOLDER "C:\\CO5Uploader"

#define CO5_UPLOADER_EXE CO5_UPLOADER_FOLDER "\\CO5Uploader" FILE_EXT_EXE

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

static bool SplitFolderAndFile(const char* aIn, char* aFolder, unsigned int aFolderSize_byte, char* aFile, unsigned int aFileSize_byte);

namespace TRiLOGI
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    Software::Software() {}

    // NOT TESTED
    void Software::Edit(const char* aFile)
    {
        Proc::Process lP(File::Folder::CURRENT, aFile);

        lP.SetVerb("open");

        lP.Run(0xffffffff);
    }

    void Software::Program(const char* aFile, const char* aIPAddress, uint8_t aId)
    {
        File::Folder lBin(CO5_UPLOADER_FOLDER);

        char lFile  [PATH_LENGTH];
        char lFolder[PATH_LENGTH];

        if (SplitFolderAndFile(aFile, lFolder, sizeof(lFolder), lFile, sizeof(lFile)))
        {
            File::Folder::CURRENT.Copy(lBin, aFile, lFile);
        }
        else
        {
            File::Folder::CURRENT.Copy(lBin, lFile, lFile);
        }

        char lId[4];

        sprintf_s(lId, "%u", aId);

        Proc::Process lP(File::Folder::NONE, CO5_UPLOADER_EXE);

        lP.SetWorkingDirectory(CO5_UPLOADER_FOLDER);

        lP.AddArgument(lFile);
        lP.AddArgument(aIPAddress);
        lP.AddArgument(lId);

        KMS::Console::HumanScript lHS;

        lHS.Begin("Complete PLC programming");
        lHS.Step("- In the \"Uploading CO5 File to ...\" dialog, click \"Reboot\"");
        lHS.Step("- Close the \"CO5 Uploader Version 3.3\" dialog");
        lHS.End();

        lP.Run(1000 * 60 * 5);
    }

}

// Static functions
// //////////////////////////////////////////////////////////////////////////

bool SplitFolderAndFile(const char* aIn, char* aFolder, unsigned int aFolderSize_byte, char* aFile, unsigned int aFileSize_byte)
{
    memset(aFolder, 0, aFolderSize_byte);

    const char* lPtr = strrchr(aIn, '/');
    if (nullptr == lPtr)
    {
        lPtr = strrchr(aIn, '\\');
    }

    bool lResult = false;

    if (nullptr == lPtr)
    {
        strcpy_s(aFile SizeInfoV(aFileSize_byte), aIn);
    }
    else
    {
        auto lSize_byte = static_cast<unsigned int>(lPtr - aIn);

        KMS_EXCEPTION_ASSERT(aFolderSize_byte > lSize_byte, RESULT_OUTPUT_TOO_SHORT, "Folder buffer too short", aIn);

        memcpy(aFolder, aIn, lSize_byte);

        strcpy_s(aFile SizeInfoV(aFileSize_byte), lPtr + 1);

        lResult = true;
    }

    return lResult;
}
