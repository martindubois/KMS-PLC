
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/TRiLOGY/Object.h

#pragma once

// ====== C++ ===============================================================
#include <string>

// ====== Import/Includes ===================================================
#include <KMS/Text/File_UTF16.h>

namespace TRiLOGY
{

    class Object
    {

    public:

        static const unsigned int FLAG_NOT_USED;
        static const unsigned int FLAG_SINGLE_USE_INFO;
        static const unsigned int FLAG_SINGLE_USE_WARNING;
        static const unsigned int FLAG_TO_INSERT;

        Object(const char* aName, unsigned int mIndex, unsigned int aLineNo, unsigned int aFlags);

        virtual ~Object();

        void AddFlags(unsigned int aFlags);

        unsigned int GetIndex () const;
        unsigned int GetLineNo() const;
        const char * GetName  () const;

        void SetName(const char* aName, unsigned int aLineNo);

        bool TestFlag(unsigned int aFlag) const;

        virtual void GetLine(wchar_t* aOut, unsigned int aOutSize_byte) const;

    protected:

        Object(unsigned int mIndex);

        void Update(KMS::Text::File_UTF16* aFile_PC6);

    private:

        unsigned int mFlags;
        unsigned int mIndex;
        unsigned int mLineNo;
        std::string  mName;

    };

}
