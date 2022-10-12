
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/Convert.cpp

#include "Component.h"

// ===== Local ===============================================================
#include "Convert.h"

// Functions
// //////////////////////////////////////////////////////////////////////////

void ToExportableString(const wchar_t* aIn, wchar_t* aOut)
{
    assert(NULL != aIn);
    assert(NULL != aOut);

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
    assert(NULL != aIn);
    assert(NULL != aOut);

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
