
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGY_Software.cpp

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KMS/Proc/Process.h>

// ===== Local ==============================================================
#include "../Common/TRiLOGY/Software.h"

using namespace KMS;

namespace TRiLOGY
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    Software::Software() {}

    void Software::Edit(const char* aFile)
    {
        Proc::Process lP(File::Folder::CURRENT, aFile);

        lP.SetVerb("open");

        lP.Run(0xffffffff);
    }

}
