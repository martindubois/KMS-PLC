
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/EBPro_List.cpp

// TEST  COVERAGE 2023-08-06  KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KMS/Cfg/MetaData.h>

// ===== Local ==============================================================
#include "../Common/EBPro/Software.h"

#include "../Common/EBPro/List.h"

using namespace KMS;

// Constants
// //////////////////////////////////////////////////////////////////////////

static const Cfg::MetaData MD_EXPORTED ("Exported = {Path}");
static const Cfg::MetaData MD_SOURCES  ("Sources += {Path}.txt");
static const Cfg::MetaData MD_TO_IMPORT("ToImport = {Path}");

namespace EBPro
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    List::List(Software* aSoftware) : mSoftware(aSoftware)
    {
        assert(NULL != aSoftware);

        mSources.SetCreator(DI::String_Expand::Create);

        AddEntry("Exported", &mExported, false, &MD_EXPORTED);
        AddEntry("Sources" , &mSources , false, &MD_SOURCES);
        AddEntry("ToImport", &mToImport, false, &MD_TO_IMPORT);
    }

    List::~List() {}

    void List::Import()
    {
        auto lChanged = false;

        for (const auto& lEntry : mSources.mInternal)
        {
            auto lSource = dynamic_cast<const DI::String*>(lEntry.Get());
            assert(NULL != lSource);

            ::Console::Progress_Begin("EBPro", "Importing", lSource->Get());

            lChanged |= ImportSource(lSource->Get());

            ::Console::Progress_End("Imported");
        }

        if (lChanged)
        {
            ::Console::Progress_Begin("EBPro", "Saving (NOT TESTED)", mToImport.Get());

            SaveToImport();
        }
        else
        {
            // If no Source are configured, we execute this code. This is why
            // we verify if the ToImport is configured.
            if (IsToImportConfigured() && File::Folder::CURRENT.DoesFileExist(mToImport.Get()))
            {
                ::Console::Progress_Begin("EBPro", "Deleting (NOT TESTED)", mToImport.Get());

                File::Folder lCurrent(File::Folder::Id::CURRENT);

                lCurrent.Delete(mToImport.Get());

                ::Console::Progress_End("Deleted");
            }
        }
    }

    void List::Read()
    {
        if (IsExportedConfigured())
        {
            ::Console::Progress_Begin("EBPro", "Reading", GetExported());

            ReadExported();

            ::Console::Progress_End("Read");
        }
    }

    void List::ValidateConfig() const
    {
        char lMsg[64 + PATH_LENGTH];

        if (IsExportedConfigured())
        {
            auto lStr = GetExported();

            if (!File::Folder::CURRENT.DoesFileExist(lStr))
            {
                sprintf_s(lMsg, "\"%s\" does not exist (NOT TESTED)", lStr);
                KMS_EXCEPTION(APPLICATION_USER_ERROR, lMsg, "");
            }
        }

        for (auto& lEntry : mSources.mInternal)
        {
            auto lSource = dynamic_cast<const DI::String*>(lEntry.Get());
            assert(NULL != lSource);

            if (!File::Folder::CURRENT.DoesFileExist(lSource->Get()))
            {
                sprintf_s(lMsg, "\"%s\" does not exist (NOT TESTED)", lSource->Get());
                KMS_EXCEPTION(APPLICATION_USER_ERROR, lMsg, "");
            }
        }

        if (0 < mSources.GetCount())
        {
            ValidateConfig_ToImport();
        }
    }

    // Protected
    // //////////////////////////////////////////////////////////////////////

    const char* List::GetExported() const { return mExported.Get(); }
    Software  * List::GetSoftware()       { assert(NULL != mSoftware); return mSoftware; }
    const char* List::GetToImport() const { return mToImport.Get(); }

    bool List::IsExportedConfigured() const { return 0 < mExported.GetLength(); }
    bool List::IsToImportConfigured() const { return 0 < mToImport.GetLength(); }

    void List::ReadExported() { assert(false); }

    void List::ValidateConfig_ToImport() const
    {
        KMS_EXCEPTION_ASSERT(IsToImportConfigured(), APPLICATION_USER_ERROR, "ToImport cannot be empty in this configuration", "");
    }

}
