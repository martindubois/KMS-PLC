
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

    Constant::Constant(const char* aName, unsigned int aIndex, unsigned int aLineNo, const char* aValue)
        : Object(aName, aIndex, aLineNo), mValue(aValue)
    {
        assert(NULL != aValue);
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

}
