
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/Parser.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "../Common/Parser.h"

using namespace KMS;

// Public
// //////////////////////////////////////////////////////////////////////////

Parser::Parser(const char* aFileName) : mFile(nullptr)
{
    auto lRet = fopen_s(&mFile, aFileName, "rb");
    KMS_EXCEPTION_ASSERT(0 == lRet, RESULT_OPEN_FAILED, "Cannot open input file", aFileName);

    assert(nullptr != mFile);
}

Parser::~Parser()
{
    assert(nullptr != mFile);

    auto lRet = fclose(mFile);
    assert(0 == lRet);
    (void)lRet;
}

bool Parser::GetNextLine(char* aOut, unsigned int aOutSize_byte)
{
    assert(nullptr != aOut);
    assert(0 < aOutSize_byte);

    assert(nullptr != mFile);

    char lLine[LINE_LENGTH];

    if (nullptr != fgets(lLine, sizeof(lLine), mFile))
    {
        unsigned int lStart = 0;

        for (;;)
        {
            switch (lLine[lStart])
            {
            case '\0':
            case '\n':
            case '\r':
            case '#':
                // The line is empty or a comment
                return GetNextLine(aOut, aOutSize_byte);

            case ' ':
            case '\t':
                lStart++;
                break;

            default:
                strcpy_s(aOut SizeInfoV(aOutSize_byte), lLine + lStart);
                return true;
            }
        }
    }

    return false;
}
