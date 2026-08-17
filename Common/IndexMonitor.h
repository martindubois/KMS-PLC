
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/IndexMonitor.h

#pragma once

// ===== C++ ================================================================
#include <map>

class IndexMonitor
{

public:

    IndexMonitor();

    ~IndexMonitor();

    void Init(const char* aName, unsigned int aFirst, unsigned int aLast);

    void MarkUsed(unsigned int aIndex);

private:

    void AddRange(unsigned int aFirst, unsigned int aLast);

    typedef std::map<unsigned int, unsigned int> RangeMap;

    const char * mName;
    unsigned int mQuantity;
    RangeMap     mRanges;

};
