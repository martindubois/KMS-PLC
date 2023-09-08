
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
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

static const Cfg::MetaData MD_FILE_NAME("FileName = {Path}.emtp");

namespace EBPro
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    Project::Project() : mAddresses(&mSoftware), mFunctions(&mSoftware), mLabels(&mSoftware)
    {
        AddEntry("FileName", &mFileName, false, &MD_FILE_NAME);

        AddEntry("Addresses", &mAddresses, false);
        AddEntry("Functions", &mFunctions, false);
        AddEntry("Labels"   , &mLabels   , false);
    }

    void Project::Edit()
    {
        KMS_EXCEPTION_ASSERT(0 < mFileName.GetLength(), RESULT_INVALID_CONFIG, "No file name configured", "");

        mSoftware.Open(mFileName.Get());
    }

    void Project::Export() const
    {
        mLabels.Export();
    }

    void Project::Import()
    {
        mAddresses.Import();
        mFunctions.Import();
        mLabels   .Import();
    }

    void Project::Import(const ::AddressList& aAL)
    {
        mAddresses.ImportAddresses(aAL);
    }

    void Project::Parse()
    {
        mAddresses.Parse();
        mFunctions.Parse();
        mLabels   .Parse();
    }

    void Project::Read()
    {
        mAddresses.Read();
        mFunctions.Read();
        mLabels   .Read();
    }

    void Project::Verify() const
    {
        mAddresses.Verify();
        mLabels   .Verify();
    }

}
