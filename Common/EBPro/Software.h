
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/EBPro/Software.h
// Status    DEV

#pragma once

// ===== Import/Includes ====================================================
#include <KMS/Proc/Process.h>

namespace EBPro
{

    class Software
    {

    public:

        Software();

        ~Software();

        void ImportAddresses(const char *aImp, const char* aExp);
        void ImportFunctions(const char *aImp, const char* aExp);
        void ImportLabels   (const char *aImp, const char* aExp);

        void Open(const char* aFile);

    private:

        KMS::Proc::Process* mProcess;

    };

}
