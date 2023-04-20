
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/EBPro_Function.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "EBPro/DataPtr.h"
#include "EBPro/Function.h"

using namespace KMS;

// Data type and constants
// //////////////////////////////////////////////////////////////////////////

typedef struct
{
    const char    * mCode;
    unsigned int    mCode_Length;
    EBPro::DataType mType;
    uint32_t        mValue;
}
TypeInfo;

static const TypeInfo TYPES[] =
{
    { "unsigned short", 14, EBPro::DataType::TYPE_UNSIGNED_SHORT, 5 },
    { "unsigned int"  , 12, EBPro::DataType::TYPE_UNSIGNED_INT  , 7 },
    { "float"         ,  5, EBPro::DataType::TYPE_FLOAT         , 8 },
    { "short"         ,  5, EBPro::DataType::TYPE_SHORT         , 4 },
    { "bool"          ,  4, EBPro::DataType::TYPE_BOOL          , 1 },
    { "int"           ,  3, EBPro::DataType::TYPE_INT           , 6 },

    { NULL, 0, EBPro::DataType::TYPE_NONE, 2 },
    { NULL, 0, EBPro::DataType::TYPE_VOID, 0 },
};

#define TYPE_QTY (sizeof(TYPES) / sizeof(TYPES[0]))

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

static bool ParseDataType(const char** aPtr, EBPro::DataType* aOut);
static void ParseName    (const char** aPtr, std::string    * aOut);

static uint32_t        DataType_to_UInt32(EBPro::DataType aIn);
static EBPro::DataType UInt32_to_DataType(uint32_t        aIn);

namespace EBPro
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    Function::Function() : mFunctionCode(3), mReturnType(DataType::TYPE_VOID) {}

    bool Function::operator != (const Function& aB) const
    {
        if (mCode != aB.mCode ) { return true; }

        return false;
    }

    const char* Function::GetName() const { return mName.c_str(); }

    void Function::Read(DataPtr* aPtr)
    {
        assert(NULL != aPtr);

        aPtr->Verify(104);
        aPtr->Read(&mName);

        uint32_t lType;

        aPtr->Read(&lType);

        mReturnType = UInt32_to_DataType(lType);

        uint32_t lArgCount;
        aPtr->Read(&lArgCount);
        for (unsigned int i = 0; i < lArgCount; i++)
        {
            aPtr->Read(&lType);

            mArguments.push_back(UInt32_to_DataType(lType));
        }

        aPtr->Read(&mFunctionCode);
        aPtr->Read(&mCode);
        aPtr->Read(&mComment0);
        aPtr->Read(&mComment1);
    }

    unsigned int Function::Read(const Text::File_ASCII& aFile, unsigned int aLineNo)
    {
        assert(0 < aLineNo);

        ParseDefinition(aFile.GetLine(aLineNo));

        for (unsigned int lLineNo = aLineNo; lLineNo < aFile.GetLineCount(); lLineNo++)
        {
            const char* lPtr = aFile.GetLine(lLineNo);

            if (0 == strncmp(lPtr, "FUNCTION END", 12))
            {
                return lLineNo + 1;
            }

            mCode += aFile.GetLine(lLineNo);
            mCode += "\r\n";
        }

        KMS_EXCEPTION(APPLICATION_ERROR, "No FUNCTION END before the end of the source file", "");
    }

    void Function::Write(FILE* aOut) const
    {
        DataPtr::Write(aOut, 104);
        DataPtr::Write(aOut, mName);

        DataPtr::Write(aOut, DataType_to_UInt32(mReturnType));
        DataPtr::Write(aOut, static_cast<uint32_t>(mArguments.size()));

        for (DataType lType : mArguments)
        {
            DataPtr::Write(aOut, DataType_to_UInt32(lType));
        }

        DataPtr::Write(aOut, mFunctionCode);
        DataPtr::Write(aOut, mCode);
        DataPtr::Write(aOut, mComment0);
        DataPtr::Write(aOut, mComment1);
    }

    // Private
    // //////////////////////////////////////////////////////////////////////

    void Function::ParseDefinition(const char* aLine)
    {
        assert(NULL != aLine);

        const char* lPtr = aLine;

        KMS_EXCEPTION_ASSERT(0 == strncmp("sub ", lPtr, 4), APPLICATION_USER_ERROR, "The function definition does not start with \"sub\"", aLine);
        lPtr += 4;

        ParseDataType(&lPtr, &mReturnType);
        ParseName(&lPtr, &mName);

        KMS_EXCEPTION_ASSERT(0 == strncmp("(", lPtr, 1), APPLICATION_USER_ERROR, "The function definition does not contain argument list", aLine);
        lPtr += 1;

        DataType lType;

        while (ParseDataType(&lPtr, &lType))
        {
            mArguments.push_back(lType);

            std::string lName;

            ParseName(&lPtr, &lName);

            if (',' != *lPtr)
            {
                break;
            }

            lPtr++;
        }

        KMS_EXCEPTION_ASSERT(')' == *lPtr, APPLICATION_USER_ERROR, "The argument list is not terminated", aLine);
    }

}

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

bool ParseDataType(const char** aPtr, EBPro::DataType* aOut)
{
    assert(NULL != aPtr);
    assert(NULL != *aPtr);
    assert(NULL != aOut);

    for (unsigned int i = 0; i < TYPE_QTY; i++)
    {
        if (NULL == TYPES[i].mCode)
        {
            break;
        }

        if (0 == strncmp(*aPtr, TYPES[i].mCode, TYPES[i].mCode_Length))
        {
            *aOut  = TYPES[i].mType;
            *aPtr += TYPES[i].mCode_Length;
            return true;
        }
    }

    *aOut = EBPro::DataType::TYPE_VOID;
    return false;
}

void ParseName(const char** aPtr, std::string* aOut)
{
    assert(NULL != aPtr);
    assert(NULL != *aPtr);
    assert(NULL != aOut);

    char lName[NAME_LENGTH];

    int lRet = sscanf_s(*aPtr, " %[^,(]", lName SizeInfo(lName));
    KMS_EXCEPTION_ASSERT(1 == lRet, APPLICATION_ERROR, "Corrupted source file", "");

    *aOut  = lName;
    *aPtr += strlen(lName);
}

uint32_t DataType_to_UInt32(EBPro::DataType aIn)
{
    for (unsigned int i = 0; i < TYPE_QTY; i++)
    {
        if (TYPES[i].mType == aIn)
        {
            return TYPES[i].mValue;
        }
    }

    assert(false);
    return 0;
}

EBPro::DataType UInt32_to_DataType(uint32_t aIn)
{
    for (unsigned int i = 0; i < TYPE_QTY; i++)
    {
        if (TYPES[i].mValue == aIn)
        {
            return TYPES[i].mType;
        }
    }

    KMS_EXCEPTION(APPLICATION_ERROR, "Corrupted mlb file", aIn);
}
