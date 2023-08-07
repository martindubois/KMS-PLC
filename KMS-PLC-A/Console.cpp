
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
    assert(NULL != aMsg);
    assert(NULL != aElement);
    assert(NULL != aName);

    gConsole.OutputStream() << KMS::Console::Color::BLUE << aMsg << " " << aElement << " - " << aName << KMS::Console::Color::WHITE << std::endl;
}

void ::Console::Change(const char* aMsg, const char* aName)
{
    assert(NULL != aMsg);
    assert(NULL != aName);

    gConsole.OutputStream() << KMS::Console::Color::BLUE << aMsg << " - " << aName << KMS::Console::Color::WHITE << std::endl;
}

void ::Console::Change(const char* aMsg, const wchar_t* aName)
{
    assert(NULL != aMsg);
    assert(NULL != aName);

    gConsole.OutputStream() << KMS::Console::Color::BLUE << aMsg << " - " << aName << KMS::Console::Color::WHITE << std::endl;
}

void ::Console::Change(const char* aMsg, const char* aName, const char* aFrom, const char* aTo)
{
    assert(NULL != aMsg);
    assert(NULL != aName);

    gConsole.OutputStream() << KMS::Console::Color::BLUE;
    gConsole.OutputStream() << aMsg << " - " << aName << " (\"" << aFrom << "\" -> \"" << aTo << "\")";
    gConsole.OutputStream() << KMS::Console::Color::WHITE << std::endl;
}

void ::Console::Stats(uint64_t aValue, const char* aUnit)
{
    assert(NULL != aUnit);

    gConsole.OutputStream() << "    " << aValue << " " << aUnit << std::endl;
}

void ::Console::Error_Begin() { gConsole.OutputStream() << KMS::Console::Color::RED << "    ERROR  "; }
void ::Console::Error_End  () { gConsole.OutputStream() << KMS::Console::Color::WHITE << std::endl; }

void ::Console::Info_Begin() { gConsole.OutputStream() << "    INFO  "; }

void ::Console::Info_Begin(unsigned int aLineNo)
{
    gConsole.OutputStream() << "    INFO  Line " << aLineNo << "  ";
}

void ::Console::Info_End() { gConsole.OutputStream() << std::endl; }

void ::Console::Instruction_Begin() { gConsole.OutputStream() << KMS::Console::Color::BLUE << "INSTRUCTION\n"; }
void ::Console::Instruction_End  () { gConsole.OutputStream() << KMS::Console::Color::WHITE << std::endl; }

void ::Console::Progress_Begin(const char* aModule, const char* aMsg)
{
    assert(NULL != aModule);
    assert(NULL != aMsg);

    gConsole.OutputStream() << aModule << " - " << aMsg << " ..." << std::endl;
}

void ::Console::Progress_Begin(const char* aModule, const char* aMsg, const char* aFileName)
{
    assert(NULL != aModule);
    assert(NULL != aMsg);
    assert(NULL != aFileName);

    gConsole.OutputStream() << aModule << " - " << aMsg << " " << aFileName << " ..." << std::endl;
}

void ::Console::Progress_End(const char* aMsg)
{
    assert(NULL != aMsg);

    gConsole.OutputStream() << aMsg << std::endl;
}

void ::Console::Warning_Begin() { gConsole.OutputStream() << KMS::Console::Color::YELLOW << "    WARNING  "; }

void ::Console::Warning_Begin(unsigned int aLineNo)
{
    gConsole.OutputStream() << KMS::Console::Color::YELLOW << "    WARNING  Line " << aLineNo << "  ";
}

void ::Console::Warning_Begin(unsigned int aLineNoA, unsigned int aLineNoB)
{
    gConsole.OutputStream() << KMS::Console::Color::YELLOW << "    WARNING  Line " << aLineNoA << " and " << aLineNoB << "  ";
}

void ::Console::Warning_End() { gConsole.OutputStream() << KMS::Console::Color::WHITE << std::endl; }

void ::Console::Warning_IgnoredLine(unsigned int aLineNo, const char* aLine)
{
    Console::Warning_Begin(aLineNo);
    gConsole.OutputStream() << "Ignored - " << aLine;
    Console::Warning_End();
}
