
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/EBPro_Project.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "../Common/EBPro/Project.h"

namespace EBPro
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    Project::Project()
    {
        AddEntry("Addresses", &mAddresses, false);
        AddEntry("Labels"   , &mLabels   , false);
    }

    void Project::Export() const
    {
        mLabels.Export();
    }

    void Project::Import()
    {
        mLabels.Import();
    }

    void Project::Import(const ::AddressList& aAL)
    {
        mAddresses.Import(aAL);
    }

    void Project::Parse()
    {
        mAddresses.Parse();
        mLabels   .Parse();
    }

    void Project::Read()
    {
        mAddresses.Read();
        mLabels   .Read();
    }

    void Project::ValidateConfig() const
    {
        mAddresses.ValidateConfig();
        mLabels   .ValidateConfig();
    }

    void Project::Verify() const
    {
        mAddresses.Verify();
        mLabels   .Verify();
    }

    void Project::Write() const
    {
        mLabels.Write();
    }

}
