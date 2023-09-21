
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/EBPro/Project.h
// Status    DEV

#pragma once

// ===== Local ==============================================================
#include "AddressList.h"
#include "FunctionList.h"
#include "LabelList.h"
#include "Software.h"

namespace EBPro
{

    class Project : public KMS::DI::Dictionary
    {

    public:

        static const char* FILE_NAME_DEFAULT;

        KMS::DI::String_Expand mFileName;

        Project();

        void Edit();

        void Export() const;
        
        void Import();

        void Import(const ::AddressList& aAL);

        void Parse();

        void Read();

        void Verify() const;

        AddressList mAddresses;

    private:

        LabelList    mLabels;
        FunctionList mFunctions;
        Software     mSoftware;

    };

};
