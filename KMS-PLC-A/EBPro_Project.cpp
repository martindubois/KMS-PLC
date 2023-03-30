
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/EBPro_Project.cpp

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KMS/Cfg/MetaData.h>
#include <KMS/Exception.h>
#include <KMS/Proc/Process.h>

// ===== Local ==============================================================
#include "../Common/EBPro/Project.h"

using namespace KMS;

// Constants
// //////////////////////////////////////////////////////////////////////////

static const Cfg::MetaData MD_FILE_NAME_EMTP("FileName_emtp = {Path}");

namespace EBPro
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    Project::Project()
    {
        AddEntry("FileName_emtp", &mFileName_emtp, false, &MD_FILE_NAME_EMTP);

        AddEntry("Addresses", &mAddresses, false);
        AddEntry("Labels"   , &mLabels   , false);
    }

    void Project::Edit()
    {
        KMS_EXCEPTION_ASSERT(0 < mFileName_emtp.GetLength(), APPLICATION_USER_ERROR, "No file name configured", "");

        Proc::Process lP(File::Folder::CURRENT, mFileName_emtp.Get());

        lP.SetVerb("open");

        lP.Start();

        lP.Detach();
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
