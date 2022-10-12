
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// Product   KMS-PLC
// File      Common/TRiLOGY/Object.h

#pragma once

// ====== C++ ===============================================================
#include <string>

namespace TRiLOGY
{

    class Object
    {

    public:

        static const unsigned int FLAG_NOT_USED;
        static const unsigned int FLAG_TO_APPLY;

        Object(const char* aName, unsigned int mIndex, unsigned int aLineNo);

        virtual ~Object();

        void AddFlags(unsigned int aFlags);

        unsigned int GetIndex () const;
        unsigned int GetLineNo() const;
        const char * GetName  () const;

        void SetName(const char* aName, unsigned int aLineNo);

        bool TestFlag(unsigned int aFlag) const;

    protected:

        Object(unsigned int mIndex);

    private:

        unsigned int mFlags;
        unsigned int mIndex;
        unsigned int mLineNo;
        std::string  mName;

    };

}
