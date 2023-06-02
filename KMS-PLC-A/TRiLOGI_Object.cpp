
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI_Object.cpp

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
    const unsigned int Object::FLAG_TO_APPLY           = 0x00000008;
    const unsigned int Object::FLAG_TO_INSERT          = 0x00000010;

    Object::Object(const char* aName, unsigned int aIndex, unsigned int aLineNo, unsigned int aFlags)
        : mFlags(aFlags), mIndex(aIndex), mLineNo(aLineNo), mName(aName)
    {
        assert(NULL != aName);
    }

    Object::~Object() {}

    void Object::AddFlags(unsigned int aFlags) { mFlags |= aFlags; }

    unsigned int Object::GetIndex() const { return mIndex; }

    unsigned int Object::GetLineNo() const { return mLineNo; }

    const char* Object::GetName() const { return mName.c_str(); }

    void Object::SetName(const char* aName, unsigned int aLineNo) { assert(NULL != aName); mLineNo = aLineNo; mName = aName; }

    bool Object::TestFlag(unsigned int aFlag) const { assert(0 != aFlag); return 0 != (mFlags & aFlag); }

    void Object::GetLine(wchar_t* aOut, unsigned int aOutSize_byte) const
    {
        assert(NULL != aOut);

        swprintf_s(aOut SizeInfoV(aOutSize_byte / sizeof(wchar_t)), L"%u,%S", GetIndex(), GetName());
    }

    // Protected
    // //////////////////////////////////////////////////////////////////////

    Object::Object(unsigned int aIndex) : mFlags(0), mIndex(aIndex), mLineNo(0) {}

    void Object::Update(Text::File_UTF16* aFile_PC6)
    {
        assert(NULL != aFile_PC6);

        wchar_t lLine[LINE_LENGTH];

        GetLine(lLine, sizeof(lLine));

        aFile_PC6->ReplaceLine(mLineNo, lLine);
    }

}