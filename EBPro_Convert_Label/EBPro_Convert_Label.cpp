
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// Product   KMS-PLC
// File      EBPro_Convert_Label/EBPro_Convert_Label.cpp

#include <KMS/Base.h>

// ===== C++ ================================================================
#include <fstream>

// ===== Import/Includes ====================================================
#include <KMS/Banner.h>
#include <KMS/Console/Color.h>
#include <KMS/Exception.h>

// ===== Local ==============================================================
#include "../Common/EBPro/Config.h"
#include "../Common/HMI/HMI.h"
#include "../Common/HMI/Label.h"
#include "../Common/Version.h"

using namespace KMS;

// Configurations
// //////////////////////////////////////////////////////////////////////////

static const unsigned int LANGUAGE_QTY = 8;

static const char* EXPORTED_LABEL_LBL = "EBPro_Exported_Label.lbl";

// Data type
// //////////////////////////////////////////////////////////////////////////

enum class ReadResult
{
    OK,
    EMPTY_STRING,
    END_OF_LABEL
};

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

static void Convert_Label(FILE* aIn, std::ostream& aOut, const EBPro::Config& aConfig);

static void Convert_Labels(const char* aInName);

static void DisplayUsage();

static void Read_Data(FILE* aFile, void* aOut, unsigned int aOutSize_byte);

static ReadResult Read_String(FILE* aFile, std::wstring* aOut);

// Entry point
// //////////////////////////////////////////////////////////////////////////

int main(int aCount, const char** aVector)
{
    KMS_BANNER("KMS-PLC", "EBPro_Convert_Label");

    assert(nullptr != aVector);

    std::string lInputFileName(EXPORTED_LABEL_LBL);

    int lResult = 0;

    try
    {
        switch (aCount)
        {
        case 2:
            assert(nullptr != aVector[1]);

            lInputFileName = aVector[1]; break;
            // no break

        case 1:
            Convert_Labels(lInputFileName.c_str());
            break;

        default:
            std::cout << Console::Color::RED;
            std::cout << "Invalid commmand line\n";
            std::cout << Console::Color::WHITE;
            std::cout << std::endl;
            DisplayUsage();
            lResult = __LINE__;
        }
    }
    KMS_CATCH_RESULT(lResult);

    return lResult;
}

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

void Convert_Label(FILE* aIn, std::ostream& aOut, const EBPro::Config& aConfig)
{
    assert(nullptr != aIn);

    auto lLanguageFirst = aConfig.GetLanguageName(0);

    std::wstring lName;

    auto lRR = Read_String(aIn, &lName);
    KMS_EXCEPTION_ASSERT(ReadResult::OK == lRR, RESULT_FILE_FORMAT_ERROR, "Corrupted input file", "");

    uint16_t lStateCount;

    Read_Data(aIn, &lStateCount, sizeof(lStateCount));

    HMI::Label lLabel;

    lLabel.SetName(lName);

    lLabel.SetStateCount(lStateCount);

    for (unsigned int i = 0; i < EBPro::Config::LANGUAGE_QTY; i++)
    {
        for (unsigned int j = 0; j < lStateCount; j++)
        {
            std::wstring lStr;

            switch (Read_String(aIn, &lStr))
            {
            case ReadResult::OK: break;

            case ReadResult::EMPTY_STRING:
                if (0 < i)
                {
                    lLabel.GetString(j, lLanguageFirst, &lStr);
                }
                break;

            default: KMS_EXCEPTION(RESULT_FILE_FORMAT_ERROR, "Corrupted input file", i);
            }

            lLabel.SetString(j, aConfig.GetLanguageName(i), lStr);
        }
    }

    auto lContinue = true;
    unsigned int lEmptyString = 0;

    while (lContinue)
    {
        std::wstring lDump;

        switch (Read_String(aIn, &lDump))
        {
        case ReadResult::OK: KMS_EXCEPTION(RESULT_FILE_FORMAT_ERROR, "Corrupted input file", "");

        case ReadResult::EMPTY_STRING: lEmptyString++; break;

        case ReadResult::END_OF_LABEL:
            KMS_EXCEPTION_ASSERT(lStateCount * 16 == lEmptyString, RESULT_FILE_FORMAT_ERROR, "Corrupted input file", lEmptyString);
            lContinue = false;
            break;

        default: assert(false);
        }
    }

    lLabel.Write(aOut);
}

