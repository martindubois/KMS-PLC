
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-A/Console.cpp

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KMS/Console/Color.h>

// Public
// //////////////////////////////////////////////////////////////////////////

void ::Console::Change(const char* aMsg, const char* aElement, const char* aName)
{
    assert(nullptr != aMsg);
    assert(nullptr != aElement);
    assert(nullptr != aName);

    std::cout << KMS::Console::Color::BLUE << aMsg << " " << aElement << " - " << aName << KMS::Console::Color::WHITE << std::endl;
}

void ::Console::Change(const char* aMsg, const char* aName)
{
    assert(nullptr != aMsg);
    assert(nullptr != aName);

    std::cout << KMS::Console::Color::BLUE << aMsg << " - " << aName << KMS::Console::Color::WHITE << std::endl;
}

void ::Console::Change(const char* aMsg, const wchar_t* aName)
{
    assert(nullptr != aMsg);
    assert(nullptr != aName);

    char lName[NAME_LENGTH];

    sprintf_s(lName, "%S", aName);

    std::cout << KMS::Console::Color::BLUE << aMsg << " - " << lName << KMS::Console::Color::WHITE << std::endl;
}

void ::Console::Change(const char* aMsg, const char* aName, const char* aFrom, const char* aTo)
{
    assert(nullptr != aMsg);
    assert(nullptr != aName);

    std::cout << KMS::Console::Color::BLUE;
    std::cout << aMsg << " - " << aName << " (\"" << aFrom << "\" -> \"" << aTo << "\")";
    std::cout << KMS::Console::Color::WHITE << std::endl;
}

void ::Console::Stats(uint64_t aValue, const char* aUnit)
{
    assert(nullptr != aUnit);

    std::cout << "    " << aValue << " " << aUnit << std::endl;
}

std::ostream& ::Console::Error_Begin()
{
    std::cout << KMS::Console::Color::RED << "    ERROR  ";

    return std::cout;
}

void ::Console::Error_End() { std::cout << KMS::Console::Color::WHITE << std::endl; }

std::ostream& ::Console::Info_Begin()
{
    std::cout << "    INFO  ";

    return std::cout;
}

std::ostream& ::Console::Info_Begin(unsigned int aLineNo)
{
    std::cout << "    INFO  Line " << aLineNo << "  ";

    return std::cout;
}

void ::Console::Info_End() { std::cout << std::endl; }

void ::Console::Progress_Begin(const char* aModule, const char* aMsg)
{
    assert(nullptr != aModule);
    assert(nullptr != aMsg);

    std::cout << aModule << " - " << aMsg << " ..." << std::endl;
}

void ::Console::Progress_Begin(const char* aModule, const char* aMsg, const char* aFileName)
{
    assert(nullptr != aModule);
    assert(nullptr != aMsg);
    assert(nullptr != aFileName);

    std::cout << aModule << " - " << aMsg << " " << aFileName << " ..." << std::endl;
}

void ::Console::Progress_End(const char* aMsg)
{
    assert(nullptr != aMsg);

    std::cout << aMsg << std::endl;
}

std::ostream& ::Console::Warning_Begin()
{
    std::cout << KMS::Console::Color::YELLOW << "    WARNING  ";

    return std::cout;
}

std::ostream& ::Console::Warning_Begin(unsigned int aLineNo)
{
    std::cout << KMS::Console::Color::YELLOW << "    WARNING  Line " << aLineNo << "  ";

    return std::cout;
}

std::ostream& ::Console::Warning_Begin(unsigned int aLineNoA, unsigned int aLineNoB)
{
    std::cout << KMS::Console::Color::YELLOW << "    WARNING  Line " << aLineNoA << " and " << aLineNoB << "  ";

    return std::cout;
}

void ::Console::Warning_End() { std::cout << KMS::Console::Color::WHITE << std::endl; }

void ::Console::Warning_IgnoredLine(unsigned int aLineNo, const char* aLine)
{
    Console::Warning_Begin(aLineNo)
        << "Ignored - " << aLine;
    Console::Warning_End();
}
