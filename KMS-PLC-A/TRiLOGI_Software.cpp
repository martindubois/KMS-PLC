
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI_Software.cpp

// TEST COVERAGE  2023-08-08  KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KMS/Proc/Process.h>

// ===== Local ==============================================================
#include "../Common/TRiLOGI/Software.h"

using namespace KMS;

namespace TRiLOGI
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    Software::Software() {}

    // NOT TESTED
    void Software::Edit(const char* aFile)
    {
        Proc::Process lP(File::Folder::CURRENT, aFile);

        lP.SetVerb("open");

        lP.Run(0xffffffff);
    }

}
