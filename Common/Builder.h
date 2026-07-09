
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/Builder.h

#pragma once

// ===== C++ ================================================================
#include <list>
#include <string>

class Builder
{

public:

    virtual ~Builder();

    void AddSources(int aCount, const char** aVector);

    virtual void Build();

protected:

    Builder();

    void AddSource(const char* aSource);

    virtual void ReadSource(const char* aSource) = 0;

    std::list<std::string> mSources;

};
