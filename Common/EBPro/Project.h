
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/EBPro/Project.h

#pragma once

// ===== Local ==============================================================
#include "../Address.h"
#include "AddressList.h"
#include "LabelList.h"

namespace EBPro
{

    class Project : public KMS::DI::Dictionary
    {

    public:

        Project();

        void Export() const;
        
        void Import();

        void Import(const ::AddressList& aAL);

        void Parse();

        void Read();

        void ValidateConfig() const;

        void Verify() const;

        void Write() const;

        AddressList mAddresses;

    private:

        LabelList mLabels;

    };

};
