
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/TRiLOGI/Project.h
// Status    DEV

#pragma once

// ===== Import =============================================================
#include <KMS/DI/Array.h>
#include <KMS/DI/Boolean.h>
#include <KMS/DI/Dictionary.h>
#include <KMS/DI/Enum.h>
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

        const AddressList* GetPublicAddresses() const;

        // ===== Configurable attributes ====================================
        KMS::DI::Boolean       mCreateIfNeeded;
        KMS::DI::String_Expand mExported;
        KMS::DI::String_Expand mFileName;
        KMS::DI::String_Expand mHeaderFile;
        KMS::DI::String_Expand mHeaderPrefix;
        KMS::DI::Enum<ProjectType, PROJECT_TYPE_NAMES> mProjectType;
        KMS::DI::Array         mPublicDefs;
        KMS::DI::Array         mSources;
        KMS::DI::String_Expand mToolConfig;

    private:

        NO_COPY(Project);

        void AddPublicAddress(const char* aPrivate, const char* aPublic);
        void AddPublicAddresses(const char* aRegEx);

        bool Apply();

        void Create();

        void Export_HeaderFile();
        void Export_ToolConfig();

        unsigned int ParseNothing(unsigned int aLineNo);

        void ParsePublicDef(const char* aFileName);
        void ParsePublicDefs();

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
        AddressList   mPublicAddresses;
        BitList       mRelays;
        Software      mSoftware;
        TimerList     mTimers;

    };

}
