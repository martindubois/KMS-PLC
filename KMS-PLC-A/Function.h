
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-A/Function.h

#pragma once

// ===== C++ ================================================================
#include <string>
#include <vector>

// ===== Import/Includes ====================================================
#include <KMS/Text/File_ASCII.h>

class Function
{

public:

    bool operator != (const Function& aB) const;

    void AddLine(const char* aLine);

    void Parse(KMS::Text::File_ASCII* aFile, KMS::Text::File_ASCII::Internal::iterator* aIt, const char* aEndMark);

protected:

    typedef std::vector<std::string> StringList;

    unsigned int GetLength() const;

    const char* GetLine(unsigned int aIndex) const;

    unsigned int GetLineCount() const;

    StringList mLines;

};
