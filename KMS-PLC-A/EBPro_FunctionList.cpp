
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/EBPro_FunctionList.cpp

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KMS/Cfg/MetaData.h>

// ===== Local ==============================================================
#include "../Common/EBPro/FunctionList.h"

using namespace KMS;

// Constants
// //////////////////////////////////////////////////////////////////////////

static const Cfg::MetaData MD_EXPORTED ("Exported = {Path}.mlb");
static const Cfg::MetaData MD_SOURCES  ("Sources += {Path}.mlb.in0");
static const Cfg::MetaData MD_TO_IMPORT("ToImport = {Path}.mlb");

namespace EBPro
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    FunctionList::FunctionList()
    {
        mSources.SetCreator(DI::String_Expand::Create);

        AddEntry("Exported", &mExported, false, &MD_EXPORTED);
        AddEntry("Sources" , &mSources , false, &MD_SOURCES);
        AddEntry("ToImport", &mToImport, false, &MD_TO_IMPORT);
    }

    FunctionList::~FunctionList()
    {
        // TODO
    }

    void FunctionList::Import()
    {
        // TODO
    }

    void FunctionList::Parse()
    {
        // TODO
    }

    void FunctionList::Read()
    {
        if (0 < mExported.GetLength())
        {
            std::cout << "Reading " << mExported.Get() << " ..." << std::endl;

            mFile.Read(File::Folder::CURRENT, mExported.Get());

            std::cout << "Read" << std::endl;
        }
    }

    void FunctionList::ValidateConfig() const
    {
        char lMsg[64 + PATH_LENGTH];

        if (0 < mExported.GetLength())
        {
            const char* lStr = mExported.Get();

            sprintf_s(lMsg, "\"%s\" does not exist", lStr);
            KMS_EXCEPTION_ASSERT(File::Folder::CURRENT.DoesFileExist(lStr), APPLICATION_USER_ERROR, lMsg, "");
        }

        for (const DI::Container::Entry& lEntry : mSources.mInternal)
        {
            const DI::String* lSource = dynamic_cast<const DI::String*>(lEntry.Get());
            assert(NULL != lSource);

            sprintf_s(lMsg, "\"%s\" does not exist", lSource->Get());
            KMS_EXCEPTION_ASSERT(File::Folder::CURRENT.DoesFileExist(lSource->Get()), APPLICATION_USER_ERROR, lMsg, "");
        }

        if (0 < mSources.GetCount())
        {
            KMS_EXCEPTION_ASSERT(0 < mToImport.GetLength(), APPLICATION_USER_ERROR, "Sources are present, ToImport cannot be empty", "");
        }
    }

    void FunctionList::Verify() const
    {
        // TODO
    }

}