void Convert_Labels(const char* aInName)
{
    assert(nullptr != aInName);

    FILE* lIn;

    auto lRet = fopen_s(&lIn, aInName, "rb");
    KMS_EXCEPTION_ASSERT(0 == lRet, RESULT_OPEN_FAILED, "Cannot open input file", aInName);

    uint16_t lLabelCount;

    auto lSize_byte = fread_s(&lLabelCount SizeInfo(lLabelCount) + 1, 1, sizeof(lLabelCount), lIn);
    KMS_EXCEPTION_ASSERT(sizeof(lLabelCount) == lSize_byte, RESULT_READ_FAILED, "Cannot read input file", lSize_byte);

    uint8_t lHeader[18];

    lSize_byte = fread_s(&lHeader SizeInfo(lHeader) + 1, 1, sizeof(lHeader), lIn);
    KMS_EXCEPTION_ASSERT(sizeof(lHeader) == lSize_byte, RESULT_READ_FAILED, "Cannot read input file", lSize_byte);

    EBPro::Config lConfig;

    std::ofstream lOut(HMI::LABELS_TXT);

    lOut << "\n";
    lOut << "# This file has been generated by KMS-PLC - EBPro_Convert_Label.exe\n";
    lOut << "# from the data read from " << aInName << "\n";
    lOut << "\n";

    unsigned int lIndex = 0;

    while (!feof(lIn))
    {
        std::cout << " Label " << lIndex << "    \r"; lIndex++;

        Convert_Label(lIn, lOut, lConfig);
    }

    std::cout << std::endl;

    lRet = fclose(lIn);
    assert(0 == lRet);
}

void DisplayUsage()
{
    std::cout << "Usage: EBPro_Convert_Label.exe [" << EXPORTED_LABEL_LBL << "]\n";
    std::cout << std::endl;
}

void Read_Data(FILE* aFile, void* aOut, unsigned int aOutSize_byte)
{
    assert(nullptr != aFile);
    assert(nullptr != aOut);
    assert(0 < aOutSize_byte);

    auto lSize_byte = fread_s(aOut SizeInfoV(aOutSize_byte + 1), 1, aOutSize_byte, aFile);
    KMS_EXCEPTION_ASSERT(aOutSize_byte == lSize_byte, RESULT_FILE_FORMAT_ERROR, "Corrupted input file", lSize_byte);
}

ReadResult Read_String(FILE* aFile, std::wstring* aOut)
{
    assert(nullptr != aFile);

    uint8_t lHeader[4];

    auto lSize_byte = fread_s(lHeader SizeInfoV(3), 1, 2, aFile);
    if (0 == lSize_byte)
    {
        return ReadResult::END_OF_LABEL;
    }

    KMS_EXCEPTION_ASSERT(2 == lSize_byte, RESULT_READ_FAILED, "Cannot read input file", lSize_byte);

    if ((0x01 == lHeader[0]) && (0x80 == lHeader[1]))
    {
        return ReadResult::END_OF_LABEL;
    }

    KMS_EXCEPTION_ASSERT(0xff == lHeader[0], RESULT_FILE_FORMAT_ERROR, "Corrupted input file", lHeader[0]);
    KMS_EXCEPTION_ASSERT(0xfe == lHeader[1], RESULT_FILE_FORMAT_ERROR, "Corrupted input file", lHeader[1]);

    Read_Data(aFile, lHeader + 2, 2);

    KMS_EXCEPTION_ASSERT(0xff == lHeader[2], RESULT_FILE_FORMAT_ERROR, "Corrupted input file", lHeader[2]);

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
