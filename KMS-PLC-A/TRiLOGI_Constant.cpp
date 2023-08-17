
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI_ConstantList.cpp

// TEST COVERAGE  2023-08-15  KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== Local ==============================================================
#include "TRiLOGI/Constant.h"

using namespace KMS;

namespace TRiLOGI
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    Constant::Constant(const char* aName, unsigned int aIndex, const char* aValue, const char* aComment)
        : Object(aName, aIndex, 0), mComment(aComment), mValue(aValue)
    {}

    bool Constant::SetValue(const char* aValue)
    {
        assert(nullptr != aValue);

        bool lResult = (mValue != aValue);
        if (lResult)
        {
            ::Console::Change("Constant changed (NOT TESTED)", GetName(), mValue.c_str(), aValue);

            mValue = aValue;
        }

        return lResult;
    }

    void Constant::Verify()
    {
        if (0 >= mValue.size())
        {
            AddFlags(Object::FLAG_NOT_USED);

            ::Console::Warning_Begin()
                << "The constant named \"" << GetName() << "\" (" << GetIndex() << ") has no value (NOT TESTED)";
            ::Console::Warning_End();
        }
    }

    // ===== Object =========================================================

    Constant::~Constant() {}

    void Constant::GetLine(wchar_t* aOut, unsigned int aOutSize_byte) const
    {
        assert(nullptr != aOut);

        unsigned int lOutLen = aOutSize_byte / sizeof(wchar_t);

        auto lI = GetIndex();
        auto lN = GetName ();
        auto lV = mValue.c_str();

        if (mComment.empty())
        {
            swprintf_s(aOut SizeInfoV(lOutLen), L"%u,%S,%S", lI, lN, lV);
        }
        else
        {
            // NOT TESTED
            swprintf_s(aOut SizeInfoV(lOutLen), L"%u,%S,%S,%S", lI, lN, lV, mComment.c_str());
        }
    }

}
