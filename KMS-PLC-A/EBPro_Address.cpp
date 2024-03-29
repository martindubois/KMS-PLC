
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/EBPro_Address.cpp

// TEST COVERAGE 2023-09-14 KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KMS/Convert.h>

// ===== Local ==============================================================
#include "EBPro/Address.h"

using namespace KMS;

KMS_RESULT_STATIC(RESULT_INVALID_ADDRESS_FORMAT);
KMS_RESULT_STATIC(RESULT_INVALID_ADDRESS_TYPE);

// Constants
// //////////////////////////////////////////////////////////////////////////

typedef struct
{
    const char* mType;
    const char* mSubType;
}
AddressTypeName;

static const AddressTypeName ADDRESS_SUBTYPE_NAMES[] =
{
    { "Local HMI" , "LB"       },
    { "Local HMI" , "LW"       },
    { "Local HMI" , "LW_Bit"   },
    { "Local HMI" , "RW_A"     },
    { "Local HMI" , "RW_A_Bit" },
    { "MODBUS RTU", "1x"       },
    { "MODBUS RTU", "4x"       },
    { "UNKNOWN"   , ""         },
};

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

static AddressType ToAddressType(const char* aType, const char* aSubType);

namespace EBPro
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    Address::Address(const char* aLine, unsigned int aLineNo) : mType(AddressType::UNKNOWN)
    {
        assert(nullptr != aLine);

        char lAddress [NAME_LENGTH];
        char lComment [LINE_LENGTH];
        char lDataType[NAME_LENGTH];
        char lMsg     [LINE_LENGTH];
        char lName    [NAME_LENGTH];
        char lType    [NAME_LENGTH];
        char lSubType [NAME_LENGTH];

        sprintf_s(lMsg, "Line %u  The CSV file is corrupted", aLineNo);

        //                          Name  Type  SubTy Address     DataType
        int lRet = sscanf_s(aLine, "%[^,],%[^,],%[^,],\"%[^\"]\",,%s",
            lName     SizeInfo(lName),
            lType     SizeInfo(lType),
            lSubType  SizeInfo(lSubType),
            lAddress  SizeInfo(lAddress),
            lDataType SizeInfo(lDataType));
        if (5 != lRet)
        {
            //                      Name  Type  SubTy Addre Comme DataType
            lRet = sscanf_s(aLine, "%[^,],%[^,],%[^,],%[^,],%[^,],%s",
                lName     SizeInfo(lName),
                lType     SizeInfo(lType),
                lSubType  SizeInfo(lSubType),
                lAddress  SizeInfo(lAddress),
                lComment  SizeInfo(lComment),
                lDataType SizeInfo(lDataType));
            switch (lRet)
            {
            case 6: mComment = lComment; break;
            case 4:
                //                      Name  Type  SubTy Addre  DataType
                lRet = sscanf_s(aLine, "%[^,],%[^,],%[^,],%[^,],,%s",
                    lName     SizeInfo(lName),
                    lType     SizeInfo(lType),
                    lSubType  SizeInfo(lSubType),
                    lAddress  SizeInfo(lAddress),
                    lDataType SizeInfo(lDataType));

                KMS_EXCEPTION_ASSERT(5 == lRet, RESULT_INVALID_ADDRESS_FORMAT, lMsg, aLine);
                break;

            default:
                // NOT TESTED
                KMS_EXCEPTION(RESULT_INVALID_ADDRESS_FORMAT, lMsg, aLine);
            }
        }

        mAddress  = lAddress;
        mDataType = lDataType;
        mLineNo   = aLineNo;
        mName     = lName;
        mType     = ToAddressType(lType, lSubType);
    }

    // NOT TESTED
    Address::Address(const char* aName, AddressType aType, uint16_t aAddr, unsigned int aLineNo)
        : mDataType("Undesignated")
        , mLineNo(aLineNo)
        , mName(aName)
        , mType(aType)
    {
        assert(nullptr != aName);
        assert(AddressType::QTY > aType);

        char lAddr[32];

        sprintf_s(lAddr, "%u", aAddr);

        mAddress = lAddr;
    }

    Address::Address(const char* aName, AddressType aType, const char* aAddr, unsigned int aLineNo)
        : mAddress(aAddr)
        , mDataType("Undesignated")
        , mLineNo(aLineNo)
        , mName(aName)
        , mType(aType)
    {
        assert(nullptr != aName);
        assert(AddressType::QTY > aType);
        assert(nullptr != aAddr);
    }

    const char* Address::GetAddress() const { return mAddress.c_str(); }

    uint16_t Address::GetAddress_UInt16() const { return Convert::ToUInt16(mAddress.c_str()); }

    unsigned int Address::GetLineNo() const { return mLineNo; }
    const char * Address::GetName  () const { return mName.c_str(); }
    AddressType  Address::GetType  () const { return mType; }

    void Address::GetLine(char* aOut, unsigned int aOutSize_byte) const
    {
        assert(nullptr != aOut);

        auto lType = static_cast<unsigned int>(mType);

        sprintf_s(aOut SizeInfoV(aOutSize_byte), "%s,%s,%s,%s,%s,%s", mName.c_str(),
            ADDRESS_SUBTYPE_NAMES[lType].mType, ADDRESS_SUBTYPE_NAMES[lType].mSubType,
            mAddress.c_str(), mComment.c_str(), mDataType.c_str());
    }

    // NOT TESTED
    bool Address::Set(AddressType aType, unsigned int aAddr)
    {
        char lAddr[NAME_LENGTH];

        sprintf_s(lAddr, "%u", aAddr);

        return Set(aType, lAddr);
    }

    bool Address::Set(AddressType aType, const char* aAddr)
    {
        assert(AddressType::QTY > aType);
        assert(nullptr != aAddr);

        if (mType != aType)
        {
            char lMsg[128 + NAME_LENGTH];
            sprintf_s(lMsg, "The improted address type of \"%s\" does not match the current type (NOT TESTED)", mName.c_str());
            KMS_EXCEPTION(RESULT_INVALID_ADDRESS_TYPE, lMsg, "");
        }

        bool lResult = 0 != strcmp(mAddress.c_str(), aAddr);
        if (lResult)
        {
            ::Console::Change("Address changed (NOT TESTED)", GetName(), mAddress.c_str(), aAddr);

            mAddress = aAddr;
        }

        return lResult;
    }

}

// Static functions
// //////////////////////////////////////////////////////////////////////////

AddressType ToAddressType(const char* aType, const char* aSubType)
{
    assert(nullptr != aType);
    assert(nullptr != aSubType);

    unsigned int lResult;

    for (lResult = static_cast<unsigned int>(AddressType::LOCAL_HMI_LB); lResult < static_cast<unsigned int>(AddressType::UNKNOWN); lResult++)
    {
        if (   (0 == strcmp(ADDRESS_SUBTYPE_NAMES[lResult].mType   , aType    ))
            && (0 == strcmp(ADDRESS_SUBTYPE_NAMES[lResult].mSubType, aSubType)))
        {
            break;
        }
    }

    return static_cast<AddressType>(lResult);
}
