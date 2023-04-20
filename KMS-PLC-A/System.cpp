
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/System.cpp

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KMS/Cfg/Configurator.h>
#include <KMS/Console/Color.h>
#include <KMS/Dbg/Log.h>
#include <KMS/Dbg/Stats.h>
#include <KMS/Dbg/Stats_Timer.h>

// ===== Local ==============================================================
#include "../Common/EBPro/Address.h"

#include "../Common/System.h"

using namespace KMS;

//
// //////////////////////////////////////////////////////////////////////////

#define CONFIG_FILE ("KMS-PLC.cfg")

// Public
// //////////////////////////////////////////////////////////////////////////

int System::Main(int aCount, const char** aVector)
{
    assert(1 <= aCount);
    assert(NULL != aVector);

    int lResult = __LINE__;

    auto lET = new Dbg::Stats_Timer("ExecutionTime");
    lET->Start();

    try
    {
        Cfg::Configurator lC;
        System            lS;

        lC.AddConfigurable(&lS);

        lC.AddConfigurable(&Dbg::gLog);
        lC.AddConfigurable(&Dbg::gStats);

        lC.ParseFile(File::Folder::CURRENT, CONFIG_FILE);

        lC.ParseArguments(aCount - 1, aVector + 1);

        lResult = lS.Run();
    }
    KMS_CATCH_RESULT(lResult);

    lET->Stop();
    
    return lResult;
}

System::System()
{
    AddEntry("EBPro"     , &mEBPro     , false);
    AddEntry("TRiLOGY"   , &mTRiLOGY   , false);
}

void System::Clean()
{
    mTRiLOGY.Clean();
}

void System::Edit_emtp() { mEBPro  .Edit(); }
void System::Edit_PC6 () { mTRiLOGY.Edit(); }

void System::Export()
{
    mEBPro  .Export();
    mTRiLOGY.Export();
}

void System::Import()
{
    mEBPro  .Import();
    mTRiLOGY.Import();

    AddressList* lAL = mTRiLOGY.GetSharedAddresses();
    if ((NULL != lAL) && (0 < lAL->size()))
    {
        mEBPro.Import(*lAL);
    }
}

void System::Parse()
{
    mEBPro  .Parse();
    mTRiLOGY.Parse();
}

void System::Read()
{
    mEBPro  .Read();
    mTRiLOGY.Read();
}

void System::Verify()
{
    mEBPro  .Verify();
    mTRiLOGY.Verify();

    if ((0 < mEBPro.mAddresses.GetCount()) && mTRiLOGY.IsValid())
    {
        std::cout << "Verifying shared addresses ..." << std::endl;

        for (const EBPro::Address* lA : mEBPro.mAddresses.mAddresses)
        {
            switch (lA->GetType())
            {
            case AddressType::MODBUS_RTU_1X:
                if (!mTRiLOGY.VerifyAddress_1X(lA->GetAddress_UInt16()))
                {
                    std::cout << Console::Color::RED << "    ERROR  The HMI access an invalid PLC address named \"";
                    std::cout << lA->GetName() << "\" (1x-" << lA->GetAddress() << ")" << Console::Color::WHITE << std::endl;
                }
                break;

            case AddressType::MODBUS_RTU_4X:
                if (!mTRiLOGY.VerifyAddress_4X(lA->GetAddress_UInt16()))
                {
                    std::cout << Console::Color::RED << "    ERROR  The HMI access an invalid PLC address named \"";
                    std::cout << lA->GetName() << "\" (4x-" << lA->GetAddress() << ")" << Console::Color::WHITE << std::endl;
                }
                break;
            }
        }

        std::cout << "Verifyed" << std::endl;
    }
}

void System::Write()
{
    mTRiLOGY.Write();
}

// ===== CLI::Tool ==========================================================

void System::DisplayHelp(FILE* aOut) const
{
    assert(NULL != aOut);

    fprintf(aOut,
        "Clean\n"
        "Edit emtp | PC6\n"
        "Export\n"
        "Import\n"
        "Write\n");

    CLI::Tool::DisplayHelp(aOut);
}

void System::ExecuteCommand(const char* aC)
{
    assert(NULL != aC);

    if      (0 == strcmp("Clean"    , aC)) { Clean    (); }
    else if (0 == strcmp("Edit emtp", aC)) { Edit_emtp(); }
    else if (0 == strcmp("Edit PC6" , aC)) { Edit_PC6 (); }
    else if (0 == strcmp("Export"   , aC)) { Export   (); }
    else if (0 == strcmp("Import"   , aC)) { Import   (); }
    else if (0 == strcmp("Verify"   , aC)) { Verify   (); }
    else if (0 == strcmp("Write"    , aC)) { Write    (); }
    else
    {
        CLI::Tool::ExecuteCommand(aC);
    }
}

int System::Run()
{
    mEBPro  .ValidateConfig();
    mTRiLOGY.ValidateConfig();

    Read  ();
    Parse ();
    Verify();

    return CLI::Tool::Run();
}
