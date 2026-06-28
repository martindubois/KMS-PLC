
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/Parser.cpp

#include "Component.h"

// ===== C++ ================================================================
#include <regex>

// ===== Local ==============================================================
#include "../Common/Parser.h"

using namespace KMS;

// Public
// //////////////////////////////////////////////////////////////////////////

Parser::Parser(const char* aFileName, bool aOptional) : mFile(nullptr)
{
    auto lRet = fopen_s(&mFile, aFileName, "rb");
    KMS_EXCEPTION_ASSERT(0 == lRet || aOptional, RESULT_OPEN_FAILED, "Cannot open input file", aFileName);
}

Parser::~Parser()
{
    if (nullptr != mFile)
    {
        auto lRet = fclose(mFile);
        assert(0 == lRet);
        (void)lRet;
    }
}

bool Parser::GetNextLine(char* aOut, unsigned int aOutSize_byte)
{
    assert(nullptr != aOut);
    assert(0 < aOutSize_byte);

    if (nullptr != mFile)
    {
        char lLine[LINE_LENGTH];

        while (nullptr != fgets(lLine, sizeof(lLine), mFile))
        {
            static const std::regex REGEX_COMMENT("^\\s*\\#.*\r\n");
            static const std::regex REGEX_EMPTY("^\\s*\r\n$");
            static const std::regex REGEX_LINE("^\\s*(.+)\r\n$");

            std::smatch lMatch;

            std::string lLineStr(lLine);

            if (std::regex_match(lLine, REGEX_COMMENT))
            {
            }
            else if (std::regex_match(lLine, REGEX_EMPTY))
            {
            }
            else
            {
                auto lRet = std::regex_match(lLineStr, lMatch, REGEX_LINE);
                assert(lRet);

                strcpy_s(aOut SizeInfoV(aOutSize_byte), lMatch[1].str().c_str());
                return true;
            }
        }
    }

    return false;
}

bool Parser::GetNextLine_Code(char* aOut, unsigned int aOutSize_byte)
{
    assert(nullptr != aOut);
    assert(0 < aOutSize_byte);

    if (nullptr != mFile)
    {
        char lLine[LINE_LENGTH];

        while (nullptr != fgets(lLine, sizeof(lLine), mFile))
        {
            static const std::regex REGEX_LINE("^(.*)\r\n$");

            std::smatch lMatch;

            std::string lLineStr(lLine);

            if (std::regex_match(lLineStr, lMatch, REGEX_LINE))
            {
                strcpy_s(aOut SizeInfoV(aOutSize_byte), lMatch[1].str().c_str());
                return true;
            }
            else
            {
                printf("Ignored line : %s\r\n", lLine);
            }
        }
    }

    return false;
}
