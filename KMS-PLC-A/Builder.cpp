
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/Builder.cpp

#include "Component.h"

// ===== Local ===============================================================
#include "../Common/Builder.h"

// Public
// ///////////////////////////////////////////////////////////////////////////

Builder::~Builder() {}

void Builder::AddSources(int aCount, const char** aVector)
{
    assert(nullptr != aVector);

    for (int i = 0; i < aCount; i++)
    {
        AddSource(aVector[i]);
    }
}

void Builder::Build()
{
    for (const auto& lSource : mSources)
    {
        std::cout << "Reading " << lSource << std::endl;

        ReadSource(lSource.c_str());
    }
}

// Protected
// ///////////////////////////////////////////////////////////////////////////

Builder::Builder() {}

void Builder::AddSource(const char* aSource)
{
    assert(nullptr != aSource);

    mSources.push_back(aSource);
}
