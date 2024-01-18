
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/System.cpp

// TEST COVERAGE  2023-08-14  KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== Import/Includes ====================================================
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
    AddEntry("EBPro"  , &mEBPro  , false);
    AddEntry("TRiLOGI", &mTRiLOGI, false);
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

int System::ExecuteCommand(const char* aC)
{
    assert(nullptr != aC);

    int lResult = 0;

    if      (0 == strcmp("Clean"      , aC)) { Clean      (); }
    else if (0 == strcmp("Edit emtp"  , aC)) { Edit_emtp  (); }
    else if (0 == strcmp("Edit PC6"   , aC)) { Edit_PC6   (); }
    else if (0 == strcmp("Export"     , aC)) { Export     (); }
    else if (0 == strcmp("Import"     , aC)) { Import     (); }
    else if (0 == strcmp("Program PLC", aC)) { Program_PLC(); }
    else if (0 == strcmp("Verify"     , aC)) { Verify     (); }
    else if (0 == strcmp("Write"      , aC)) { Write      (); }
    else
    {
        lResult = CLI::Tool::ExecuteCommand(aC);
    }

    return lResult;
}

int System::Run()
{
    Read  ();
    Parse ();

    return CLI::Tool::Run();
}
