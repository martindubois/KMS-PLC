
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/System.h

#pragma once

// ===== Import =============================================================
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

    void Verify();

    void Write();

    // ===== KMS::CLI::Tool =================================================
    virtual void DisplayHelp(FILE* aOut) const;
    virtual void ExecuteCommand(const char* aC);
    virtual int  Run();

private:

    NO_COPY(System);

    void ExecuteOperations();

    EBPro::Project   mEBPro;
    TRiLOGI::Project mTRiLOGI;

};
