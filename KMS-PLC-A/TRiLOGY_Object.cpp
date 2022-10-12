
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGY_Object.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "../Common/TRiLOGY/Object.h"

namespace TRiLOGY
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    const unsigned int Object::FLAG_NOT_USED = 0x00000001;
    const unsigned int Object::FLAG_TO_APPLY = 0x00000002;

    Object::Object(const char* aName, unsigned int aIndex, unsigned int aLineNo)
        : mFlags(0), mIndex(aIndex), mLineNo(aLineNo), mName(aName)
    {
        assert(NULL != aName);
    }

    Object::~Object() {}

    void Object::AddFlags(unsigned int aFlags) { mFlags |= aFlags; }

    unsigned int Object::GetIndex() const { return mIndex; }

    const char* Object::GetName() const { return mName.c_str(); }

    void Object::SetName(const char* aName, unsigned int aLineNo) { assert(NULL != aName); mLineNo = aLineNo; mName = aName; }

    bool Object::TestFlag(unsigned int aFlag) const { assert(0 != aFlag); return 0 != (mFlags & aFlag); }

    // Protected
    // //////////////////////////////////////////////////////////////////////

    Object::Object(unsigned int aIndex) : mFlags(0), mIndex(aIndex), mLineNo(0) {}

}
