
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI_ConstantList.cpp

// TEST COVERAGE  2023-08-08  KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== Local ==============================================================
#include "TRiLOGI/Constant.h"

using namespace KMS;

namespace TRiLOGI
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    Constant::Constant(const char* aName, unsigned int aIndex, unsigned int aLineNo, const char* aValue, const char* aComment, unsigned int aFlags)
        : Object(aName, aIndex, aLineNo, aFlags), mComment(aComment), mValue(aValue)
    {
        assert(nullptr != aValue);
    }

    bool Constant::SetValue(const char* aValue, Text::File_UTF16* aFile_PC6)
    {
        assert(nullptr != aValue);

        bool lResult = (mValue != aValue);
        if (lResult)
        {
            ::Console::Change("Constant changed (NOT TESTED)", GetName(), mValue.c_str(), aValue);

            mValue = aValue;

            Update(aFile_PC6);
        }

        return lResult;
    }

    void Constant::Verify()
    {
        if (0 >= mValue.size())
        {
            AddFlags(Object::FLAG_NOT_USED);

            ::Console::Warning_Begin(GetLineNo())
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

        if (mComment.empty())
        {
            swprintf_s(aOut SizeInfoV(lOutLen), L"%u,%S,%S", GetIndex(), GetName(), mValue.c_str());
        }
        else
        {
            // NOT TESTED
            swprintf_s(aOut SizeInfoV(lOutLen), L"%u,%S,%S,%S", GetIndex(), GetName(), mValue.c_str(), mComment.c_str());
        }
    }

}
