
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/EBPro/FunctionList.h

#pragma once

// ===== Import =============================================================
#include <KMS/File/Binary.h>

// ===== Local ==============================================================
#include "List.h"

namespace EBPro
{

    class DataPtr;
    class Function;

    class FunctionList : public List
    {

    public:

        typedef std::map<std::string, Function*> ByName;

        FunctionList(Software* aSoftware);

        void Parse();

        // ===== List =======================================================
        virtual ~FunctionList();

        ByName mFunctions_ByName;

    protected:

        // ===== List =======================================================
        virtual bool ImportSource(const char* aFileName);
        virtual void ReadExported();
        virtual void SaveToImport();

    private:

        NO_COPY(FunctionList);

        void Add(Function* aIn);

        Function* FindByName(const char* aName);

        void Header_Parse(DataPtr* aPtr);
        void Header_Write(FILE   * aOut);

        void Replace(Function* aIn);

        KMS::File::Binary* mFile;
        const char       * mFile_Data;

        std::string mParsed_Name;
        std::string mParsed_Type;
        uint32_t    mParsed_Values[2];

    };

}
