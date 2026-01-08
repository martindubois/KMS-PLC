
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// Product   KMS-PLC
// File      Common/Parser.h

#pragma once

// ===== C ==================================================================
#include <stdio.h>

class Parser
{

public:

    Parser(const char* aFileName);

    ~Parser();

    bool IsAtEnd();

    bool GetNextLine(char* aOut, unsigned int aOutSize_byte);

private:

    FILE* mFile;

};
