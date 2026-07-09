
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      EBPro_Build_Label/Builder_Label.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "../Common/HMI/Label.h"

#include "Builder_Label.h"

using namespace KMS;

// Configurations
// //////////////////////////////////////////////////////////////////////////

#define TO_IMPORT_LBL "EBPro_Labels_ToImport.lbl"

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

static void Write_Data(FILE* aFile, const void* aIn, unsigned int aInSize_byte);

static void Write_String(FILE* aFile, const std::wstring& aIn);

// Public
// //////////////////////////////////////////////////////////////////////////

Builder_Label::Builder_Label() : mIndex(0), mOut(nullptr) {}

// ===== Builder ============================================================

Builder_Label::~Builder_Label()
{
    std::cout << "\n";
    std::cout << std::endl;

    if (nullptr != mOut)
    {
        auto lRet = fclose(mOut);
        assert(0 == lRet);
        (void)lRet;
    }
}

// Protected
// //////////////////////////////////////////////////////////////////////////

// ===== HMI::Builder_Label =================================================

void Builder_Label::ProcessLabel(const HMI::Label& aLabel)
{
    assert(nullptr != mOut);

    std::cout << " Label " << mIndex << "    \r";

    if (0 < mIndex)
    {
        static const uint8_t MARK[] = { 0x001, 0x80 };

        auto lSize_byte = fwrite(MARK, 1, sizeof(MARK), mOut);
        KMS_EXCEPTION_ASSERT(sizeof(MARK) == lSize_byte, RESULT_WRITE_FAILED, "Cannot write to output file", lSize_byte);
    }

    std::wstring lName;
    auto lStateCount = static_cast<uint16_t>(aLabel.GetStateCount());

    aLabel.GetName(&lName);

    Write_String(mOut, lName);
    Write_Data(mOut, &lStateCount, sizeof(lStateCount));

    for (unsigned int i = 0; i < EBPro::Config::LANGUAGE_QTY; i++)
    {
        for (unsigned int j = 0; j < lStateCount; j++)
        {
            std::wstring lString;

            aLabel.GetString(j, mConfig.GetLanguageName(i), &lString);

            Write_String(mOut, lString);
        }
    }

    static const std::wstring EMPTY(L"");

    unsigned int lEmptyString = lStateCount * 16;
    for (unsigned int i = 0; i < lEmptyString; i++)
    {
        Write_String(mOut, EMPTY);
    }

    mIndex++;
}

void Builder_Label::ProcessLabelCount(unsigned int aCount)
{
    static const uint8_t HEADER[] =
    {
        //                      C   L   a   b    e    l   O   b    j    e   c    t
        255, 255, 1, 0, 12, 0, 67, 76, 97, 98, 101, 108, 79, 98, 106, 101, 99, 116
    };

    assert(0xffff >= aCount);

    std::cout << aCount << " labels\n" << std::endl;

    auto lRet = fopen_s(&mOut, TO_IMPORT_LBL, "wb");
    KMS_EXCEPTION_ASSERT(0 == lRet, RESULT_OPEN_FAILED, "Cannot open ouput file", lRet);

    assert(nullptr != mOut);

    auto lLabelCount = static_cast<uint16_t>(aCount);

    auto lSize_byte = fwrite(&lLabelCount, 1, sizeof(lLabelCount), mOut);
    KMS_EXCEPTION_ASSERT(sizeof(lLabelCount) == lSize_byte, RESULT_WRITE_FAILED, "Cannot write to output file", lSize_byte);

    lSize_byte = fwrite(HEADER, 1, sizeof(HEADER), mOut);
    KMS_EXCEPTION_ASSERT(sizeof(HEADER) == lSize_byte, RESULT_WRITE_FAILED, "Cannot write to output file", lSize_byte);
}

// Static functions
// //////////////////////////////////////////////////////////////////////////

void Write_Data(FILE* aFile, const void* aIn, unsigned int aInSize_byte)
{
    assert(nullptr != aFile);
    assert(nullptr != aIn);
    assert(0 < aInSize_byte);

    auto lSize_byte = fwrite(aIn, 1, aInSize_byte, aFile);
    KMS_EXCEPTION_ASSERT(aInSize_byte == lSize_byte, RESULT_WRITE_FAILED, "Cannot write to output file", lSize_byte);
}

void Write_String(FILE* aFile, const std::wstring& aIn)
{
    assert(0xffff >= aIn.size());

    auto lLength = static_cast<uint16_t>(aIn.size());

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
