
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/Version.h

#pragma once

// ===== Includes ===========================================================
#ifdef __cplusplus
    #include <KMS/Version.h>
#endif

// Constants
// //////////////////////////////////////////////////////////////////////////

#define VERSION_RC  0,0,10,0
#define VERSION_STR "0.0.10.0"

#ifdef __cplusplus
    KMS_VERSION("dev");
#endif
