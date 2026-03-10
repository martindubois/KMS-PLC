
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/HMI/Builder_Label.h

#pragma once

namespace HMI
{

    class Label;

    class Builder_Label
    {

    public:

        virtual ~Builder_Label();

        void Build();

    protected:

        Builder_Label();

        virtual void ProcessLabel(const Label& aLabel) = 0;

        virtual void ProcessLabelCount(unsigned int aCount) = 0;

    private:

        unsigned int CountLabels(const char* aSource);

        void ProcessLabels(const char* aSource);

    };

}
