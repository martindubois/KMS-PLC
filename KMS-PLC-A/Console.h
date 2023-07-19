
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-A/Console.h

#pragma once

class Console
{

public:

    static void Change(const char* aMsg, const char* aName);
    static void Change(const char* aMsg, const char* aElement, const char* aName);
    static void Change(const char* aMsg, const wchar_t* aName);
    static void Stats (uint64_t aValue, const char* aUnit);

    static void Error_Begin();
    static void Error_End  ();

    static void Info_Begin();
    static void Info_End  ();

    static void Instruction_Begin();
    static void Instruction_End  ();

    static void Progress_Begin(const char* aMsg);
    static void Progress_Begin(const char* aMsg, const char* aFileName);
    static void Progress_End  (const char* aMsg);

    static void Warning_Begin();
    static void Warning_End  ();

};
