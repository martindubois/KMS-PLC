
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/EBPro/LabelList.h

#pragma once

// ===== C++ ================================================================
#include <string>

// ===== Local ==============================================================
#include "../Types.h"

namespace EBPro
{

    class Address
    {

    public:

        Address(const char* aLine, unsigned int aLineNo);

        Address(const char* aName, AddressType aType, unsigned int aAddr, unsigned int aLineNo);

        const char * GetAddress       () const;
        uint16_t     GetAddress_UInt16() const;
        unsigned int GetLineNo        () const;
        const char * GetName          () const;
        AddressType  GetType          () const;

        void GetLine(char* aOut, unsigned int aOutSize_byte) const;

        bool Set(AddressType aType, unsigned int aAddr);

    private:

        std::string  mAddress;
        std::string  mDataType;
        std::string  mComment;
        unsigned int mLineNo;
        std::string  mName;
        AddressType  mType;

    };

}
