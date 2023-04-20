
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/EBPro/Function.h

#pragma once

// ===== C++ ================================================================
#include <list>
#include <string>

// ===== Import/Includes ====================================================
#include <KMS/Text/File_ASCII.h>

// ===== Local ==============================================================
#include "EBPro/DataType.h"

namespace EBPro
{

    class DataPtr;

    class Function
    {

    public:

        Function();

        bool operator != (const Function& aB) const;

        const char* GetName() const;

        void Read(DataPtr* aPtr);

        unsigned int Read(const KMS::Text::File_ASCII& aFile, unsigned int aLineNo);

        void Write(FILE* aOut) const;

    private:

        void ParseDefinition(const char* aLine);

        std::list<DataType> mArguments;
        std::string         mCode;
        std::string         mComment0;
        std::string         mComment1;
        uint32_t            mFunctionCode;
        std::string         mName;
        DataType            mReturnType;

    };

}
