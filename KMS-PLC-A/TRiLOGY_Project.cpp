
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGY_Project.cpp

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KMS/Cfg/MetaData.h>
#include <KMS/Proc/Process.h>
#include <KMS/Text/File_ASCII.h>

// ===== Local ==============================================================
#include "../Common/TRiLOGY/Word.h"

#include "../Common/TRiLOGY/Project.h"

using namespace KMS;

// Constants
// //////////////////////////////////////////////////////////////////////////

static const Cfg::MetaData MD_EXPORTED             ("Exported = {Path}.PC6.txt");
static const Cfg::MetaData MD_FILE_NAME            ("FileName = {Path}.PC6");
static const Cfg::MetaData MD_SHARED_ADDRESS_REG_EX("SharedAddressRegEx = {RegEx}");
static const Cfg::MetaData MD_SOURCES              ("Sources += {Path}.PC6.in0");

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

static void Instruction_Write();

namespace TRiLOGY
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    Project::Project() : mInputs("input", 1, 256), mOutputs("output", 1, 256), mRelays("relay", 1025, 512)
    {
        mSources.SetCreator(DI::String_Expand::Create);

        AddEntry("Exported"          , &mExported          , false, &MD_EXPORTED);
        AddEntry("FileName"          , &mFileName          , false, &MD_FILE_NAME);
        AddEntry("SharedAddressRegEx", &mSharedAddressRegEx, false, &MD_SHARED_ADDRESS_REG_EX);
        AddEntry("Sources"           , &mSources           , false, &MD_SOURCES);
    }

    bool Project::IsValid() const { return 0 < mFile.GetLineCount(); }

    void Project::Clean()
    {
        std::cout << "Cleaning ..." << std::endl;

        unsigned int lCount = 0;

        lCount += mDefines .Clean();
        // TODO Functions
        lCount += mCounters.Clean();
        lCount += mTimers  .Clean();
        lCount += mRelays  .Clean();
        lCount += mOutputs .Clean();
        lCount += mInputs  .Clean();

        if (0 < lCount)
        {
            Reparse();

            std::cout << "Cleaned - " << lCount << " elements\n";

            Instruction_Write();
        }
        else
        {
            std::cout << "Cleaned - No change" << std::endl;
        }
    }

    void Project::Edit()
    {
        KMS_EXCEPTION_ASSERT(0 < mFileName.GetLength(), APPLICATION_USER_ERROR, "No file name configured", "");

        Proc::Process lP(File::Folder::CURRENT, mFileName.Get());

        lP.SetVerb("open");

        lP.Run(0xffffffff);

        Read();
        Reparse();
    }

    void Project::Export()
    {
        if (0 < mExported.GetLength())
        {
            std::cout << "Exporting " << mExported.Get() << " ..." << std::endl;

            mFile.Write_ASCII(File::Folder::CURRENT, mExported.Get());

            std::cout << "Exported" << std::endl;
        }
    }

    void Project::Import()
    {
        std::cout << "Importing sources ..." << std::endl;

        bool lChanged = false;

        for (const DI::Container::Entry& lEntry : mSources.mInternal)
        {
            const DI::String* lSource = dynamic_cast<const DI::String*>(lEntry.Get());
            assert(NULL != lSource);

            Text::File_ASCII lFile;

            lFile.Read(File::Folder::CURRENT, lSource->Get());

            lFile.RemoveEmptyLines();
            lFile.RemoveComments_Script();

            for (const std::string& lLine : lFile.mLines)
            {
                unsigned int lIndex;
                unsigned int lInit;
                unsigned int lOffset;
                char lName[NAME_LENGTH];
                char lValue[LINE_LENGTH];

                // TODO Import Constant and Word comment or at least put the source file name in the
                //      comment of newly created ones.

                if (2 == sscanf_s(lLine.c_str(), "COUNTER %[0-9A-Za-z_] %u", lName SizeInfo(lName), &lInit))
                {
                    lChanged |= mCounters.Import(lName, lInit);
                }
                else if (2 == sscanf_s(lLine.c_str(), "CONSTANT %[0-9A-Za-z_] %s", lName SizeInfo(lName), lValue SizeInfo(lValue)))
                {
                    lChanged |= mDefines.ImportConstant(lName, lValue);
                }
                else if (1 == sscanf_s(lLine.c_str(), "FUNCTION %[0-9A-Za-z_]", lName SizeInfo(lName)))
                {
                    // TODO
                }
                else if (2 == sscanf_s(lLine.c_str(), "INPUT %[0-9A-Za-z_] %u", lName SizeInfo(lName), &lIndex))
                {
                    lChanged |= mInputs.Import(lName, lIndex);
                }
                else if (2 == sscanf_s(lLine.c_str(), "OUTPUT %[0-9A-Za-z_] %u", lName SizeInfo(lName), &lIndex))
                {
                    lChanged |= mOutputs.Import(lName, lIndex);
                }
                else if (2 == sscanf_s(lLine.c_str(), "RELAY %[0-9A-Za-z_] %u", lName SizeInfo(lName), &lIndex))
                {
                    lChanged |= mRelays.Import(lName, lIndex);
                }
                else if (1 == sscanf_s(lLine.c_str(), "RELAY %[0-9A-Za-z_]", lName SizeInfo(lName)))
                {
                    lChanged |= mRelays.Import(lName);
                }
                else if (2 == sscanf_s(lLine.c_str(), "TIMER %[0-9A-Za-z_] %u", lName SizeInfo(lName), &lInit))
                {
                    lChanged |= mTimers.Import(lName, lInit);
                }
                else if (2 == sscanf_s(lLine.c_str(), "WORD %[0-9A-Za-z_] %u", lName SizeInfo(lName), &lOffset))
                {
                    lChanged |= mDefines.ImportWord(lName, lOffset);
                }
                else if (1 == sscanf_s(lLine.c_str(), "WORD %[0-9A-Za-z_]", lName SizeInfo(lName)))
                {
                    lChanged |= mDefines.ImportWord(lName);
                }
            }
        }

        if (lChanged)
        {
            if (Apply())
            {
                Reparse();
            }

            std::cout << "Imported\n";

            Instruction_Write();
        }
        else
        {
            std::cout << "Imported - No change" << std::endl;
        }
    }

    void Project::Parse()
    {
        if (0 < mFile.GetLineCount())
        {
            std::cout << "Parsing " << mFileName.Get() << " ..." << std::endl;

            const wchar_t* lLine;
            unsigned int   lLineNo = 1;
            unsigned int   lLineCount = mFile.GetLineCount();

            lLineNo = mInputs  .Parse(&mFile, lLineNo, TRiLOGY::Object::FLAG_SINGLE_USE_INFO);
            lLineNo = mOutputs .Parse(&mFile, lLineNo, TRiLOGY::Object::FLAG_SINGLE_USE_INFO);
            lLineNo = mRelays  .Parse(&mFile, lLineNo, TRiLOGY::Object::FLAG_SINGLE_USE_INFO);
            lLineNo = mTimers  .Parse(&mFile, lLineNo, TRiLOGY::Object::FLAG_SINGLE_USE_WARNING);
            lLineNo = mCounters.Parse(&mFile, lLineNo, TRiLOGY::Object::FLAG_SINGLE_USE_WARNING);

            // ===== Circuits ===============================================
            for (; lLineNo < lLineCount; lLineNo++)
            {
                lLine = mFile.GetLine(lLineNo);
                if (0 == wcscmp(L"~END_CIRCUIT~\r", lLine)) { lLineNo++; break; }
            }

            lLineNo = mFunctions.Parse_Code(mFile, lLineNo);
            lLineNo = mFunctions.Parse_Name(mFile, lLineNo);

            // ===== Quick tags =============================================
            for (; lLineNo < lLineCount; lLineNo++)
            {
                lLine = mFile.GetLine(lLineNo);
                if (0 == wcscmp(L"~END_QUICKTAGS~\r", lLine)) { lLineNo++; break; }
            }

            lLineNo = mDefines.Parse(&mFile, lLineNo);
        }
    }

    void Project::Read()
    {
        if (0 < mFileName.GetLength())
        {
            std::cout << "Reading " << mFileName.Get() << " ..." << std::endl;

            mFile.Read(File::Folder::CURRENT, mFileName.Get());

            std::cout << "Read" << std::endl;
        }
    }

    void Project::ValidateConfig() const
    {
        char lMsg[64 + PATH_LENGTH];

        if (0 < mFileName.GetLength())
        {
            sprintf_s(lMsg, "\"%s\" does not exist", mFileName.Get());
            KMS_EXCEPTION_ASSERT(File::Folder::CURRENT.DoesFileExist(mFileName.Get()), APPLICATION_USER_ERROR, lMsg, "");
        }

        for (const DI::Container::Entry& lEntry : mSources.mInternal)
        {
            const DI::String* lSource = dynamic_cast<const DI::String*>(lEntry.Get());
            assert(NULL != lSource);

            sprintf_s(lMsg, "\"%s\" does not exist", lSource->Get());
            KMS_EXCEPTION_ASSERT(File::Folder::CURRENT.DoesFileExist(lSource->Get()), APPLICATION_USER_ERROR, lMsg, "");
        }
    }

    void Project::Verify()
    {
        if (0 < mFile.GetLineCount())
        {
            std::cout << "Verifying " << mFileName.Get() << " ..." << std::endl;

            // TODO For the verification, use a version of mFile_PC6 without
            //      comment

            mCounters  .Verify(mFile);
            mDefines   .Verify(mFile);
            mFunctions .Verify(mFile);
            mInputs    .Verify(mFile);
            mOutputs   .Verify(mFile);
            mRelays    .Verify(mFile);
            mTimers    .Verify(mFile);

            // TODO

            std::cout << "Verified" << std::endl;
        }
    }

    bool Project::VerifyAddress_1X(uint16_t aAddr) const
    {
        uint16_t               lAddr = aAddr;
        const TRiLOGY::Object* lObject;

        if (1025 > lAddr)
        {
            lObject = mInputs.FindObject_ByIndex(lAddr);
        }
        else
        {
            lAddr -= 1025;
            lObject = mRelays.FindObject_ByIndex(lAddr);
        }

        if (NULL == lObject)
        {
            return false;
        }

        if (lObject->TestFlag(TRiLOGY::Object::FLAG_NOT_USED))
        {
            return false;
        }

        return true;
    }

    bool Project::VerifyAddress_4X(uint16_t aAddr) const
    {
        uint16_t lAddr = aAddr;

        if (1000 > lAddr)
        {
            return false;
        }

        lAddr -= 1000;

        const Word* lWord = mDefines.mWords.Find_ByOffset(lAddr);
        if (NULL == lWord)
        {
            return false;
        }

        if (lWord->TestFlag(TRiLOGY::Object::FLAG_NOT_USED))
        {
            return false;
        }

        return true;
    }

    void Project::Write()
    {
        std::cout << "Writing ..." << std::endl;

        File::Folder::CURRENT.Backup(mFileName.Get(), File::Folder::FLAG_BACKUP_RENAME);

        mFile.Write(File::Folder::CURRENT, mFileName.Get(), L"\r");

        std::cout << "Written" << std::endl;
    }

    AddressList* Project::GetSharedAddresses() const
    {
        AddressList* lResult = NULL;

        if (0 < mSharedAddressRegEx.GetLength())
        {
            lResult = new AddressList;

            std::regex lRegEx(mSharedAddressRegEx.Get());

            mInputs .GetAddresses(lRegEx, lResult);
            mOutputs.GetAddresses(lRegEx, lResult);
            mRelays .GetAddresses(lRegEx, lResult);

            mDefines.mWords.GetAddresses(lRegEx, lResult);
        }

        return lResult;
    }

    // Private
    // //////////////////////////////////////////////////////////////////////

    bool Project::Apply()
    {
        bool lResult = false;

        lResult |= mDefines .Apply();
        // TODO Functions
        lResult |= mCounters.Apply();
        lResult |= mTimers  .Apply();
        lResult |= mRelays  .Apply();
        lResult |= mOutputs .Apply();
        lResult |= mInputs  .Apply();

        return lResult;
    }

    void Project::Reparse()
    {
        mCounters  .Clear();
        mDefines   .Clear();
        mFunctions .Clear();
        mInputs    .Clear();
        mOutputs   .Clear();
        mRelays    .Clear();
        mTimers    .Clear();

        Parse();
    }

}

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

void Instruction_Write()
{
    std::cout << "INSTRUCTION\n";
    //                     1           2         3         4         5         6         7
    //            123456789012 345678 90123456789012345678901234567890123456789012345678901234567 89
    std::cout << "    Use the \"Write\" command to save the changes                           [ ]" << std::endl;
}
