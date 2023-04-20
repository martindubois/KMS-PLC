
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/EBPro/DataType.h

#pragma once

namespace EBPro
{

    enum class DataType
    {
        TYPE_BOOL = 0,
        TYPE_FLOAT,
        TYPE_INT,
        TYPE_NONE,
        TYPE_SHORT,
        TYPE_UNSIGNED_INT,
        TYPE_UNSIGNED_SHORT,
        TYPE_VOID,
    };

}
