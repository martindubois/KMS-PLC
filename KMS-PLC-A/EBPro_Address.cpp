
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/EBPro_Address.cpp

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KMS/Convert.h>

// ===== Local ==============================================================
#include "../Common/EBPro/Address.h"

// Constants
// //////////////////////////////////////////////////////////////////////////

static const char* ADDRESS_TYPE_NAMES[] =
{
    "Local HMI",
    "Local HMI",
    "Local HMI",
    "Local HMI",
    "Local HMI",
    "MODBUS RTU",
    "MODBUS RTU",
    "UNKNOWN",
};

static const char* ADDRESS_SUB_TYPE_NAMES[] =
{
    "LB",
    "LW",
    "LW_Bit",
    "RW_A",
    "RW_A_Bit",
    "1x",
    "4x",
    "",
};

namespace EBPro
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    Address::Address(const char* aLine, unsigned int aLineNo) : mType(AddressType::UNKNOWN)
    {
        char lAddress [NAME_LENGTH];
        char lComment [LINE_LENGTH];
        char lDataType[NAME_LENGTH];
        char lMsg     [LINE_LENGTH];
        char lName    [NAME_LENGTH];
        char lType    [NAME_LENGTH];
        char lSubType [NAME_LENGTH];

        sprintf_s(lMsg, "Line %u  The CSV file is corrupted", aLineNo);

        int lRet = sscanf_s(aLine, "%[^,],%[^,],%[^,],%[^,],%[^,],%s",
            lName SizeInfo(lName), lType SizeInfo(lType), lSubType SizeInfo(lSubType),
            lAddress SizeInfo(lAddress), lComment SizeInfo(lComment), lDataType SizeInfo(lDataType));
        switch (lRet)
        {
        case 6: mComment = lComment; break;
        case 4:
            lRet = sscanf_s(aLine, "%[^,],%[^,],%[^,],%[^,],,%s",
                lName SizeInfo(lName), lType SizeInfo(lType), lSubType SizeInfo(lSubType),
                lAddress SizeInfo(lAddress), lDataType SizeInfo(lDataType));

            KMS_EXCEPTION_ASSERT(5 == lRet, APPLICATION_ERROR, lMsg, aLine);
            break;

        default: KMS_EXCEPTION(APPLICATION_ERROR, lMsg, aLine);
        }

        if (0 == strcmp("Local HMI", lType))
        {
            if      (0 == strcmp("LB"      , lSubType)) { mType = AddressType::LOCAL_HMI_LB; }
            else if (0 == strcmp("LW"      , lSubType)) { mType = AddressType::LOCAL_HMI_LW; }
            else if (0 == strcmp("LW_Bit"  , lSubType)) { mType = AddressType::LOCAL_HMI_LW_BIT; }
            else if (0 == strcmp("RW_A"    , lSubType)) { mType = AddressType::LOCAL_HMI_RW_A; }
            else if (0 == strcmp("RW_A_Bit", lSubType)) { mType = AddressType::LOCAL_HMI_RW_A_BIT; }
        }
        else if (0 == strcmp("MODBUS RTU", lType))
        {
            if      (0 == strcmp("1x", lSubType)) { mType = AddressType::MODBUS_RTU_1X; }
            else if (0 == strcmp("4x", lSubType)) { mType = AddressType::MODBUS_RTU_4X; }
        }

        mAddress  = lAddress;
        mDataType = lDataType;
        mLineNo   = aLineNo;
        mName     = lName;
    }

    Address::Address(const char* aName, AddressType aType, uint16_t aAddr, unsigned int aLineNo)
        : mDataType("Undesignated")
        , mLineNo(aLineNo)
        , mName(aName)
        , mType(aType)
    {
        assert(NULL != aName);

        char lAddr[32];

        sprintf_s(lAddr, "%u", aAddr);

        mAddress = lAddr;
    }

    const char  * Address::GetAddress() const { return mAddress.c_str(); }

    uint16_t Address::GetAddress_UInt16() const
    {
        return KMS::Convert::ToUInt16(mAddress.c_str());
    }

    unsigned int Address::GetLineNo() const { return mLineNo; }
    const char * Address::GetName  () const { return mName   .c_str(); }
    AddressType  Address::GetType  () const { return mType; }

    void Address::GetLine(char* aOut, unsigned int aOutSize_byte) const
    {
        sprintf_s(aOut SizeInfoV(aOutSize_byte), "%s,%s,%s,%s,%s,%s", mName.c_str(),
            ADDRESS_TYPE_NAMES[static_cast<unsigned int>(mType)], ADDRESS_SUB_TYPE_NAMES[static_cast<unsigned int>(mType)],
            mAddress.c_str(), mComment.c_str(), mDataType.c_str());
    }

    bool Address::Set(AddressType aType, unsigned int aAddr)
    {
        char lMsg[128 + NAME_LENGTH];
        sprintf_s(lMsg, "The improted address type of \"%s\" does not match the current type", mName.c_str());
        KMS_EXCEPTION_ASSERT(mType == aType, APPLICATION_ERROR, lMsg, "");

        bool lResult = GetAddress_UInt16() != aAddr;
        if (lResult)
        {
            char lAddress[32];

            sprintf_s(lAddress, "%u", aAddr);

            mAddress = lAddress;
        }

        return lResult;
    }

}
