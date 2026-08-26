
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/Convert.cpp

#include "Component.h"

// ===== Windows ============================================================
#include <Windows.h>

// ===== Local ===============================================================
#include "../Common/Convert.h"

using namespace KMS;

// Functions
// //////////////////////////////////////////////////////////////////////////

void ToASCII(const std::wstring& aIn, char* aOut, unsigned int aOutSize_byte)
{
    assert(nullptr != aOut);
    assert(0 < aOutSize_byte);

    auto lRet = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS | WC_NO_BEST_FIT_CHARS, aIn.c_str(), -1, aOut, aOutSize_byte, nullptr, nullptr);
    KMS_EXCEPTION_ASSERT(0 < lRet, RESULT_INVALID_FORMAT, "Invalid unicode string (NOT TESTED)", "");
}

void ToASCII(const std::wstring& aIn, std::string* aOut)
{
    assert(nullptr != aOut);

    char lOut[LINE_LENGTH];

    ToASCII(aIn, lOut, sizeof(lOut));

    *aOut = lOut;
}

void ToUnicode(const std::string& aIn, wchar_t* aOut, unsigned int aOutSize_byte)
{
    assert(nullptr != aOut);
    assert(0 < aOutSize_byte);

    auto lRet = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, aIn.c_str(), -1, aOut, aOutSize_byte / sizeof(wchar_t));
    KMS_EXCEPTION_ASSERT(0 < lRet, RESULT_INVALID_FORMAT, "Invalid ASCII string (NOT TESTED)", "");
}

void ToUnicode(const std::string& aIn, std::wstring* aOut)
{
    assert(nullptr != aOut);

    wchar_t lOut[LINE_LENGTH];

    ToUnicode(aIn, lOut, sizeof(lOut));

    *aOut = lOut;
}

void ToExportableString(const wchar_t* aIn, wchar_t* aOut)
{
    assert(nullptr != aIn);
    assert(nullptr != aOut);

    size_t lLength = wcslen(aIn);
    size_t lOut = 0;

    for (unsigned int i = 0; i < lLength; i++)
    {
        switch (aIn[i])
        {
        case '\0':
            assert(false);
            break;

        case '\\':
            aOut[lOut] = '\\'; lOut++;
            aOut[lOut] = '\\'; lOut++;
            break;

        case '\n':
            aOut[lOut] = '\\'; lOut++;
            aOut[lOut] = 'n'; lOut++;
            break;

        case '\r':
            aOut[lOut] = '\\'; lOut++;
            aOut[lOut] = 'r'; lOut++;
            break;

        default:
            aOut[lOut] = aIn[i]; lOut++;
        }
    }

    aOut[lOut] = 0;
}

void ToImportableString(const wchar_t* aIn, wchar_t* aOut)
{
    assert(nullptr != aIn);
    assert(nullptr != aOut);

    size_t lLength = wcslen(aIn);
    size_t lOut = 0;

    for (unsigned int i = 0; i < lLength; i++)
    {
        switch (aIn[i])
        {
        case '\0':
            assert(false);
            break;

        case '\\':
            switch (aIn[i + 1])
            {
            case '\\':
                i++;
                aOut[lOut] = aIn[i]; lOut++;
                break;

            case 'n':
                i++;
                aOut[lOut] = '\n'; lOut++;
                break;

            case 'r':
                i++;
                aOut[lOut] = '\r'; lOut++;
                break;

            default:
                aOut[lOut] = aIn[i]; lOut++;
            }
            break;

        default:
            aOut[lOut] = aIn[i]; lOut++;
        }
    }

    aOut[lOut] = 0;
}
