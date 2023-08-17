
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI_Project.cpp

// TEST COVERAGE  2023-08-17  KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== C++ ================================================================
#include <fstream>

// ===== Import/Includes ====================================================
#include <KMS/Cfg/MetaData.h>
#include <KMS/Text/File_ASCII.h>

// ===== Local ==============================================================
#include "../Common/Version.h"

#include "../Common/TRiLOGI/Project.h"

#include "TRiLOGI/PC6.h"
#include "TRiLOGI/PC6_in0.h"
#include "TRiLOGI/Word.h"

using namespace KMS;

// Constants
// //////////////////////////////////////////////////////////////////////////

static const Cfg::MetaData MD_CREATE_IF_NEEDED("CreateIfNeeded = false | true");
static const Cfg::MetaData MD_EXPORTED        ("Exported = {Path}.PC6.txt");
static const Cfg::MetaData MD_FILE_NAME       ("FileName = {Path}.PC6");
static const Cfg::MetaData MD_HEADER_FILE     ("HeaderFile = {Path}.h");
static const Cfg::MetaData MD_HEADER_PREFIX   ("HeaderPrefix = {Prefix}");
static const Cfg::MetaData MD_PROJECT_TYPE    ("ProjectType = LEGACY | NEW");
static const Cfg::MetaData MD_SOURCES         ("Sources += {Path}.PC6.txt");
static const Cfg::MetaData MD_TOOL_CONFIG     ("HeaderFile = {Path}.cfg");

#define PROJECT_TYPE_DEFAULT (ProjectType::LEGACY)

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

static void Instruction_Write();

