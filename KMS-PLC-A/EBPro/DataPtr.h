
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/EBPro/DataPtr.h

#pragma once

// ===== C++ ================================================================
#include <string>

namespace EBPro
{

    class DataPtr
    {

    public:

        static void Write(FILE* aOut, const std::string& aIn);
        static void Write(FILE* aOut, uint32_t           aIn);

        DataPtr(const char* aData, unsigned int aSize_byte);

        bool IsAtEnd() const;

        void Read(std::string* aOut);
        void Read(uint32_t   * aOut);

        void Verify(uint32_t aExpected);

    private:

        NO_COPY(DataPtr);

        const char * mData;
        unsigned int mOffset_byte;
        unsigned int mSize_byte;

    };

}
