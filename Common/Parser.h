
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/Parser.h

#pragma once

// ===== C ==================================================================
#include <stdio.h>

class Parser final
{

public:

    Parser(const char* aFileName, bool aOptional = false);

    ~Parser();

    bool GetNextLine(char* aOut, unsigned int aOutSize_byte);

    bool GetNextLine_Code(char* aOut, unsigned int aOutSize_byte);

private:

    FILE* mFile;

};
