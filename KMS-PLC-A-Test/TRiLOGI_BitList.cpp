
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A-Test/TRiLOGI_BitList.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "../Common/TRiLOGI/BitList.h"

using namespace KMS;

KMS_TEST(TRiLOGI_BitList_Base, "Auto", sTest_Base)
{
    // Constructor
          TRiLOGI::BitList lBL  ("Test", 0, 64);
    const TRiLOGI::BitList lBL_C("Test", 0, 64);

    // GetAddresses

    // Display_ByName

    // Display_ByIndex

    // Import

    // ===== ObjectList =====================================================

    // Clear

    // GetCount

    // FindObject_ByIndex
    KMS_TEST_ASSERT(nullptr == lBL_C.FindObject_ByIndex(0));

    // FindObject_ByName

    // Clean

    // Parse

    // Verify
}

KMS_TEST(TRiLOGI_BitList_Exception, "Auto", sTest_Exception)
{

    // ===== ObjectList =====================================================

}
