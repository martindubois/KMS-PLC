
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI_Project.cpp

// TEST COVERAGE  2023-08-08  KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== C++ ================================================================
#include <fstream>

// ===== Import/Includes ====================================================
#include <KMS/Cfg/MetaData.h>
#include <KMS/Text/File_ASCII.h>

// ===== Local ==============================================================
#include "../Common/Version.h"

#include "../Common/TRiLOGI/Project.h"

#include "TRiLOGI/Word.h"

using namespace KMS;

// Constants
// //////////////////////////////////////////////////////////////////////////

static const Cfg::MetaData MD_CREATE_IF_NEEDED("CreateIfNeeded = false | true");
static const Cfg::MetaData MD_EXPORTED        ("Exported = {Path}.PC6.txt");
static const Cfg::MetaData MD_FILE_NAME       ("FileName = {Path}.PC6");
static const Cfg::MetaData MD_HEADER_FILE     ("HeaderFile = {Path}.h");
static const Cfg::MetaData MD_HEADER_PREFIX   ("HeaderPrefix = {Prefix}");
static const Cfg::MetaData MD_PUBLIC_DEFS     ("PublicDefs += {Path}.txt");
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
        mPublicDefs.SetCreator(DI::String_Expand::Create);
        mSources   .SetCreator(DI::String_Expand::Create);

        mProjectType.mOnChanged = &ON_PROJECT_TYPE_CHANGED;

        AddEntry("CreateIfNeeded", &mCreateIfNeeded, false, &MD_CREATE_IF_NEEDED);
        AddEntry("Exported"      , &mExported      , false, &MD_EXPORTED);
        AddEntry("FileName"      , &mFileName      , false, &MD_FILE_NAME);
        AddEntry("HeaderFile"    , &mHeaderFile    , false, &MD_HEADER_FILE);
        AddEntry("HeaderPrefix"  , &mHeaderPrefix  , false, &MD_HEADER_PREFIX);
        AddEntry("PublicDefs"    , &mPublicDefs    , false, &MD_PUBLIC_DEFS);
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
            Reparse();

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

        auto lChanged = false;

        switch (mProjectType)
        {
        case ProjectType::LEGACY: break;

        case ProjectType::NEW:
            lChanged = true;
            RemoveParsed();
            break;

        default: assert(false);
        }

        for (const auto& lEntry : mSources.mInternal)
        {
            auto lSource = dynamic_cast<const DI::String*>(lEntry.Get());
            assert(nullptr != lSource);

            lChanged |= Import_Source(lSource->Get());
        }

        if (lChanged)
        {
            if (Apply())
            {
                Reparse();
            }

            ::Console::Progress_End("Imported");

            switch (mProjectType)
            {
            case ProjectType::LEGACY: Instruction_Write(); break;

            case ProjectType::NEW:
                Verify();
                Write();
                break;

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

            ParsePublicDefs();
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

        for (auto& lEntry : mPublicDefs.mInternal)
        {
            const DI::String* lSource = dynamic_cast<const DI::String*>(lEntry.Get());
            assert(nullptr != lSource);

            if (!File::Folder::CURRENT.DoesFileExist(lSource->Get()))
            {
                sprintf_s(lMsg, "\"%s\" does not exist", lSource->Get());
                KMS_EXCEPTION(APPLICATION_USER_ERROR, lMsg, "");
            }
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

                mCounters  .Verify(mFile);
                mDefines   .Verify(mFile);
                mFunctions .Verify(mFile);
                mInputs    .Verify(mFile);
                mOutputs   .Verify(mFile);
                mRelays    .Verify(mFile);
                mTimers    .Verify(mFile);
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
                    mPublicAddresses.push_back(Address(aPublic, AddressType::MODBUS_RTU_4X, lWord->GetOffset()));
                }
            }
        }
        else
        {
            mPublicAddresses.push_back(Address(aPublic, AddressType::MODBUS_RTU_1X, lObj->GetIndex()));
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

    bool Project::Apply()
    {
        auto lResult = false;

        lResult |= mDefines  .Apply();
        lResult |= mFunctions.Apply();
        lResult |= mFunctions.Apply_Code();
        lResult |= mCounters .Apply();
        lResult |= mTimers   .Apply();
        lResult |= mRelays   .Apply();
        lResult |= mOutputs  .Apply();
        lResult |= mInputs   .Apply();

        return lResult;
    }

    void Project::Create()
    {
        mFile.AddLine(L"\x00f8\x00f5TRiLOGI Ver 5.0");
        mFile.AddLine(L"~"); mInputs  .SetFile(&mFile, 1);
        mFile.AddLine(L"~"); mOutputs .SetFile(&mFile, 2);
        mFile.AddLine(L"~"); mRelays  .SetFile(&mFile, 3);
        mFile.AddLine(L"~"); mTimers  .SetFile(&mFile, 4);
        mFile.AddLine(L"~"); mCounters.SetFile(&mFile, 5);
        mFile.AddLine(L"~END_CIRCUIT~");
        mFile.AddLine(L"~END_CUSTFN~"     ); mFunctions.SetLineNo_End_Code(7);
        mFile.AddLine(L"~END_CUSTFNLABEL~"); mFunctions.SetFile(&mFile, 8);
        mFile.AddLine(L"~END_QUICKTAGS~");
        mFile.AddLine(L"~END_DEFINES~"); mDefines.SetFile(&mFile, 10);
        mFile.AddLine(L"~END_BREAKPOINTS~");
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
            char lName[NAME_LENGTH];
            char lValue[LINE_LENGTH];

            // TODO Import Constant and Word comment or at least put the source file name in the
            //      comment of newly created ones.

            if (2 == sscanf_s(lIt->c_str(), "COUNTER %[0-9A-Za-z_] %u", lName SizeInfo(lName), &lInit))
            {
                lResult |= mCounters.Import(lName, lInit);
            }
            else if (2 == sscanf_s(lIt->c_str(), "CONSTANT %[0-9A-Za-z_] %s", lName SizeInfo(lName), lValue SizeInfo(lValue)))
            {
                lResult |= mDefines.ImportConstant(lName, lValue);
            }
            else if (1 == sscanf_s(lIt->c_str(), "FUNCTION %[0-9A-Za-z_]", lName SizeInfo(lName)))
            {
                lResult |= mFunctions.Import(lName, &lFile, &lIt);
            }
            else if (2 == sscanf_s(lIt->c_str(), "INPUT %[0-9A-Za-z_] %u", lName SizeInfo(lName), &lIndex))
            {
                lResult |= mInputs.Import(lName, lIndex);
            }
            else if (2 == sscanf_s(lIt->c_str(), "OUTPUT %[0-9A-Za-z_] %u", lName SizeInfo(lName), &lIndex))
            {
                lResult |= mOutputs.Import(lName, lIndex);
            }
            else if (2 == sscanf_s(lIt->c_str(), "RELAY %[0-9A-Za-z_] %u", lName SizeInfo(lName), &lIndex))
            {
                lResult |= mRelays.Import(lName, lIndex);
            }
            else if (1 == sscanf_s(lIt->c_str(), "RELAY %[0-9A-Za-z_]", lName SizeInfo(lName)))
            {
                lResult |= mRelays.Import(lName);
            }
            else if (2 == sscanf_s(lIt->c_str(), "TIMER %[0-9A-Za-z_] %u", lName SizeInfo(lName), &lInit))
            {
                lResult |= mTimers.Import(lName, lInit);
            }
            else if (2 == sscanf_s(lIt->c_str(), "WORD %[0-9A-Za-z_] %u", lName SizeInfo(lName), &lOffset))
            {
                lResult |= mDefines.ImportWord(lName, lOffset);
            }
            else if (1 == sscanf_s(lIt->c_str(), "WORD %[0-9A-Za-z_]", lName SizeInfo(lName)))
            {
                lResult |= mDefines.ImportWord(lName);
            }
            else
            {
                ::Console::Warning_IgnoredLine(lIt->GetUserLineNo(), lIt->c_str());
            }
        }

        return lResult;
    }

    unsigned int Project::ParseNothing(unsigned int aLineNo)
    {
        assert(0 < aLineNo);

        auto lLineCount = mFile.GetLineCount();
        auto lLineNo    = aLineNo;

        for (; lLineNo < lLineCount; lLineNo++)
        {
            const wchar_t* lLine = mFile.GetLine(lLineNo);
            if (L'~' == lLine[0])
            {
                lLineNo++;
                break;
            }
        }

        return lLineNo;
    }

    void Project::ParsePublicDef(const char* aFileName)
    {
        ::Console::Progress_Begin("TRiLOGY", "Parsing", aFileName);
        {
            Text::File_ASCII lFile;

            lFile.Read(File::Folder::CURRENT, aFileName);

            lFile.RemoveEmptyLines();
            lFile.RemoveComments_Script();

            for (auto lIt = lFile.mLines.begin(); lIt != lFile.mLines.end(); lIt++)
            {
                char lName   [NAME_LENGTH];
                char lPrivate[NAME_LENGTH];
                char lPublic [NAME_LENGTH];
                char lRegEx  [NAME_LENGTH];

                if (1 == sscanf_s(lIt->c_str(), "ADDRESSES %s", lRegEx SizeInfo(lRegEx)))
                {
                    AddPublicAddresses(lRegEx);
                }
                else if (2 == sscanf_s(lIt->c_str(), "ADDRESS %[^ \n\r\t] %s", lPrivate SizeInfo(lPrivate), lPublic SizeInfo(lPublic)))
                {
                    AddPublicAddress(lPrivate, lPublic);
                }
                else if (1 == sscanf_s(lIt->c_str(), "ADDRESS %s", lName SizeInfo(lName)))
                {
                    AddPublicAddress(lName, lName);
                }
                else
                {
                    ::Console::Warning_IgnoredLine(lIt->GetUserLineNo(), lIt->c_str());
                }
            }
        }
        ::Console::Progress_End("Parsed");
    }

    void Project::ParsePublicDefs()
    {
        for (auto& lEntry : mPublicDefs.mInternal)
        {
            auto lSource = dynamic_cast<const DI::String*>(lEntry.Get());
            assert(nullptr != lSource);

            ParsePublicDef(lSource->Get());
        }
    }

    void Project::RemoveParsed()
    {
        unsigned int lLineNo = 1;

        lLineNo = RemoveRead(lLineNo); // Inputs
        lLineNo = RemoveRead(lLineNo); // Outputs
        lLineNo = RemoveRead(lLineNo); // Relays
        lLineNo = RemoveRead(lLineNo); // Timers
        lLineNo = RemoveRead(lLineNo); // Counters
        lLineNo = ParseNothing(lLineNo); // Circuits
        lLineNo = RemoveRead(lLineNo); // Functions
        lLineNo = RemoveRead(lLineNo); // Function names
        lLineNo = ParseNothing(lLineNo); // Quick tags
        lLineNo = RemoveRead(lLineNo); // Defines

        Reparse();
    }

    unsigned int Project::RemoveRead(unsigned int aLineNo)
    {
        assert(0 < aLineNo);

        auto lLineCount = mFile.GetLineCount();
        auto lLineNo    = aLineNo;

        while (lLineNo < lLineCount)
        {
            const wchar_t* lLine = mFile.GetLine(lLineNo);
            if (L'~' == lLine[0])
            {
                lLineNo++;
                break;
            }

            mFile.RemoveLines(lLineNo, 1);
            lLineCount--;
        }

        return lLineNo;
    }

    void Project::Reparse()
    {
        mCounters .ClearList();
        mCircuits .ClearList();
        mDefines  .ClearList();
        mFooter   .ClearList();
        mFunctions.ClearList();
        mInputs   .ClearList();
        mOutputs  .ClearList();
        mQuickTags.ClearList();
        mRelays   .ClearList();
        mTimers   .ClearList();

        mPublicAddresses.clear();

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
