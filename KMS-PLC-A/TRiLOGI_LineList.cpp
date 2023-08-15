
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI_LineList.cpp

// TEST COVERAGE  2023-08-15  KMS - Martin Dubois. P. Eng.

#include "Component.h"

// ===== Local ==============================================================
#include "../Common/TRiLOGI/LineList.h"

#include "TRiLOGI/PC6.h"

using namespace KMS;

namespace TRiLOGI
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    LineList::LineList() {}

    void LineList::ClearList() { mLines.clear(); }

    unsigned int LineList::Parse(Text::File_UTF16* aFile_PC6, unsigned int aLineNo, bool aUntilEnd)
    {
        assert(nullptr != aFile_PC6);
        assert(0 < aLineNo);

        auto lLineCount = aFile_PC6->GetLineCount();
        auto lLineNo    = aLineNo;

        for (; lLineNo < lLineCount; lLineNo++)
        {
            const wchar_t* lLine = aFile_PC6->GetLine(lLineNo);

            if ((!aUntilEnd) && (PC6_SECTION_END_C == lLine[0]))
            {
                lLineNo++;
                break;
            }

            mLines.push_back(lLine);
        }

        return lLineNo;
    }

}
