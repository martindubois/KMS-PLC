
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      HMI/Builder_Label.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "../Common/HMI/Config.h"
#include "../Common/HMI/HMI.h"
#include "../Common/HMI/Label.h"
#include "../Common/Parser.h"

#include "../Common/HMI/Builder_Label.h"

using namespace KMS;

namespace HMI
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    void Builder_Label::Build()
    {
        Config lConfig;

        auto lSourceCount = lConfig.Label_GetSourceCount();

        auto lLabelCount = CountLabels(LABELS_TXT);

        for (unsigned int i = 0; i < lSourceCount; i++)
        {
            lLabelCount += CountLabels(lConfig.Label_GetSource(i));
        }

        ProcessLabelCount(lLabelCount);

        ProcessLabels(LABELS_TXT);

        for (unsigned int i = 0; i < lSourceCount; i++)
        {
            ProcessLabels(lConfig.Label_GetSource(i));
        }
    }

    // Protected
    // //////////////////////////////////////////////////////////////////////

    Builder_Label::Builder_Label() {}

    Builder_Label::~Builder_Label() {}

    // Private
    // //////////////////////////////////////////////////////////////////////

    unsigned int Builder_Label::CountLabels(const char* aSource)
    {
        unsigned int lResult = 0;

        Parser lParser(aSource);

        char lLine[LINE_LENGTH];

        while (lParser.GetNextLine(lLine, sizeof(lLine)))
        {
            if (0 == strncmp("LABEL", lLine, 5))
            {
                lResult++;
            }
        }

        return lResult;
    }

    void Builder_Label::ProcessLabels(const char* aSource)
    {
        std::cout << "\n\n" << aSource << std::endl;

        Parser lParser(aSource);

        Label lLabel;

        while (lLabel.Read(&lParser))
        {
            ProcessLabel(lLabel);

            lLabel.Clear();
        }
    }

}
