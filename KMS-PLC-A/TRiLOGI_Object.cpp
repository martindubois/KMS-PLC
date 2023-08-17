
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI_Object.cpp

// TEST COVERAGE  2023-08-15  KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== Local ==============================================================
#include "TRiLOGI/Object.h"

using namespace KMS;

namespace TRiLOGI
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    const unsigned int Object::FLAG_NOT_USED           = 0x00000001;
    const unsigned int Object::FLAG_SINGLE_USE_INFO    = 0x00000002;
    const unsigned int Object::FLAG_SINGLE_USE_WARNING = 0x00000004;

    Object::Object(const char* aName, unsigned int aIndex, unsigned int aFlags)
        : mFlags(aFlags), mIndex(aIndex), mName(aName)
    {
        assert(nullptr != aName);
    }

    Object::~Object() {}

    void Object::AddFlags(unsigned int aFlags) { mFlags |= aFlags; }

    unsigned int Object::GetIndex() const { return mIndex; }

    const char* Object::GetName() const { return mName.c_str(); }

    void Object::SetName(const char* aName)
    {
        assert(nullptr != aName);

        mName   = aName;
    }

    bool Object::TestFlag(unsigned int aFlag) const
    {
        assert(0 != aFlag);

        return 0 != (mFlags & aFlag);
    }

    void Object::GetLine(wchar_t* aOut, unsigned int aOutSize_byte) const
    {
        assert(nullptr != aOut);

        swprintf_s(aOut SizeInfoV(aOutSize_byte / sizeof(wchar_t)), L"%u,%S", GetIndex(), GetName());
    }

    // Protected
    // //////////////////////////////////////////////////////////////////////

    Object::Object(unsigned int aIndex) : mFlags(0), mIndex(aIndex) {}

}
