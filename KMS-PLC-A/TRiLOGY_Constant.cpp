
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGY_ConstantList.cpp

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KMS/Console/Color.h>

// ===== Local ==============================================================
#include "../Common/TRiLOGY/Constant.h"

namespace TRiLOGY
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    Constant::Constant(const char* aName, unsigned int aIndex, unsigned int aLineNo, const char* aValue, const char* aComment, unsigned int aFlags)
        : Object(aName, aIndex, aLineNo, aFlags), mComment(aComment), mValue(aValue)
    {
        assert(NULL != aValue);
    }

    bool Constant::SetValue(const char* aValue, KMS::Text::File_UTF16* aFile_PC6)
    {
        bool lResult = (mValue != aValue);
        if (lResult)
        {
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
            std::cout << KMS::Console::Color::YELLOW << "    WARNING  The constant named " << GetName();
            std::cout << " (" << GetIndex() << ") has no value" << KMS::Console::Color::WHITE << std::endl;
        }
    }

    // ===== Object =========================================================

    Constant::~Constant() {}

    void Constant::GetLine(wchar_t* aOut, unsigned int aOutSize_byte) const
    {
        assert(NULL != aOut);

        unsigned int lOutLen = aOutSize_byte / sizeof(wchar_t);

        if (mComment.empty())
        {
            swprintf_s(aOut SizeInfoV(lOutLen), L"%u,%S,%S", GetIndex(), GetName(), mValue.c_str());
        }
        else
        {
            swprintf_s(aOut SizeInfoV(lOutLen), L"%u,%S,%S,%S", GetIndex(), GetName(), mValue.c_str(), mComment.c_str());
        }
    }

}
