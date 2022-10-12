
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/EBPro_Label.cpp

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KMS/Console/Color.h>

// ===== Local ==============================================================
#include "../Common/EBPro/LabelState.h"

#include "../Common/EBPro/Label.h"

#include "Convert.h"

// data type
// //////////////////////////////////////////////////////////////////////////

enum class ReadResult
{
    OK,
    EMPTY_STRING,
    END_OF_LABEL
};

// Constants
// //////////////////////////////////////////////////////////////////////////

#define LANGUAGE_QTY (8)

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

static void Read_Data(FILE* aFile, void* aOut, unsigned int aOutSize_byte);

static ReadResult Read_String(FILE* aFile, std::wstring* aOut);

static void Write_Data(FILE* aFile, const void* aIn, unsigned int aInSize_byte);

static void Write_String(FILE* aFile, const std::wstring& aIn);

namespace EBPro
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    Label::Label(const wchar_t* aName) : mName(aName) {}

    Label::~Label()
    {
        for (LabelState* lState : mStates)
        {
            assert(NULL != lState);

            delete lState;
        }
    }

    void Label::Export(FILE* aFile, const KMS::DI::Array& aLanguages) const
    {
        assert(NULL != aFile);

        fwprintf(aFile, L"LABEL %s\n", mName.c_str());

        for (const LabelState* lState : mStates)
        {
            lState->Export(aFile, aLanguages);
        }
    }

    void Label::Read(FILE* aFile)
    {
        assert(NULL != aFile);

        ReadResult lRR = Read_String(aFile, &mName);
        KMS_EXCEPTION_ASSERT(ReadResult::OK == lRR, APPLICATION, "Corrupted exported LBL file", "");

        uint16_t lStateCount;

        Read_Data(aFile, &lStateCount, sizeof(lStateCount));

        for (unsigned int i = 0; i < lStateCount; i++)
        {
            LabelState* lState = new LabelState;

            mStates.push_back(lState);
        }

        for (unsigned int i = 0; i < LANGUAGE_QTY; i++)
        {
            for (LabelState* lState : mStates)
            {
                std::wstring lStr;

                lRR = Read_String(aFile, &lStr);
                KMS_EXCEPTION_ASSERT(ReadResult::OK != ReadResult::END_OF_LABEL, APPLICATION, "Corrupted exported LBL file", "");

                lState->mStrings.push_back(lStr);
            }
        }

        unsigned int lEmptyString = 0;

        for (;;)
        {
            std::wstring lDump;

            switch (Read_String(aFile, &lDump))
            {
            case ReadResult::OK: KMS_EXCEPTION(APPLICATION, "Corrupted exported LBL file", "");

            case ReadResult::EMPTY_STRING: lEmptyString++; break;

            case ReadResult::END_OF_LABEL:
                KMS_EXCEPTION_ASSERT(mStates.size() * 16 == lEmptyString, APPLICATION, "Corrupted exported LBL file", lEmptyString);
                return;

            default: assert(false);
            }
        }
    }

    void Label::Verify() const
    {
        if (0 >= mStates.size())
        {
            std::cout << KMS::Console::Color::YELLOW << "WARNING  The label " << mName.c_str() << " has no state" << KMS::Console::Color::WHITE << std::endl;
            return;
        }

        // TODO Verify more
    }

    void Label::Write(FILE* aFile) const
    {
        assert(NULL != aFile);

        assert(0xffff >= mStates.size());

        Write_String(aFile, mName);

        uint16_t lStateCount = static_cast<uint16_t>(mStates.size());

        Write_Data(aFile, &lStateCount, sizeof(lStateCount));

        for (unsigned int i = 0; i < LANGUAGE_QTY; i++)
        {
            for (const LabelState* lState : mStates)
            {
                Write_String(aFile, lState->mStrings[i]);
            }
        }

        std::wstring lEmpty(L"");

        size_t lEmptyString = mStates.size() * 16;
        for (unsigned int i = 0; i < lEmptyString; i++)
        {
            Write_String(aFile, lEmpty);
        }
    }

    // Internal
    // //////////////////////////////////////////////////////////////////////

    LabelState* Label::FindOrCreate(unsigned int aState, bool* aChanged)
    {
        LabelState* lResult;

        if (mStates.size() > aState)
        {
            lResult = mStates[aState];
        }
        else
        {
            while (mStates.size() < aState)
            {
                mStates.push_back(new LabelState);
            }

            lResult = new LabelState;

            mStates.push_back(lResult);
        }

        return lResult;
    }
}

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

