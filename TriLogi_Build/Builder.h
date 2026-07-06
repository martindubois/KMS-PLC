
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      TRiLOGI_Build/Builder.h

#pragma once

// ===== Local ==============================================================
#include "../Common/PLC/Builder.h"

class Builder final : public PLC::Builder
{

public:

    Builder();

    void Write();

    // ===== PLC::Builder ===================================================
    virtual ~Builder();

private:

    void Write_DEFINE        (std::wofstream& aOut);
    void Write_FUNCTION      (std::wofstream& aOut);
    void Write_FUNCTION_LABEL(std::wofstream& aOut);
    void Write_SEQUENCE      (std::wofstream& aOut);
    void Write_TIMER         (std::wofstream& aOut);

};
