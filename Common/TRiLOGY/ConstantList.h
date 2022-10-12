
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/TRiLOGY/ConstantList.h

#pragma once

// ===== C++ ================================================================
#include <map>
#include <string>

namespace TRiLOGY
{

    class Constant;
    class Object;

    class ConstantList
    {

    public:

        Object* AddConstant(const char* aName, unsigned int aIndex, unsigned int aLineNo, const char* aValue);

        unsigned int GetCount() const;

        void Display_ByName(FILE* aOut) const;

        void Verify() const;

    private:

        typedef std::map<std::string, Constant*> ByName;

        ByName mConstants_ByName;

    };

}
