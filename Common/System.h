
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/System.h
// Status    DEV

#pragma once

// ===== Import =============================================================
#include <KMS/CLI/Macros.h>
#include <KMS/CLI/Tool.h>

// ===== Local ==============================================================
#include "EBPro/Project.h"
#include "TRiLOGI/Project.h"

class System : public KMS::CLI::Tool
{

public:

    static int Main(int aCount, const char** aVector);

    System();

    void Clean();

    void Edit_PC6();

    void Edit_emtp();

    void Export();

    void Import();

    void Read();

    void Parse();

    void Program_PLC();

    void Verify();

    void Write();

    // ===== KMS::CLI::Tool =================================================
    virtual void DisplayHelp(FILE* aOut) const;
    virtual int  ExecuteCommand(KMS::CLI::CommandLine* aCmd);
    virtual int  Run();

private:

    NO_COPY(System);

    int Cmd_Clean      (KMS::CLI::CommandLine* aCmd);
    int Cmd_Edit       (KMS::CLI::CommandLine* aCmd);
    int Cmd_Edit_emtp  (KMS::CLI::CommandLine* aCmd);
    int Cmd_Edit_PC6   (KMS::CLI::CommandLine* aCmd);
    int Cmd_Export     (KMS::CLI::CommandLine* aCmd);
    int Cmd_Import     (KMS::CLI::CommandLine* aCmd);
    int Cmd_Program    (KMS::CLI::CommandLine* aCmd);
    int Cmd_Program_PLC(KMS::CLI::CommandLine* aCmd);
    int Cmd_Verify     (KMS::CLI::CommandLine* aCmd);
    int Cmd_Write      (KMS::CLI::CommandLine* aCmd);

    EBPro::Project   mEBPro;
    TRiLOGI::Project mTRiLOGI;

    KMS::CLI::Macros mMacros;

};
