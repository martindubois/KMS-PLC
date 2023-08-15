
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/TRiLOGI/LineList.h
// Status    TEST

#pragma once

// ===== C++ ================================================================
#include <list>
#include <string>

// ===== Import/Includes ====================================================
#include <KMS/Text/File_UTF16.h>

namespace TRiLOGI
{

    class LineList
    {

    public:

        LineList();

        void ClearList();

        void AddToFile(KMS::Text::File_UTF16* aFile_PC6);

        unsigned int Parse(KMS::Text::File_UTF16* aFile_PC6, unsigned int aLineNo, bool aUntilEnd);

    private:

        typedef std::list<std::wstring> Internal;

        Internal mLines;

    };

}
