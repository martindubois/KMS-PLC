
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI/Object.h

#pragma once

// ====== C++ ===============================================================
#include <string>

// ====== Import/Includes ===================================================
#include <KMS/Text/File_UTF16.h>

namespace TRiLOGI
{

    class Object
    {

    public:

        static const unsigned int FLAG_NOT_USED;
        static const unsigned int FLAG_SINGLE_USE_INFO;
        static const unsigned int FLAG_SINGLE_USE_WARNING;

        Object(const char* aName, unsigned int mIndex, unsigned int aFlags);

        virtual ~Object();

        void AddFlags(unsigned int aFlags);

        unsigned int GetIndex () const;
        const char * GetName  () const;

        void SetName(const char* aName);

        bool TestFlag(unsigned int aFlag) const;

        virtual void GetLine(wchar_t* aOut, unsigned int aOutSize_byte) const;

    protected:

        Object(unsigned int mIndex);

    private:

        unsigned int mFlags;
        unsigned int mIndex;
        std::string  mName;

    };

}
