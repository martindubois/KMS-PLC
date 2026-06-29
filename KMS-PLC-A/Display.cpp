
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/Display.cpp

#include "Component.h"

// ===== Windows ============================================================
#include "Windows.h"

// ===== Import/Includes ====================================================
#include <KMS/Console/Color.h>

// ===== Local ===============================================================
#include "../Common/Display.h"

using namespace KMS;

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

static void ToASCII(const std::wstring& aIn, char* aOut, unsigned int aOutSize_byte);

// Functions
// //////////////////////////////////////////////////////////////////////////

void Display_Error(const char* aMsg, const char* aInfo)
{
    assert(nullptr != aMsg);

    std::cout << KMS::Console::Color::RED;
    {
        std::cout << aMsg << "\n";
        if (nullptr != aInfo)
        {
            std::cout << "    " << aInfo << "\n";
        }
    }
    std::cout << KMS::Console::Color::WHITE;
    std::cout << std::endl;
}

void Display_Error(const char* aMsg, const wchar_t* aInfo)
{
    assert(nullptr != aMsg);

    std::cout << KMS::Console::Color::RED;
    {
        std::cout << aMsg << "\n";
        if (nullptr != aInfo)
        {
            char lInfo[LINE_LENGTH];

            ToASCII(aInfo, lInfo, sizeof(lInfo));

            std::cout << "    " << lInfo << "\n";
        }
    }
    std::cout << KMS::Console::Color::WHITE;
    std::cout << std::endl;
}

void Display_Warning(const char* aMsg, const char* aInfo)
{
    assert(nullptr != aMsg);

    std::cout << KMS::Console::Color::YELLOW;
    {
        std::cout << aMsg << "\n";
        if (nullptr != aInfo)
        {
            std::cout << "    " << aInfo << "\n";
        }
    }
    std::cout << KMS::Console::Color::WHITE;
    std::cout << std::endl;
}

void Display_Warning(const char* aMsg, const wchar_t* aInfo)
{
    assert(nullptr != aMsg);

    std::cout << KMS::Console::Color::YELLOW;
    {
        std::cout << aMsg << "\n";
        if (nullptr != aInfo)
        {
            char lInfo[LINE_LENGTH];

            ToASCII(aInfo, lInfo, sizeof(lInfo));

            std::cout << "    " << lInfo << "\n";
        }
    }
    std::cout << KMS::Console::Color::WHITE;
    std::cout << std::endl;
}

// Static functions
// //////////////////////////////////////////////////////////////////////////

void ToASCII(const std::wstring& aIn, char* aOut, unsigned int aOutSize_byte)
{
    assert(nullptr != aOut);
    assert(0 < aOutSize_byte);

    auto lRet = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS | WC_NO_BEST_FIT_CHARS, aIn.c_str(), -1, aOut, aOutSize_byte, nullptr, nullptr);

    KMS_EXCEPTION_ASSERT(0 < lRet, RESULT_INVALID_FORMAT, "Invalid unicode string (NOT TESTED)", "");
}
