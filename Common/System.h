
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/System.h

#pragma once

// ===== Import =============================================================
#include <KMS/DI/Dictionary.h>

// ===== Local ==============================================================
#include "EBPro/Project.h"
#include "TRiLOGY/Project.h"

class System : public KMS::DI::Dictionary
{

public:

    static int Main(int aCount, const char** aVector);

    System();

    void Clean();

    void Export() const;

    void Import();

    void Read();

    void Parse();

    int Run();

    void Verify();

    void Write() const;

private:

    System(const System&);

    const System& operator = (const System&);

    void ExecuteOperations();

    EBPro::Project   mEBPro;
    TRiLOGY::Project mTRiLOGY;

    // ===== Configurable attributes ========================================
    KMS::DI::Array mOperations;

};
