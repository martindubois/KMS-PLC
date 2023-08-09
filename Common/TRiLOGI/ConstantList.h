
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/TRiLOGI/ConstantList.h
// Status    TEST

#pragma once

// ===== C++ ================================================================
#include <map>
#include <string>

namespace TRiLOGI
{

    class Constant;

    class ConstantList
    {

    public:

        void AddConstant(Constant* aConstant);

        void Clear();

        unsigned int GetCount() const;

        void Display_ByName(FILE* aOut) const;

        void Verify() const;

    private:

        typedef std::map<std::string, Constant*> ByName;

        ByName mConstants_ByName;

    };

}
