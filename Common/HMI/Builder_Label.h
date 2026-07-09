
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/HMI/Builder_Label.h

#pragma once

// ===== Local ==============================================================
#include "../Common/Builder.h"

namespace HMI
{

    class Label;

    class Builder_Label : public Builder
    {

    public:

        // ===== Builder ====================================================
        virtual void Build() override;

    protected:

        Builder_Label();

        virtual void ProcessLabel(const Label& aLabel) = 0;

        virtual void ProcessLabelCount(unsigned int aCount) = 0;

        // ===== Builder ====================================================
        virtual ~Builder_Label();
        virtual void ReadSource(const char* aSource) override;

    private:

        unsigned int CountLabels(const char* aSource);

    };

}
