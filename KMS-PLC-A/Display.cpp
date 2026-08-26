
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
#include "../Common/Convert.h"
#include "../Common/Display.h"

using namespace KMS;

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
