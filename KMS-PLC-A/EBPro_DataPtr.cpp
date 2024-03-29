
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/EBPro_DataPtr.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "EBPro/DataPtr.h"

using namespace KMS;

KMS_RESULT_STATIC(RESULT_CORRUPTED_MBL_FILE);

namespace EBPro
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    void DataPtr::Write(FILE* aOut, const std::string& aIn)
    {
        assert(nullptr != aOut);

        Write(aOut, static_cast<uint32_t>(aIn.size()));

        fprintf(aOut, "%s ", aIn.c_str());
    }

    void DataPtr::Write(FILE* aOut, uint32_t aIn)
    {
        assert(nullptr != aOut);
        
        fprintf(aOut, "%u ", aIn);
    }

    DataPtr::DataPtr(const char* aData, unsigned int aSize_byte) : mData(aData), mOffset_byte(0), mSize_byte(aSize_byte - 2)
    {
        assert(nullptr != aData);
        assert(2 <= aSize_byte);
    }

    bool DataPtr::IsAtEnd() const { return mSize_byte <= mOffset_byte; }

    void DataPtr::Read(std::string* aOut)
    {
        assert(nullptr != aOut);

        assert(nullptr != mData);

        uint32_t lSize_byte;

        Read(&lSize_byte);

        const char* lStart = mData + mOffset_byte;

        for (unsigned int i = 0; i <= lSize_byte; i++)
        {
            if ('\n' == lStart[i])
            {
                lSize_byte++;
            }
        }

        aOut->assign(lStart, lSize_byte);

        mOffset_byte += lSize_byte;
    }

    void DataPtr::Read(uint32_t* aOut)
    {
        assert(nullptr != aOut);

        assert(nullptr != mData);

        const char* lStart = mData + mOffset_byte;

        char* lEnd;

        *aOut = strtol(lStart, &lEnd, 10);

        mOffset_byte += static_cast<unsigned int>(lEnd - lStart + 1);
    }

    void DataPtr::Verify(uint32_t aExpected)
    {
        uint32_t lUInt32;

        Read(&lUInt32);

        KMS_EXCEPTION_ASSERT(aExpected == lUInt32, RESULT_CORRUPTED_MBL_FILE, "Corrupted mlb file", lUInt32);
    }

}