void Read_Data(FILE* aFile, void* aOut, unsigned int aOutSize_byte)
{
    assert(NULL != aFile);
    assert(NULL != aOut);
    assert(0 < aOutSize_byte);

    size_t lSize_byte = fread_s(aOut SizeInfoV(aOutSize_byte + 1), 1, aOutSize_byte, aFile);
    KMS_EXCEPTION_ASSERT(aOutSize_byte == lSize_byte, APPLICATION, "Corrupted exported LBL file", lSize_byte);
}

ReadResult Read_String(FILE* aFile, std::wstring* aOut)
{
    assert(NULL != aFile);

    uint8_t lHeader[4];

    size_t lSize_byte = fread_s(lHeader SizeInfoV(3), 1, 2, aFile);
    if (0 == lSize_byte)
    {
        return ReadResult::END_OF_LABEL;
    }

    KMS_EXCEPTION_ASSERT(2 == lSize_byte, FILE_READ, "Cannot read the exported LBL file", lSize_byte);

    if ((0x01 == lHeader[0]) && (0x80 == lHeader[1]))
    {
        return ReadResult::END_OF_LABEL;
    }

    KMS_EXCEPTION_ASSERT(0xff == lHeader[0], APPLICATION, "Corrupted exported LBL file", lHeader[0]);
    KMS_EXCEPTION_ASSERT(0xfe == lHeader[1], APPLICATION, "Corrupted exported LBL file", lHeader[1]);

    Read_Data(aFile, lHeader + 2, 2);

    KMS_EXCEPTION_ASSERT(0xff == lHeader[2], APPLICATION, "Corrupted exported LBL file", lHeader[2]);

    if (0 == lHeader[3])
    {
        return ReadResult::EMPTY_STRING;
    }

    uint16_t lLength;

    if (0xff == lHeader[3])
    {
        Read_Data(aFile, &lLength, sizeof(lLength));
    }
    else
    {
        lLength = lHeader[3];
    }

    unsigned int lToRead_byte = lLength * sizeof(wchar_t);

    wchar_t lBuffer[1024];

    assert(sizeof(lBuffer) > lToRead_byte);

    Read_Data(aFile, lBuffer, lToRead_byte);

    lBuffer[lLength] = 0;

    *aOut = lBuffer;

    return ReadResult::OK;
}

void Write_Data(FILE* aFile, const void* aIn, unsigned int aInSize_byte)
{
    assert(NULL != aFile);
    assert(NULL != aIn);
    assert(0 < aInSize_byte);

    size_t lSize_byte = fwrite(aIn, 1, aInSize_byte, aFile);
    KMS_EXCEPTION_ASSERT(aInSize_byte == lSize_byte, FILE_WRITE, "Cannot write the output LBL file", lSize_byte);

}

void Write_String(FILE* aFile, const std::wstring& aIn)
{
    assert(0xffff >= aIn.size());

    uint16_t lLength = static_cast<uint16_t>(aIn.size());

    uint8_t lHeader[4];

    lHeader[0] = 0xff;
    lHeader[1] = 0xfe;
    lHeader[2] = 0xff;

    if (0xff > lLength)
    {
        lHeader[3] = static_cast<uint8_t>(lLength);
    }
    else
    {
        lHeader[3] = 0xff;
    }

    Write_Data(aFile, lHeader, sizeof(lHeader));

    if (0xff <= aIn.size())
    {
        Write_Data(aFile, &lLength, sizeof(lLength));
    }

    if (0 < lHeader[3])
    {
        unsigned int lToWrite_byte = lLength * 2;

        Write_Data(aFile, aIn.c_str(), lToWrite_byte);
    }
}