namespace TRiLOGI
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    Project::Project()
        : mInputs ("input" ,    1, 256)
        , mOutputs("output",    1, 256)
        , mRelays ("relay" , 1025, 512)
        // ===== Configurable attributes ====================================
        , mProjectType(PROJECT_TYPE_DEFAULT)
        // ===== Callbacks ==================================================
        , ON_PROJECT_TYPE_CHANGED(this, &Project::OnProjectTypeChanged)
    {
        mSources.SetCreator(DI::String_Expand::Create);

        mProjectType.mOnChanged = &ON_PROJECT_TYPE_CHANGED;

        AddEntry("CreateIfNeeded", &mCreateIfNeeded, false, &MD_CREATE_IF_NEEDED);
        AddEntry("Exported"      , &mExported      , false, &MD_EXPORTED);
        AddEntry("FileName"      , &mFileName      , false, &MD_FILE_NAME);
        AddEntry("HeaderFile"    , &mHeaderFile    , false, &MD_HEADER_FILE);
        AddEntry("HeaderPrefix"  , &mHeaderPrefix  , false, &MD_HEADER_PREFIX);
        AddEntry("ProjectType"   , &mProjectType   , false, &MD_PROJECT_TYPE);
        AddEntry("Sources"       , &mSources       , false, &MD_SOURCES);
        AddEntry("ToolConfig"    , &mToolConfig    , false, &MD_TOOL_CONFIG);

        AddEntry("Words", &mDefines.mWords, false);
    }

    bool Project::IsValid() const { return 0 < mFile.GetLineCount(); }

    void Project::Clean()
    {
        ::Console::Progress_Begin("TRiLOGY", "Cleaning ...");

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
            // NOT TESTED
            BuildFile();

            ::Console::Stats(lCount, "elements");
            ::Console::Progress_End("Clearned");

            Instruction_Write();
        }
        else
        {
            ::Console::Progress_End("Cleaned (No change) (NOT TESTED)");
        }
    }

    // NOT TESTED
    void Project::Edit()
    {
        KMS_EXCEPTION_ASSERT(0 < mFileName.GetLength(), APPLICATION_USER_ERROR, "No file name configured", "");

        mSoftware.Edit(mFileName.Get());

        Read();
        Reparse();
    }

    void Project::Export()
    {
        if (0 < mExported.GetLength())
        {
            ::Console::Progress_Begin("TRiLOGY", "Exporting", mExported.Get());
            {
                mFile.Write_ASCII(File::Folder::CURRENT, mExported.Get());
            }
            ::Console::Progress_End("Exported");
        }

        if (0 < mHeaderFile.GetLength())
        {
            Export_HeaderFile();
        }

        if (0 < mToolConfig.GetLength())
        {
            Export_ToolConfig();
        }
    }

    void Project::Import()
    {
        ::Console::Progress_Begin("TRiLOGY", "Importing sources");

        auto lChanged = ProjectType::NEW == mProjectType;

        for (const auto& lEntry : mSources.mInternal)
        {
            auto lSource = dynamic_cast<const DI::String*>(lEntry.Get());
            assert(nullptr != lSource);

            lChanged |= Import_Source(lSource->Get());
        }

        if (lChanged)
        {
            BuildFile();
            Verify();

            ::Console::Progress_End("Imported");

            switch (mProjectType)
            {
            case ProjectType::LEGACY: Instruction_Write(); break;
            case ProjectType::NEW   :             Write(); break;

            default: assert(false);
            }
        }
        else
        {
            ::Console::Progress_End("Imported (No change)");
        }
    }

    void Project::Parse()
    {
        if (0 < mFile.GetLineCount())
        {
            ::Console::Progress_Begin("TRiLOGY", "Parsing", mFileName.Get());

            unsigned int lLineNo = 1;

            switch (mProjectType)
            {
            case ProjectType::LEGACY: Parse_Legacy(); break;
            case ProjectType::NEW   : Parse_New   (); break;

            default: assert(false);
            }
        }
        else if (mCreateIfNeeded)
        {
            ::Console::Change("Creating", mFileName.Get());

            Create();

            Instruction_Write();
        }
    }

    void Project::Read()
    {
        if ((0 < mFileName.GetLength()) && File::Folder::CURRENT.DoesFileExist(mFileName.Get()))
        {
            ::Console::Progress_Begin("TRiLOGY", "Reading", mFileName.Get());
            {
                mFile.Clear();
                mFile.Read(File::Folder::CURRENT, mFileName.Get());
            }
            ::Console::Progress_End("Read");
        }
    }

    void Project::ValidateConfig() const
    {
        char lMsg[64 + PATH_LENGTH];

        if (0 < mFileName.GetLength())
        {
            if (!mCreateIfNeeded)
            {
                if (!File::Folder::CURRENT.DoesFileExist(mFileName.Get()))
                {
                    sprintf_s(lMsg, "\"%s\" does not exist", mFileName.Get());
                    KMS_EXCEPTION(APPLICATION_USER_ERROR, lMsg, "");
                }
            }
        }

        if (0 < mHeaderPrefix.GetLength())
        {
            KMS_EXCEPTION_ASSERT(0 < mHeaderFile.GetLength(), APPLICATION_USER_ERROR, "A header prefix is specified without a header file name", "");
        }

        for (auto& lEntry : mSources.mInternal)
        {
            const DI::String* lSource = dynamic_cast<const DI::String*>(lEntry.Get());
            assert(nullptr != lSource);

            if (!File::Folder::CURRENT.DoesFileExist(lSource->Get()))
            {
                sprintf_s(lMsg, "\"%s\" does not exist", lSource->Get());
                KMS_EXCEPTION(APPLICATION_USER_ERROR, lMsg, "");
            }
        }

        mDefines.mWords.ValidateConfig();
    }

    void Project::Verify()
    {
        if (0 < mFile.GetLineCount())
        {
            ::Console::Progress_Begin("TRiLOGY", "Verifying", mFileName.Get());
            {
                // TODO For the verification, use a version of mFile_PC6 without
                //      comment

                mCounters  .Verify(mFile, &mPublicAddresses);
                mDefines   .Verify(mFile, &mPublicAddresses);
                mFunctions .Verify(mFile);
                mInputs    .Verify(mFile, &mPublicAddresses);
                mOutputs   .Verify(mFile, &mPublicAddresses);
                mRelays    .Verify(mFile, &mPublicAddresses);
                mTimers    .Verify(mFile, &mPublicAddresses);
            }
            ::Console::Progress_End("Verified");
        }
    }

    // NOT TESTED
    bool Project::VerifyAddress_1X(uint16_t aAddr) const
    {
        auto lAddr = aAddr;

        const TRiLOGI::Object* lObject;

        if (1025 > lAddr)
        {
            lObject = mInputs.FindObject_ByIndex(lAddr);
        }
        else
        {
            lAddr -= 1025;
            lObject = mRelays.FindObject_ByIndex(lAddr);
        }

        if (nullptr == lObject)
        {
            return false;
        }

        if (lObject->TestFlag(TRiLOGI::Object::FLAG_NOT_USED))
        {
            return false;
        }

        return true;
    }

    // NOT TESTED  Returning false
    bool Project::VerifyAddress_4X(uint16_t aAddr) const
    {
        auto lAddr = aAddr;

        if (1000 > lAddr)
        {
            return false;
        }

        lAddr -= 1000;

        const Word* lWord = mDefines.mWords.Find_ByOffset(lAddr);
        if (nullptr == lWord)
        {
            return false;
        }

        if (lWord->TestFlag(TRiLOGI::Object::FLAG_NOT_USED))
        {
            return false;
        }

        return true;
    }

    void Project::Write()
    {
        ::Console::Progress_Begin("TRiLOGY", "Writing");
        {
            if (File::Folder::CURRENT.DoesFileExist(mFileName.Get()))
            {
                File::Folder::CURRENT.Backup(mFileName.Get(), File::Folder::FLAG_BACKUP_RENAME);
            }

            mFile.Write(File::Folder::CURRENT, mFileName.Get(), L"\r\n");
        }
        ::Console::Progress_End("Written");
    }

    const AddressList* Project::GetPublicAddresses() const
    {
        return &mPublicAddresses;
    }

    // Private
    // //////////////////////////////////////////////////////////////////////

    void Project::AddPublicAddress(const char* aPrivate, const char* aPublic)
    {
        assert(nullptr != aPrivate);

        auto lObj = mInputs.FindObject_ByName(aPrivate);
        if (nullptr == lObj)
        {
            lObj = mOutputs.FindObject_ByName(aPrivate);
            if (nullptr == lObj)
            {
                lObj = mRelays.FindObject_ByName(aPrivate);
            }
        }

        if (nullptr == lObj)
        {
            lObj = mDefines.FindObject_ByName(aPrivate);
            if (nullptr == lObj)
            {
                ::Console::Warning_Begin()
                    << "No object named " << aPrivate;
                ::Console::Warning_End();
            }
            else
            {
                auto lWord = dynamic_cast<Word*>(lObj);
                if (nullptr == lWord)
                {
                    ::Console::Warning_Begin()
                        << "The object " << aPrivate << " is not of an expected type";
                    ::Console::Warning_End();
                }
                else
                {
                    mPublicAddresses.Add(aPublic, AddressType::MODBUS_RTU_4X, lWord->GetOffset());
                }
            }
        }
        else
        {
            mPublicAddresses.Add(aPublic, AddressType::MODBUS_RTU_1X, lObj->GetIndex());
        }
    }

    void Project::AddPublicAddresses(const char* aRegEx)
    {
        assert(nullptr != aRegEx);

        auto lCount = mPublicAddresses.size();

        std::regex lRegEx(aRegEx);

        mInputs .GetAddresses(lRegEx, &mPublicAddresses);
        mOutputs.GetAddresses(lRegEx, &mPublicAddresses);
        mRelays .GetAddresses(lRegEx, &mPublicAddresses);

        mDefines.mWords.GetAddresses(lRegEx, &mPublicAddresses);

        if (mPublicAddresses.size() == lCount)
        {
            ::Console::Warning_Begin()
                << "The expression \"" << aRegEx << "\" does not match any name";
            ::Console::Warning_End();
        }
    }

    void Project::BuildFile()
    {
        mFile.Clear();

        mFile.AddLine(PC6_FILE_HEADER);

        mInputs   .AddToFile    (&mFile); mFile.AddLine(PC6_SECTION_END);
        mOutputs  .AddToFile    (&mFile); mFile.AddLine(PC6_SECTION_END);
        mRelays   .AddToFile    (&mFile); mFile.AddLine(PC6_SECTION_END);
        mTimers   .AddToFile    (&mFile); mFile.AddLine(PC6_SECTION_END);
        mCounters .AddToFile    (&mFile); mFile.AddLine(PC6_SECTION_END);
        mCircuits .AddToFile    (&mFile); mFile.AddLine(PC6_SECTION_END_CIRCUIT);
        mFunctions.AddCodeToFile(&mFile); mFile.AddLine(PC6_SECTION_END_CUSTFN);
        mFunctions.AddToFile    (&mFile); mFile.AddLine(PC6_SECTION_END_CUSTFNLABEL);
        mQuickTags.AddToFile    (&mFile); mFile.AddLine(PC6_SECTION_END_QUICKTAGS);
        mDefines  .AddToFile    (&mFile); mFile.AddLine(PC6_SECTION_END_DEFINES);
        mFooter   .AddToFile    (&mFile);
    }

    void Project::Create()
    {
        mFooter.AddLine(PC6_SECTION_END_BREAKPOINT);

        BuildFile();
    }

    void Project::Export_HeaderFile()
    {
        auto lFileName = mHeaderFile.Get();
        assert(nullptr != lFileName);

        ::Console::Progress_Begin("TRiLOGY", "Export", lFileName);
        {
            std::ofstream lFile(lFileName);
            if (!lFile.is_open())
            {
                char lMsg[64 + PATH_LENGTH];
                sprintf_s(lMsg, "Cannot open \"%s\" for writing", lFileName);
                KMS_EXCEPTION(APPLICATION_USER_ERROR, lMsg, "");
            }

            lFile << "\n";
            lFile << "// File  " << lFileName << "\n";
            lFile << "\n";
            lFile << "// Generated by KMS-PLC.exe version " << VERSION;
            lFile << "\n";
            lFile << "#pragma once\n";
            lFile << "\n";

            auto lPrefix = mHeaderPrefix.Get();
            assert(nullptr != lPrefix);

            for (const auto& lAddr : mPublicAddresses)
            {
                lFile << "#define " << lPrefix << lAddr.GetName() << " (" << lAddr.GetAddress() << ")\n";
            }

            lFile.close();
        }
        ::Console::Progress_End("Exported");
    }

    void Project::Export_ToolConfig()
    {
        auto lFileName = mToolConfig.Get();
        assert(nullptr != lFileName);

        ::Console::Progress_Begin("TRiLOGY", "Export", lFileName);
        {
            std::ofstream lFile(lFileName);
            if (!lFile.is_open())
            {
                char lMsg[64 + PATH_LENGTH];
                sprintf_s(lMsg, "Cannot open \"%s\" for writing", lFileName);
                KMS_EXCEPTION(APPLICATION_USER_ERROR, lMsg, "");
            }

            lFile << "\n";
            lFile << "# File  " << lFileName << "\n";
            lFile << "\n";
            lFile << "# Generated by KMS-PLC.exe version " << VERSION;
            lFile << "\n";

            for (const auto& lAddr : mPublicAddresses)
            {
                auto lA = lAddr.GetAddress();
                auto lN = lAddr.GetName();

                switch (lAddr.GetType())
                {
                case AddressType::MODBUS_RTU_1X: lFile << "Coils."            << lN << " = " << lA << "\n"; break;
                case AddressType::MODBUS_RTU_4X: lFile << "HoldingRegisters." << lN << " = " << lA << "\n"; break;
                }
            }

            lFile.close();
        }
        ::Console::Progress_End("Exported");
    }

    bool Project::Import_Source(const char* aFileName)
    {
        auto lResult = false;

        Text::File_ASCII lFile;

        lFile.Read(File::Folder::CURRENT, aFileName);

        lFile.RemoveEmptyLines();
        lFile.RemoveComments_Script();

        for (auto lIt = lFile.mLines.begin(); lIt != lFile.mLines.end(); lIt++)
        {
            unsigned int lIndex;
            unsigned int lInit;
            unsigned int lOffset;
            char lN[NAME_LENGTH];
            char lValue[LINE_LENGTH];

            // TODO Import Constant and Word comment or at least put the source file name in the
            //      comment of newly created ones.

            auto lL = lIt->c_str();

            if (1 == sscanf_s(lL, PC6_IN0_ADDRESSES " " PC6_IN0_STRING, lN SizeInfo(lN)))
            {
                AddPublicAddresses(lN);
            }
            else if (2 == sscanf_s(lL, PC6_IN0_ADDRESS " " PC6_IN0_NAME " " PC6_IN0_NAME, lN SizeInfo(lN), lValue SizeInfo(lValue)))
            {
                AddPublicAddress(lN, lValue);
            }
            else if (1 == sscanf_s(lL, PC6_IN0_ADDRESS " " PC6_IN0_NAME, lN SizeInfo(lN)))
            {
                AddPublicAddress(lN, lN);
            }
            else if (2 == sscanf_s(lL, PC6_IN0_CONSTANT " " PC6_IN0_NAME " " PC6_IN0_STRING, lN SizeInfo(lN), lValue SizeInfo(lValue)))
            {
                lResult |= mDefines.ImportConstant(lN, lValue);
            }
            else if (2 == sscanf_s(lL, PC6_IN0_COUNTER " " PC6_IN0_NAME " " PC6_IN0_UINT, lN SizeInfo(lN), &lInit))
            {
                lResult |= mCounters.Import(lN, lInit);
            }
            else if (1 == sscanf_s(lL, PC6_IN0_FUNCTION " " PC6_IN0_NAME, lN SizeInfo(lN)))
            {
                lResult |= mFunctions.Import(lN, &lFile, &lIt);
            }
            else if (2 == sscanf_s(lL, PC6_IN0_INPUT " " PC6_IN0_NAME " " PC6_IN0_UINT, lN SizeInfo(lN), &lIndex))
            {
                lResult |= mInputs.Import(lN, lIndex);
            }
            else if (2 == sscanf_s(lL, PC6_IN0_OUTPUT " " PC6_IN0_NAME " " PC6_IN0_UINT, lN SizeInfo(lN), &lIndex))
            {
                lResult |= mOutputs.Import(lN, lIndex);
            }
            else if (2 == sscanf_s(lL, PC6_IN0_RELAY " " PC6_IN0_NAME " " PC6_IN0_UINT, lN SizeInfo(lN), &lIndex))
            {
                lResult |= mRelays.Import(lN, lIndex);
            }
            else if (1 == sscanf_s(lL, PC6_IN0_RELAY " " PC6_IN0_NAME, lN SizeInfo(lN)))
            {
                lResult |= mRelays.Import(lN);
            }
            else if (2 == sscanf_s(lL, PC6_IN0_TIMER " " PC6_IN0_NAME " " PC6_IN0_UINT, lN SizeInfo(lN), &lInit))
            {
                lResult |= mTimers.Import(lN, lInit);
            }
            else if (2 == sscanf_s(lL, PC6_IN0_WORD " " PC6_IN0_NAME " " PC6_IN0_UINT, lN SizeInfo(lN), &lOffset))
            {
                lResult |= mDefines.ImportWord(lN, lOffset);
            }
            else if (1 == sscanf_s(lL, PC6_IN0_WORD " " PC6_IN0_NAME, lN SizeInfo(lN)))
            {
                lResult |= mDefines.ImportWord(lN);
            }
            else
            {
                ::Console::Warning_IgnoredLine(lIt->GetUserLineNo(), lL);
            }
        }

        return lResult;
    }

    void Project::Parse_Legacy()
    {
        unsigned int lLineNo = 1;

        lLineNo = mInputs   .Parse(&mFile, lLineNo, TRiLOGI::Object::FLAG_SINGLE_USE_INFO);
        lLineNo = mOutputs  .Parse(&mFile, lLineNo, TRiLOGI::Object::FLAG_SINGLE_USE_INFO);
        lLineNo = mRelays   .Parse(&mFile, lLineNo, TRiLOGI::Object::FLAG_SINGLE_USE_INFO);
        lLineNo = mTimers   .Parse(&mFile, lLineNo, TRiLOGI::Object::FLAG_SINGLE_USE_WARNING);
        lLineNo = mCounters .Parse(&mFile, lLineNo, TRiLOGI::Object::FLAG_SINGLE_USE_WARNING);
        lLineNo = mCircuits .Parse(&mFile, lLineNo, false);
        lLineNo = mFunctions.Parse_Code(&mFile, lLineNo);
        lLineNo = mFunctions.Parse_Name(&mFile, lLineNo);
        lLineNo = mQuickTags.Parse(&mFile, lLineNo, false);
        lLineNo = mDefines  .Parse(&mFile, lLineNo);
        lLineNo = mFooter   .Parse(&mFile, lLineNo, true);
    }

    void Project::Parse_New()
    {
        unsigned int lLineNo = 1;

        lLineNo = ParseNothing(lLineNo, 5); // Inputs, Outputs, Relays, Timers, Counters
        lLineNo = mCircuits.Parse(&mFile, lLineNo, false);
        lLineNo = ParseNothing(lLineNo, 2); // Functions, Function names
        lLineNo = mQuickTags.Parse(&mFile, lLineNo, false);
        lLineNo = ParseNothing(lLineNo, 1); // Defines
        lLineNo = mFooter.Parse(&mFile, lLineNo, true);
    }

    unsigned int Project::ParseNothing(unsigned int aLineNo, unsigned int aSectionCount)
    {
        assert(0 < aLineNo);
        assert(0 < aSectionCount);

        auto lLineCount    = mFile.GetLineCount();
        auto lLineNo       = aLineNo;
        auto lSectionCount = aSectionCount;

        for (; lLineNo < lLineCount; lLineNo++)
        {
            const wchar_t* lLine = mFile.GetLine(lLineNo);

            if (PC6_SECTION_END_C == lLine[0])
            {
                if (1 >= lSectionCount)
                {
                    lLineNo++;
                    break;
                }

                lSectionCount--;
            }
        }

        return lLineNo;
    }

    // NOT TESTED
    void Project::Reparse()
    {
        switch (mProjectType)
        {
        case ProjectType::LEGACY:
            mCounters .ClearList();
            mDefines  .ClearList();
            mFunctions.ClearList();
            mInputs   .ClearList();
            mOutputs  .ClearList();
            mRelays   .ClearList();
            mTimers   .ClearList();
            break;

        case ProjectType::NEW: break;

        default: assert(false);
        }

        mCircuits .ClearList();
        mFooter   .ClearList();
        mQuickTags.ClearList();

        Parse();
    }

    // ===== Callbacks ======================================================

    unsigned int Project::OnProjectTypeChanged(void*, void*)
    {
        mCounters .SetProjectType(mProjectType);
        mDefines  .SetProjectType(mProjectType);
        mFunctions.SetProjectType(mProjectType);
        mInputs   .SetProjectType(mProjectType);
        mOutputs  .SetProjectType(mProjectType);
        mRelays   .SetProjectType(mProjectType);
        mTimers   .SetProjectType(mProjectType);

        return 0;
    }

}

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

void Instruction_Write()
{
    ::Console::Instruction_Begin()

        //           1           2         3         4         5         6         7
        //  123456789012 345678 90123456789012345678901234567890123456789012345678901234567
        << "    Use the \"Write\" command to save the changes                           [ ]" << std::endl;

    ::Console::Instruction_End();
}
