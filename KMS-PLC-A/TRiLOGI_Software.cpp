
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
#include <KMS/Click/Master.h>
#include <KMS/Proc/Process.h>

// ===== Local ==============================================================
#include "../Common/Globals.h"

#include "../Common/TRiLOGI/Software.h"

using namespace KMS;

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
        char lId[ 4];

        sprintf_s(lId, "%u", aId);

        Click::Master lCM;
        Proc::Process lP(File::Folder::NONE, "C:\\CO5Uploader\\CO5Uploader.exe");

        char lFile  [PATH_LENGTH];
        char lFolder[PATH_LENGTH];

        if (SplitFolderAndFile(aFile, lFolder, sizeof(lFolder), lFile, sizeof(lFile)))
        {
            char lWorking[PATH_LENGTH];

            File::Folder::CURRENT.GetPath(lFolder, lWorking, sizeof(lWorking));

            lP.SetWorkingDirectory(lWorking);
        }
        else
        {
            lP.SetWorkingDirectory(File::Folder::CURRENT.GetPath());
        }

        lP.AddArgument(lFile);
        lP.AddArgument(aIPAddress);
        lP.AddArgument(lId);

        ::Console::Instruction_Begin()
            //           1          2         3          4         5          6         7
            //  1234567890123 45678901234567890123456789 0123456789012345 6789012 345678901234567
            << "    - In the \"Uploading CO5 File to ...\" dialog, click \"Reboot\"           [ ]\n"
            //  1234567890123456 7890123456789012345678901 234567890123456789012345678901234567
            << "    - Close the \"CO5 Uploader Version 3.3\" dialo                          [ ]\n";
        ::Console::Instruction_End();

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
