
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/EBPro_List.cpp

// TEST COVERAGE 2023-06-12 KMS - Martin Dubois, P. Eng.

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
static const Cfg::MetaData MD_SOURCES  ("Sources += {Path}.in0");
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

            std::cout << "Importing " << lSource->Get() << " ..." << std::endl;

            lChanged |= ImportSource(lSource->Get());

            std::cout << "Imported" << std::endl;
        }

        if (lChanged)
        {
            std::cout << "Saving " << mToImport.Get() << " ..." << std::endl;

            // NOT TESTED
            SaveToImport();
        }
        else
        {
            // If no Source are configured, we execute this code. This is why
            // we verify if the ToImport is configured.
            if ((0 < mToImport.GetLength()) && (File::Folder::CURRENT.DoesFileExist(mToImport.Get())))
            {
                std::cout << "Deleting " << mToImport.Get() << " ..." << std::endl;

                File::Folder lCurrent(File::Folder::Id::CURRENT);

                lCurrent.Delete(mToImport.Get());

                std::cout << "Deleted" << std::endl;
            }
        }
    }

    void List::Read()
    {
        if (IsExportedConfigured())
        {
            std::cout << "Reading " << GetExported() << " ..." << std::endl;

            ReadExported();

            std::cout << "Read" << std::endl;
        }
    }

    void List::ValidateConfig() const
    {
        char lMsg[64 + PATH_LENGTH];

        if (IsExportedConfigured())
        {
            auto lStr = GetExported();

            sprintf_s(lMsg, "\"%s\" does not exist", lStr);
            KMS_EXCEPTION_ASSERT(File::Folder::CURRENT.DoesFileExist(lStr), APPLICATION_USER_ERROR, lMsg, "");
        }

        for (auto& lEntry : mSources.mInternal)
        {
            auto lSource = dynamic_cast<const DI::String*>(lEntry.Get());
            assert(NULL != lSource);

            sprintf_s(lMsg, "\"%s\" does not exist", lSource->Get());
            KMS_EXCEPTION_ASSERT(File::Folder::CURRENT.DoesFileExist(lSource->Get()), APPLICATION_USER_ERROR, lMsg, "");
        }

        if (0 < mSources.GetCount())
        {
            KMS_EXCEPTION_ASSERT(0 < mToImport.GetLength(), APPLICATION_USER_ERROR, "Sources are present, ToImport cannot be empty", "");
        }
    }

    // Protected
    // //////////////////////////////////////////////////////////////////////

    const char* List::GetExported() const { return mExported.Get(); }
    Software  * List::GetSoftware()       { assert(NULL != mSoftware); return mSoftware; }
    const char* List::GetToImport() const { return mToImport.Get(); }

    bool List::IsExportedConfigured() const { return 0 < mExported.GetLength(); }

    void List::ReadExported() { assert(false); }

}
