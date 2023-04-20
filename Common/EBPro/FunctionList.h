
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
#include <KMS/File/Binary.h>

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

        ByName mFunctions_ByName;

        // ===== Configurable attributes ====================================
        KMS::DI::String_Expand mExported;
        KMS::DI::Array         mSources;
        KMS::DI::String_Expand mToImport;

    private:

        NO_COPY(FunctionList);

        void Add(Function* aIn);

        Function* FindByName(const char* aName);

        void Header_Parse(DataPtr* aPtr);
        void Header_Write(FILE   * aOut);

        bool Import(const char* aFileName);

        void Replace(Function* aIn);

        KMS::File::Binary* mFile;
        const char       * mFile_Data;

        std::string mParsed_Name;
        std::string mParsed_Type;

    };

}
