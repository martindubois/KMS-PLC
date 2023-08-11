
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/Types.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "../Common/Types.h"

// Constants
// //////////////////////////////////////////////////////////////////////////

const char* ADDRESS_TYPE_NAMES[] =
{
    "LB",
    "LW",
    "LW_BIT",
    "RW_A",
    "RW_A_BIT",
    "MODBUS_RTU_1X",
    "MODBUS_RTU_4X",
    "UNKNOWN",
};

const char* PROJECT_TYPE_NAMES[] =
{
    "LEGACY",
    "NEW",
};
