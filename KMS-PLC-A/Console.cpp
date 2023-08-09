
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-A/Console.cpp

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KMS/Console/Color.h>

// ===== Local ==============================================================
#include "../Common/Globals.h"

// Public
// //////////////////////////////////////////////////////////////////////////

void ::Console::Change(const char* aMsg, const char* aElement, const char* aName)
{
    assert(nullptr != aMsg);
    assert(nullptr != aElement);
    assert(nullptr != aName);

    gConsole.OutputStream() << KMS::Console::Color::BLUE << aMsg << " " << aElement << " - " << aName << KMS::Console::Color::WHITE << std::endl;
}

void ::Console::Change(const char* aMsg, const char* aName)
{
    assert(nullptr != aMsg);
    assert(nullptr != aName);

    gConsole.OutputStream() << KMS::Console::Color::BLUE << aMsg << " - " << aName << KMS::Console::Color::WHITE << std::endl;
}

void ::Console::Change(const char* aMsg, const wchar_t* aName)
{
    assert(nullptr != aMsg);
    assert(nullptr != aName);

    gConsole.OutputStream() << KMS::Console::Color::BLUE << aMsg << " - " << aName << KMS::Console::Color::WHITE << std::endl;
}

void ::Console::Change(const char* aMsg, const char* aName, const char* aFrom, const char* aTo)
{
    assert(nullptr != aMsg);
    assert(nullptr != aName);

    gConsole.OutputStream() << KMS::Console::Color::BLUE;
    gConsole.OutputStream() << aMsg << " - " << aName << " (\"" << aFrom << "\" -> \"" << aTo << "\")";
    gConsole.OutputStream() << KMS::Console::Color::WHITE << std::endl;
}

void ::Console::Stats(uint64_t aValue, const char* aUnit)
{
    assert(nullptr != aUnit);

    gConsole.OutputStream() << "    " << aValue << " " << aUnit << std::endl;
}

std::ostream& ::Console::Error_Begin()
{
    gConsole.OutputStream() << KMS::Console::Color::RED << "    ERROR  ";

    return gConsole.OutputStream();
}

void ::Console::Error_End() { gConsole.OutputStream() << KMS::Console::Color::WHITE << std::endl; }

std::ostream& ::Console::Info_Begin()
{
    gConsole.OutputStream() << "    INFO  ";

    return gConsole.OutputStream();
}

std::ostream& ::Console::Info_Begin(unsigned int aLineNo)
{
    gConsole.OutputStream() << "    INFO  Line " << aLineNo << "  ";

    return gConsole.OutputStream();
}

void ::Console::Info_End() { gConsole.OutputStream() << std::endl; }

std::ostream& ::Console::Instruction_Begin()
{
    gConsole.OutputStream() << KMS::Console::Color::BLUE << "INSTRUCTION\n";

    return gConsole.OutputStream();
}

void ::Console::Instruction_End() { gConsole.OutputStream() << KMS::Console::Color::WHITE << std::endl; }

void ::Console::Progress_Begin(const char* aModule, const char* aMsg)
{
    assert(nullptr != aModule);
    assert(nullptr != aMsg);

    gConsole.OutputStream() << aModule << " - " << aMsg << " ..." << std::endl;
}

void ::Console::Progress_Begin(const char* aModule, const char* aMsg, const char* aFileName)
{
    assert(nullptr != aModule);
    assert(nullptr != aMsg);
    assert(nullptr != aFileName);

    gConsole.OutputStream() << aModule << " - " << aMsg << " " << aFileName << " ..." << std::endl;
}

void ::Console::Progress_End(const char* aMsg)
{
    assert(nullptr != aMsg);

    gConsole.OutputStream() << aMsg << std::endl;
}

std::ostream& ::Console::Warning_Begin()
{
    gConsole.OutputStream() << KMS::Console::Color::YELLOW << "    WARNING  ";

    return gConsole.OutputStream();
}

std::ostream& ::Console::Warning_Begin(unsigned int aLineNo)
{
    gConsole.OutputStream() << KMS::Console::Color::YELLOW << "    WARNING  Line " << aLineNo << "  ";

    return gConsole.OutputStream();
}

std::ostream& ::Console::Warning_Begin(unsigned int aLineNoA, unsigned int aLineNoB)
{
    gConsole.OutputStream() << KMS::Console::Color::YELLOW << "    WARNING  Line " << aLineNoA << " and " << aLineNoB << "  ";

    return gConsole.OutputStream();
}

void ::Console::Warning_End() { gConsole.OutputStream() << KMS::Console::Color::WHITE << std::endl; }

void ::Console::Warning_IgnoredLine(unsigned int aLineNo, const char* aLine)
{
    Console::Warning_Begin(aLineNo)
        << "Ignored - " << aLine;
    Console::Warning_End();
}
