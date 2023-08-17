
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/TRiLOGI/Project.h
// Status    DEV

#pragma once

// ===== Import =============================================================
#include <KMS/Callback.h>
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
#include "LineList.h"
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
        KMS::DI::Array         mSources;
        KMS::DI::String_Expand mToolConfig;

    private:

        NO_COPY(Project);

        void AddPublicAddress(const char* aPrivate, const char* aPublic);
        void AddPublicAddresses(const char* aRegEx);

        void BuildFile();

        void Create();

        void Export_HeaderFile();
        void Export_ToolConfig();

        bool Import_Source(const char* aFileName);

        void Parse_Legacy();
        void Parse_New();

        unsigned int ParseNothing(unsigned int aLineNo, unsigned int aSectionCount);

        void Reparse();

        void Verify_Counters  () const;
        void Verify_Defines   () const;
        void Verify_Functions () const;
        void Verify_Inputs    () const;
        void Verify_Outputs   () const;

        void Verify_BitList(const BitList& aBits) const;

        // ===== Callbacks ==================================================
        const KMS::Callback<Project> ON_PROJECT_TYPE_CHANGED;
        unsigned int OnProjectTypeChanged(void* aSender, void* aData);

        KMS::Text::File_UTF16 mFile;

        LineList      mCircuits;
        CounterList   mCounters;
        DefineList    mDefines;
        LineList      mFooter;
        FunctionList  mFunctions;
        BitList       mInputs;
        BitList       mOutputs;
        AddressList   mPublicAddresses;
        LineList      mQuickTags;
        BitList       mRelays;
        Software      mSoftware;
        TimerList     mTimers;

    };

}
