
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/System.cpp

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KMS/Cfg/Configurator.h>
#include <KMS/Console/Color.h>
#include <KMS/Dbg/Log.h>

// ===== Local ==============================================================
#include "../Common/EBPro/Address.h"

#include "../Common/System.h"

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

    try
    {
        KMS::Cfg::Configurator lC;
        System                 lS;

        lC.AddConfigurable(&lS);

        lC.AddConfigurable(&KMS::Dbg::gLog);

        lC.ParseFile(KMS::File::Folder(KMS::File::Folder::Id::CURRENT), CONFIG_FILE);

        lC.ParseArguments(aCount - 1, aVector + 1);

        KMS::Dbg::gLog.CloseLogFiles();

        lResult = lS.Run();
    }
    KMS_CATCH_RESULT(lResult);
    
    return 0;
}

System::System()
{
    mOperations.SetCreator(KMS::DI::String::Create);

    AddEntry("EBPro"     , &mEBPro     , false);
    AddEntry("Operations", &mOperations, false);
    AddEntry("TRiLOGY"   , &mTRiLOGY   , false);
}

void System::Clean()
{
    // TODO
}

void System::Export() const
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
                    std::cout << KMS::Console::Color::RED << "    ERROR  The HMI access an invalid PLC address named ";
                    std::cout << lA->GetName() << " (1x-" << lA->GetAddress() << ")" << KMS::Console::Color::WHITE << std::endl;
                }
                break;

            case AddressType::MODBUS_RTU_4X:
                if (!mTRiLOGY.VerifyAddress_4X(lA->GetAddress_UInt16()))
                {
                    std::cout << KMS::Console::Color::RED << "    ERROR  The HMI access an invalid PLC address named ";
                    std::cout << lA->GetName() << " (4x-" << lA->GetAddress() << ")" << KMS::Console::Color::WHITE << std::endl;
                }
                break;
            }
        }

        std::cout << "Verifyed" << std::endl;
    }
}

void System::Write()
{
    mEBPro  .Write();
    mTRiLOGY.Write();
}

// ===== KMS::CLI::Tool =====================================================

void System::DisplayHelp(FILE* aOut) const
{
    assert(NULL != aOut);

    fprintf(aOut,
        "Clean\n"
        "Export\n"
        "Import\n"
        "Write\n");

    KMS::CLI::Tool::DisplayHelp(aOut);
}

void System::ExecuteCommand(const char* aC)
{
    assert(NULL != aC);

    if      (0 == strcmp("Clean" , aC)) { Clean (); }
    else if (0 == strcmp("Export", aC)) { Export(); }
    else if (0 == strcmp("Import", aC)) { Import(); }
    else if (0 == strcmp("Write" , aC)) { Write (); }
    else
    {
        KMS::CLI::Tool::ExecuteCommand(aC);
    }
}

int System::Run()
{
    mEBPro  .ValidateConfig();
    mTRiLOGY.ValidateConfig();

    Read  ();
    Parse ();
    Verify();

    return KMS::CLI::Tool::Run();
}
