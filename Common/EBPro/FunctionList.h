
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/EBPro/FunctionList.h

#pragma once

// ===== Import =============================================================
#include <KMS/DI/Array.h>
#include <KMS/DI/Dictionary.h>
#include <KMS/DI/String_Expand.h>
#include <KMS/Text/File_ASCII.h>

namespace EBPro
{

    class Function;

    class FunctionList : public KMS::DI::Dictionary
    {

    public:

        typedef std::map<std::string, Function*> ByName;

        FunctionList();

        ~FunctionList();

        void Import();

        void Parse();

        void Read();

        void ValidateConfig() const;

        void Verify() const;

        ByName mFunctions_ByName;

        // ===== Configurable attributes ====================================
        KMS::DI::String_Expand mExported;
        KMS::DI::Array         mSources;
        KMS::DI::String_Expand mToImport;

    private:

        FunctionList(const FunctionList&);

        const FunctionList& operator = (const FunctionList&);

        Function* FindByName(const char* aName);

        KMS::Text::File_ASCII mFile;

    };

}
