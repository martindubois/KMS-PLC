
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
#include "../Common/TRiLOGI/Project.h"
#include "../Common/Version.h"

#include "TRiLOGI/Word.h"

using namespace KMS;

// Constants
// //////////////////////////////////////////////////////////////////////////

static const Cfg::MetaData MD_CREATE_IF_NEEDED("CreateIfNeeded = false | true");
static const Cfg::MetaData MD_EXPORTED        ("Exported = {Path}.PC6.txt");
static const Cfg::MetaData MD_FILE_NAME       ("FileName = {Path}.PC6");
static const Cfg::MetaData MD_HEADER_FILE     {"HeaderFile = {Path}.h"};
static const Cfg::MetaData MD_HEADER_PREFIX   {"HeaderPrefix = {Prefix}"};
static const Cfg::MetaData MD_PUBLIC_DEFS     ("PublicDefs += {Path}.txt");
static const Cfg::MetaData MD_SOURCES         ("Sources += {Path}.PC6.txt");

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

static void Instruction_Write();

namespace TRiLOGI
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    Project::Project() : mInputs("input", 1, 256), mOutputs("output", 1, 256), mRelays("relay", 1025, 512)
    {
        mPublicDefs.SetCreator(DI::String_Expand::Create);
        mSources   .SetCreator(DI::String_Expand::Create);

        AddEntry("CreateIfNeeded", &mCreateIfNeeded, false, &MD_CREATE_IF_NEEDED);
        AddEntry("Exported"      , &mExported      , false, &MD_EXPORTED);
        AddEntry("FileName"      , &mFileName      , false, &MD_FILE_NAME);
        AddEntry("HeaderFile"    , &mHeaderFile    , false, &MD_HEADER_FILE);
        AddEntry("HeaderPrefix"  , &mHeaderPrefix  , false, &MD_HEADER_PREFIX);
        AddEntry("PublicDefs"    , &mPublicDefs    , false, &MD_PUBLIC_DEFS);
        AddEntry("Sources"       , &mSources       , false, &MD_SOURCES);
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

            mFile.Write_ASCII(File::Folder::CURRENT, mExported.Get());

            ::Console::Progress_End("Exported");
        }

        if (0 < mHeaderFile.GetLength())
        {
            auto lFileName = mHeaderFile.Get();
            assert(nullptr != lFileName);

            std::ofstream lFile(lFileName);
            if (!lFile.is_open())
            {
                char lMsg[64 + PATH_LENGTH];
                sprintf_s(lMsg, "Cannot open \"%s\" for writing (NOT TESTED)", lFileName);
                KMS_EXCEPTION(APPLICATION_USER_ERROR, lMsg, "");
            }

            lFile << "\n";
            lFile << "// File  " << lFileName << "\n";
            lFile << "\n";
            lFile << "// Generated be KMS-PLC.exe version " << VERSION;
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
    }

    void Project::Import()
    {
        ::Console::Progress_Begin("TRiLOGY", "Importing sources");

        auto lChanged = false;

        for (const auto& lEntry : mSources.mInternal)
        {
            auto lSource = dynamic_cast<const DI::String*>(lEntry.Get());
            assert(nullptr != lSource);

            Text::File_ASCII lFile;

            lFile.Read(File::Folder::CURRENT, lSource->Get());

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
                    lChanged |= mCounters.Import(lName, lInit);
                }
                else if (2 == sscanf_s(lIt->c_str(), "CONSTANT %[0-9A-Za-z_] %s", lName SizeInfo(lName), lValue SizeInfo(lValue)))
                {
                    lChanged |= mDefines.ImportConstant(lName, lValue);
                }
                else if (1 == sscanf_s(lIt->c_str(), "FUNCTION %[0-9A-Za-z_]", lName SizeInfo(lName)))
                {
                    lChanged |= mFunctions.Import(lName, &lFile, &lIt);
                }
                else if (2 == sscanf_s(lIt->c_str(), "INPUT %[0-9A-Za-z_] %u", lName SizeInfo(lName), &lIndex))
                {
                    lChanged |= mInputs.Import(lName, lIndex);
                }
                else if (2 == sscanf_s(lIt->c_str(), "OUTPUT %[0-9A-Za-z_] %u", lName SizeInfo(lName), &lIndex))
                {
                    lChanged |= mOutputs.Import(lName, lIndex);
                }
                else if (2 == sscanf_s(lIt->c_str(), "RELAY %[0-9A-Za-z_] %u", lName SizeInfo(lName), &lIndex))
                {
                    lChanged |= mRelays.Import(lName, lIndex);
                }
                else if (1 == sscanf_s(lIt->c_str(), "RELAY %[0-9A-Za-z_]", lName SizeInfo(lName)))
                {
                    lChanged |= mRelays.Import(lName);
                }
                else if (2 == sscanf_s(lIt->c_str(), "TIMER %[0-9A-Za-z_] %u", lName SizeInfo(lName), &lInit))
                {
                    lChanged |= mTimers.Import(lName, lInit);
                }
                else if (2 == sscanf_s(lIt->c_str(), "WORD %[0-9A-Za-z_] %u", lName SizeInfo(lName), &lOffset))
                {
                    lChanged |= mDefines.ImportWord(lName, lOffset);
                }
                else if (1 == sscanf_s(lIt->c_str(), "WORD %[0-9A-Za-z_]", lName SizeInfo(lName)))
                {
                    lChanged |= mDefines.ImportWord(lName);
                }
                else
                {
                    ::Console::Warning_IgnoredLine(lIt->GetUserLineNo(), lIt->c_str());
                }
            }
        }

        if (lChanged)
        {
            if (Apply())
            {
                Reparse();
            }

            ::Console::Progress_End("Imported");

            Instruction_Write();
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

            unsigned int lLineNo    = 1;
            auto         lLineCount = mFile.GetLineCount();

            lLineNo = mInputs  .Parse(&mFile, lLineNo, TRiLOGI::Object::FLAG_SINGLE_USE_INFO);
            lLineNo = mOutputs .Parse(&mFile, lLineNo, TRiLOGI::Object::FLAG_SINGLE_USE_INFO);
            lLineNo = mRelays  .Parse(&mFile, lLineNo, TRiLOGI::Object::FLAG_SINGLE_USE_INFO);
            lLineNo = mTimers  .Parse(&mFile, lLineNo, TRiLOGI::Object::FLAG_SINGLE_USE_WARNING);
            lLineNo = mCounters.Parse(&mFile, lLineNo, TRiLOGI::Object::FLAG_SINGLE_USE_WARNING);

            // Circuits
            lLineNo = ParseNothing(lLineNo);

            lLineNo = mFunctions.Parse_Code(&mFile, lLineNo);
            lLineNo = mFunctions.Parse_Name(&mFile, lLineNo);

            // Quick tags
            lLineNo = ParseNothing(lLineNo);

            lLineNo = mDefines.Parse(&mFile, lLineNo);

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

            mFile.Read(File::Folder::CURRENT, mFileName.Get());

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
    }

    void Project::Verify()
    {
        if (0 < mFile.GetLineCount())
        {
            ::Console::Progress_Begin("TRiLOGY", "Verifying", mFileName.Get());

            // TODO For the verification, use a version of mFile_PC6 without
            //      comment

            mCounters  .Verify(mFile);
            mDefines   .Verify(mFile);
            mFunctions .Verify(mFile);
            mInputs    .Verify(mFile);
            mOutputs   .Verify(mFile);
            mRelays    .Verify(mFile);
            mTimers    .Verify(mFile);

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

        if (File::Folder::CURRENT.DoesFileExist(mFileName.Get()))
        {
            File::Folder::CURRENT.Backup(mFileName.Get(), File::Folder::FLAG_BACKUP_RENAME);
        }

        mFile.Write(File::Folder::CURRENT, mFileName.Get(), L"\r\n");

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

    void Project::Reparse()
    {
        mCounters  .Clear();
        mDefines   .Clear();
        mFunctions .Clear();
        mInputs    .Clear();
        mOutputs   .Clear();
        mRelays    .Clear();
        mTimers    .Clear();

        mPublicAddresses.clear();

        Parse();
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
