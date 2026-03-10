
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      HMI/Config.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "../Common/Parser.h"

#include "../Common/HMI/Config.h"

using namespace KMS;

// Configurations
// //////////////////////////////////////////////////////////////////////////

static const char* CONFIG_TXT = "HMI_Config.txt";

namespace HMI
{

    Config::Config()
    {
        Parser lParser(CONFIG_TXT, true);

        char lLine[LINE_LENGTH];

        while (lParser.GetNextLine(lLine, sizeof(lLine)))
        {
            char lSource[NAME_LENGTH];

            if (2 == sscanf_s(lLine, "Label %[~\n\r]", lSource SizeInfo(lSource)))
            {
                Label_AddSource(lSource);
            }
            else
            {
                KMS_EXCEPTION(RESULT_INVALID_CONFIG, "Invalid configuration", lLine);
            }
        }
    }

    const char* Config::Label_GetSource(unsigned int aIndex) const
    {
        return mLabel_Sources[aIndex].c_str();
    }

    unsigned int Config::Label_GetSourceCount() const
    {
        return static_cast<unsigned int>(mLabel_Sources.size());
    }

    // Private
    // //////////////////////////////////////////////////////////////////////

    void Config::Label_AddSource(const char* aSource)
    {
        assert(nullptr != aSource);

        mLabel_Sources.push_back(aSource);
    }

}
