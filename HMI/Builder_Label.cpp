
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      HMI/Builder_Label.cpp

#include "Component.h"

// ===== Local ==============================================================
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
        unsigned int lLabelCount = 0;

        for (const auto& lSource : mSources)
        {
            lLabelCount += CountLabels(lSource.c_str());
        }

        ProcessLabelCount(lLabelCount);

        Builder::Build();
    }

    // Protected
    // //////////////////////////////////////////////////////////////////////

    Builder_Label::Builder_Label() { AddSource(LABELS_TXT); }

    // ===== Builder ========================================================

    Builder_Label::~Builder_Label() {}

    void Builder_Label::ReadSource(const char* aSource)
    {
        Parser lParser(aSource);

        Label lLabel;

        while (lLabel.Read(&lParser))
        {
            ProcessLabel(lLabel);

            lLabel.Clear();
        }
    }

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

}
