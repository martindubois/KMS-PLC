
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/System.cpp

// TEST COVERAGE  2023-08-14  KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KMS/CLI/CommandLine.h>
#include <KMS/Main.h>

// ===== Local ==============================================================
#include "../Common/System.h"

#include "EBPro/Address.h"

using namespace KMS;

//
// //////////////////////////////////////////////////////////////////////////

#define CONFIG_FILE ("KMS-PLC.cfg")

// Public
// //////////////////////////////////////////////////////////////////////////

int System::Main(int aCount, const char** aVector)
{
    KMS_MAIN_BEGIN;
    {
        System lS;

        lConfigurator.AddConfigurable(&lS);

        lConfigurator.ParseFile(File::Folder::CURRENT, CONFIG_FILE);

        KMS_MAIN_PARSE_ARGS(aCount, aVector);

        KMS_MAIN_VALIDATE;

        lResult = lS.Run();
    }
    KMS_MAIN_END;
    
    KMS_MAIN_RETURN;
}

System::System()
{
    Ptr_OF<DI::Object> lEntry;

    lEntry.Set(&mEBPro  , false); AddEntry("EBPro"  , lEntry);
    lEntry.Set(&mTRiLOGI, false); AddEntry("TRiLOGI", lEntry);
}

void System::Clean() { mTRiLOGI.Clean(); }

// NOT TESTED
void System::Edit_emtp() { mEBPro  .Edit(); }
void System::Edit_PC6 () { mTRiLOGI.Edit(); }

void System::Export()
{
    mEBPro  .Export();
    mTRiLOGI.Export();
}

void System::Import()
{
    mEBPro  .Import();
    mTRiLOGI.Import();

    const AddressList* lAL = mTRiLOGI.GetPublicAddresses();
    if ((nullptr != lAL) && (0 < lAL->size()))
    {
        mEBPro.Import(*lAL);
    }
}

void System::Parse()
{
    mEBPro  .Parse();
    mTRiLOGI.Parse();
}

void System::Program_PLC() { mTRiLOGI.Program(); }

void System::Read()
{
    mEBPro  .Read();
    mTRiLOGI.Read();
}

void System::Verify()
{
    mEBPro  .Verify();
    mTRiLOGI.Verify();

    if ((0 < mEBPro.mAddresses.GetCount()) && mTRiLOGI.IsValid())
    {
        ::Console::Progress_Begin("System", "Verifying shared addresses");
        {
            for (const auto lA : mEBPro.mAddresses.mAddresses)
            {
                // NOT TESTED
                switch (lA->GetType())
                {
                case AddressType::MODBUS_RTU_1X:
                    if (!mTRiLOGI.VerifyAddress_1X(lA->GetAddress_UInt16()))
                    {
                        ::Console::Error_Begin()
                            << "The HMI access an invalid PLC address named \""
                            << lA->GetName() << "\" (1x-" << lA->GetAddress() << ") (NOT TESTED)";
                        ::Console::Error_End();
                    }
                    break;

                case AddressType::MODBUS_RTU_4X:
                    if (!mTRiLOGI.VerifyAddress_4X(lA->GetAddress_UInt16()))
                    {
                        ::Console::Error_Begin()
                            << "The HMI access an invalid PLC address named \""
                            << lA->GetName() << "\" (4x-" << lA->GetAddress() << ") (NOT TESTED)";
                        ::Console::Error_End();
                    }
                    break;
                }
            }
        }
        ::Console::Progress_End("Verifyed");
    }
}

void System::Write() { mTRiLOGI.Write(); }

// ===== CLI::Tool ==========================================================

void System::DisplayHelp(FILE* aOut) const
{
    assert(nullptr != aOut);

    fprintf(aOut,
        "Clean\n"
        "Edit emtp | PC6\n"
        "Export\n"
        "Import\n"
        "Program PLC\n"
        "Verify\n"
        "Write\n");

    CLI::Tool::DisplayHelp(aOut);
}

int System::ExecuteCommand(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    int lResult = __LINE__;

    auto lCmd = aCmd->GetCurrent();

    if      (0 == _stricmp("Clean"  , lCmd)) { aCmd->Next(); lResult = Cmd_Clean  (aCmd); }
    else if (0 == _stricmp("Edit"   , lCmd)) { aCmd->Next(); lResult = Cmd_Edit   (aCmd); }
    else if (0 == _stricmp("Export" , lCmd)) { aCmd->Next(); lResult = Cmd_Export (aCmd); }
    else if (0 == _stricmp("Import" , lCmd)) { aCmd->Next(); lResult = Cmd_Import (aCmd); }
    else if (0 == _stricmp("Program", lCmd)) { aCmd->Next(); lResult = Cmd_Program(aCmd); }
    else if (0 == _stricmp("Verify" , lCmd)) { aCmd->Next(); lResult = Cmd_Verify (aCmd); }
    else if (0 == _stricmp("Write"  , lCmd)) { aCmd->Next(); lResult = Cmd_Write  (aCmd); }
    else
    {
        lResult = CLI::Tool::ExecuteCommand(aCmd);
    }

    return lResult;
}

int System::Run()
{
    Read  ();
    Parse ();

    return CLI::Tool::Run();
}

// Private
// //////////////////////////////////////////////////////////////////////////

int System::Cmd_Clean(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many arguments for the Clean command", aCmd->GetCurrent());

    Clean();

    return 0;
}

int System::Cmd_Edit(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    int lResult = __LINE__;

    auto lCmd = aCmd->GetCurrent();

    if      (0 == _stricmp("emtp", lCmd)) { aCmd->Next(); lResult = Cmd_Edit_emtp(aCmd); }
    else if (0 == _stricmp("PC6" , lCmd)) { aCmd->Next(); lResult = Cmd_Edit_PC6 (aCmd); }
    else
    {
        KMS_EXCEPTION(RESULT_INVALID_COMMAND, "Invalid Edit command", lCmd);
    }

    return 0;
}

int System::Cmd_Edit_emtp(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many arguments for the Edit emtp command", aCmd->GetCurrent());

    Edit_emtp();

    return 0;
}

int System::Cmd_Edit_PC6(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many arguments for the Edit PC6 command", aCmd->GetCurrent());

    Edit_PC6();

    return 0;
}

int System::Cmd_Export(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many arguments for the Export command", aCmd->GetCurrent());

    Export();

    return 0;
}

int System::Cmd_Import(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many arguments for the Import command", aCmd->GetCurrent());

    Import();

    return 0;
}

int System::Cmd_Program(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    int lResult = __LINE__;

    auto lCmd = aCmd->GetCurrent();

    if (0 == _stricmp("PLC", lCmd)) { aCmd->Next(); lResult = Cmd_Program_PLC(aCmd); }
    else
    {
        KMS_EXCEPTION(RESULT_INVALID_COMMAND, "Invalid Program command", lCmd);
    }

    return 0;
}

int System::Cmd_Program_PLC(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many arguments for the Program PLC command", aCmd->GetCurrent());

    Program_PLC();

    return 0;
}

int System::Cmd_Verify(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many arguments for the Verify command", aCmd->GetCurrent());

    Verify();

    return 0;
}

int System::Cmd_Write(CLI::CommandLine* aCmd)
{
    assert(nullptr != aCmd);

    KMS_EXCEPTION_ASSERT(aCmd->IsAtEnd(), RESULT_INVALID_COMMAND, "Too many command arguments for the Write command", aCmd->GetCurrent());

    Write();

    return 0;
}
