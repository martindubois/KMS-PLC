
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      EBPro_Build_Address/Builder_Address.h

#pragma once

// ===== C++ ================================================================
#include <fstream>
#include <regex>

// ===== Local ==============================================================
#include "../Common/Builder.h"

class Builder_Address final : public Builder
{

public:

    Builder_Address();

    // ===== Builder ========================================================
    ~Builder_Address();
    virtual void Build() override;

protected:

    // ===== Builder ========================================================
    virtual void ReadSource(const char* aSource) override;

private:

    void Write_BIT_LOCAL_LB          (const std::smatch& aMatch);
    void Write_BIT_LOCAL_LW          (const std::smatch& aMatch);
    void Write_BIT_LOCAL_RW_A        (const std::smatch& aMatch);
    void Write_BIT_MODBUS_1X         (const std::smatch& aMatch);
    void Write_BIT_MODBUS_4X         (const std::smatch& aMatch);
    void Write_WORD_LOCAL_EM0        (const std::smatch& aMatch);
    void Write_WORD_LOCAL_LW         (const std::smatch& aMatch);
    void Write_WORD_LOCAL_LW_UINT16  (const std::smatch& aMatch);
    void Write_WORD_LOCAL_RW         (const std::smatch& aMatch);
    void Write_WORD_LOCAL_RW_A       (const std::smatch& aMatch);
    void Write_WORD_LOCAL_RW_A_UINT16(const std::smatch& aMatch);
    void Write_WORD_MODBUS_4X        (const std::smatch& aMatch);
    void Write_WORD_MODBUS_4X_INT16  (const std::smatch& aMatch);
    void Write_WORD_MODBUS_4X_UINT16 (const std::smatch& aMatch);
            
    std::ofstream* mOut;

};
