
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-A/Console.cpp

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KMS/Console/Color.h>

// ===== Local ==============================================================
#include "Console.h"

// Public
// //////////////////////////////////////////////////////////////////////////

void ::Console::Change(const char* aMsg, const char* aName)
{
    assert(NULL != aMsg);
    assert(NULL != aName);

    std::cout << KMS::Console::Color::BLUE << aMsg << " - " << aName << KMS::Console::Color::WHITE << std::endl;
}

void ::Console::Change(const char* aMsg, const char* aElement, const char* aName)
{
    assert(NULL != aMsg);
    assert(NULL != aElement);
    assert(NULL != aName);

    std::cout << KMS::Console::Color::BLUE << aMsg << " " << aElement << " - " << aName << KMS::Console::Color::WHITE << std::endl;
}

void ::Console::Change(const char* aMsg, const wchar_t* aName)
{
    assert(NULL != aMsg);
    assert(NULL != aName);

    std::cout << KMS::Console::Color::BLUE << aMsg << " - " << aName << KMS::Console::Color::WHITE << std::endl;
}

void ::Console::Stats(uint64_t aValue, const char* aUnit)
{
    assert(NULL != aUnit);

    std::cout << "    " << aValue << " " << aUnit << std::endl;
}

void ::Console::Error_Begin() { std::cout << KMS::Console::Color::RED << "    ERROR  "; }
void ::Console::Error_End  () { std::cout << KMS::Console::Color::WHITE << std::endl; }

void ::Console::Info_Begin() { std::cout << "    INFO  "; }
void ::Console::Info_End  () { std::cout << std::endl; }

void ::Console::Instruction_Begin() { std::cout << KMS::Console::Color::BLUE << "INSTRUCTION\n"; }
void ::Console::Instruction_End  () { std::cout << KMS::Console::Color::WHITE << std::endl; }

void ::Console::Progress_Begin(const char* aMsg)
{
    assert(NULL != aMsg);

    std::cout << aMsg << " ..." << std::endl;
}

void ::Console::Progress_Begin(const char* aMsg, const char* aFileName)
{
    assert(NULL != aMsg);
    assert(NULL != aFileName);

    std::cout << aMsg << " " << aFileName << " ..." << std::endl;
}

void ::Console::Progress_End(const char* aMsg)
{
    assert(NULL != aMsg);

    std::cout << aMsg << std::endl;
}

void ::Console::Warning_Begin() { std::cout << KMS::Console::Color::YELLOW << "    WARNING  "; }
void ::Console::Warning_End  () { std::cout << KMS::Console::Color::WHITE << std::endl; }
