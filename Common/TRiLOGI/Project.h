
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/TRiLOGI/Project.h

#pragma once

// ===== Import =============================================================
#include <KMS/DI/Array.h>
#include <KMS/DI/Dictionary.h>
#include <KMS/DI/String_Expand.h>
#include <KMS/Text/File_UTF16.h>

// ===== Local ==============================================================
#include "BitList.h"
#include "CounterList.h"
#include "DefineList.h"
#include "FunctionList.h"
#include "Software.h"
#include "TimerList.h"

namespace TRiLOGI
{

    class Project : public KMS::DI::Dictionary
    {

    public:

        Project();

        bool IsValid() const;

        void Clean();

        void Edit();

        void Export();

        void Import();

        void Parse();

        void Read();

        void ValidateConfig() const;

        void Verify();

        bool VerifyAddress_1X(uint16_t aAddr) const;
        bool VerifyAddress_4X(uint16_t aAddr) const;

        void Write();

        AddressList* GetSharedAddresses() const;

    private:

        NO_COPY(Project);

        bool Apply();

        void Reparse();

        void Verify_Counters  () const;
        void Verify_Defines   () const;
        void Verify_Functions () const;
        void Verify_Inputs    () const;
        void Verify_Outputs   () const;

        void Verify_BitList(const BitList& aBits) const;

        KMS::Text::File_UTF16 mFile;

        CounterList   mCounters;
        DefineList    mDefines;
        FunctionList  mFunctions;
        BitList       mInputs;
        BitList       mOutputs;
        BitList       mRelays;
        Software      mSoftware;
        TimerList     mTimers;

        // ===== Configurable attributes ====================================
        KMS::DI::String_Expand mExported;
        KMS::DI::String_Expand mFileName;
        KMS::DI::String        mSharedAddressRegEx;
        KMS::DI::Array         mSources;

    };

}
