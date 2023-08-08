
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-A/Console.h

#pragma once

class Console
{

public:

    static void Change(const char* aMsg, const char* aElement, const char* aName);
    static void Change(const char* aMsg, const char* aName);
    static void Change(const char* aMsg, const wchar_t* aName);
    static void Change(const char* aMsg, const char* aName, const char* aFrom, const char* aTo);
    static void Stats (uint64_t aValue, const char* aUnit);

    static std::ostream& Error_Begin();
    static void          Error_End  ();

    static std::ostream& Info_Begin();
    static std::ostream& Info_Begin(unsigned int aLineNo);
    static void          Info_End  ();

    static std::ostream& Instruction_Begin();
    static void          Instruction_End  ();

    static void Progress_Begin(const char* aModule, const char* aMsg);
    static void Progress_Begin(const char* aModule, const char* aMsg, const char* aFileName);
    static void Progress_End  (const char* aMsg);

    static std::ostream& Warning_Begin();
    static std::ostream& Warning_Begin(unsigned int aLineNo);
    static std::ostream& Warning_Begin(unsigned int aLineNoA, unsigned int aLineNoB);
    static void          Warning_End  ();

    static void Warning_IgnoredLine(unsigned int aLineNo, const char* aLine);

};
