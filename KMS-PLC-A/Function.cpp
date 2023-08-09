
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/Address.cpp

// TEST COVERAGE  2023-08-07  KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== Local ==============================================================
#include "Function.h"

using namespace KMS;

// Public
// //////////////////////////////////////////////////////////////////////////

// NOT TESTED  Return true
bool Function::operator != (const Function& aB) const
{
    auto lCount = mLines.size();

    if (aB.mLines.size() != lCount)
    {
        return true;
    }

    for (unsigned int i = 0; i < lCount; i++)
    {
        if (mLines[i] != aB.mLines[i])
        {
            return true;
        }
    }

    return false;
}

void Function::AddLine(const char* aLine)
{
    assert(NULL != aLine);

    mLines.push_back(aLine);
}

void Function::Parse(Text::File_ASCII* aFile, Text::File_ASCII::Internal::iterator* aIt, const char* aEndMark)
{
    assert(NULL != aFile);
    assert(NULL != aIt);
    assert(NULL != aEndMark);

    auto lEndMarkLen = strlen(aEndMark);

    for (; *aIt != aFile->mLines.end(); (*aIt)++)
    {
        if (0 == strncmp(aEndMark, (*aIt)->c_str(), lEndMarkLen))
        {
            return;
        }

        AddLine((*aIt)->c_str());
    }

    KMS_EXCEPTION(APPLICATION_ERROR, "The function is not terminated", "");
}

// Protected
// ///////////////////////////////////////////////////////////////////////////

unsigned int Function::GetLength() const
{
    size_t lResult = 0;

    for (auto& lLine : mLines)
    {
        lResult += lLine.size();
        lResult++;
    }

    return static_cast<unsigned int>(lResult);
}

const char* Function::GetLine(unsigned int aIndex) const
{
    return mLines[aIndex].c_str();
}

unsigned int Function::GetLineCount() const { return static_cast<unsigned int>(mLines.size()); }
