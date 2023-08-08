
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/Types.h
// Status    PROD_READY

#pragma once

// Data types
// //////////////////////////////////////////////////////////////////////////

enum class AddressType
{
    LOCAL_HMI_LB = 0,
    LOCAL_HMI_LW,
    LOCAL_HMI_LW_BIT,
    LOCAL_HMI_RW_A,
    LOCAL_HMI_RW_A_BIT,

    MODBUS_RTU_1X,
    MODBUS_RTU_4X,

    UNKNOWN,

    QTY
};
