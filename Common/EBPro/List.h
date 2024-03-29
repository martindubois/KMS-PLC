
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/EBPro/List.h
// Status    PROD_READY

#pragma once

// ===== Import =============================================================
#include <KMS/DI/Array.h>
#include <KMS/DI/Dictionary.h>
#include <KMS/DI/String_Expand.h>

namespace EBPro
{

    class Software;

    class List : public KMS::DI::Dictionary
    {

    private:

        static const char* EXPORTED_DEFAULT;
        static const char* TO_IMPORT_DEFAULT;

        KMS::DI::String_Expand mExported;
        KMS::DI::String_Expand mToImport;
        KMS::DI::Array         mSources;

    public:

        List(Software* aSoftware);

        virtual ~List();

        void Import();

        virtual void Read();

        // ===== DI::Container ==============================================
        virtual void Validate() const;

    protected:

        const char* GetExported() const;
        Software  * GetSoftware();
        const char* GetToImport() const;

        bool IsExportedConfigured() const;
        bool IsToImportConfigured() const;

        virtual bool ImportSource(const char* aFileName) = 0;
        virtual void ReadExported();
        virtual void SaveToImport() = 0;

        virtual void ValidateConfig_ToImport() const;

    private:

        NO_COPY(List);

        Software* mSoftware;

    };

}
