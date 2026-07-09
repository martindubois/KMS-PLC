
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      EBPro_Build_Label/Builder_Label.h

#pragma once

// ===== Local ==============================================================
#include "../Common/EBPro/Config.h"
#include "../Common/HMI/Builder_Label.h"

class Builder_Label final : public HMI::Builder_Label
{

public:

    Builder_Label();

    // ===== Builder ========================================================
    virtual ~Builder_Label();

protected:

    // ===== HMI::Builder_Label =============================================
    virtual void ProcessLabel(const HMI::Label& aLabel) override;
    virtual void ProcessLabelCount(unsigned int aCount) override;

private:

    EBPro::Config mConfig;
    unsigned int  mIndex;
    FILE        * mOut;

};
