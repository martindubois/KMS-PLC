
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/HMI/Config.h

#pragma once

// ===== C++ ================================================================
#include <string>
#include <vector>

namespace HMI
{

    class Config final
    {

    public:

        Config();

        const char* Label_GetSource(unsigned int aIndex) const;

        unsigned int Label_GetSourceCount() const;

    private:

        void Label_AddSource(const char* aSource);

        typedef std::vector<std::string> SourceList;

        SourceList mLabel_Sources;

    };

}
