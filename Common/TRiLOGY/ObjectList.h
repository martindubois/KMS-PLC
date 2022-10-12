
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// Product   KMS-PLC
// File      Common/TRiLOGY/ObjectList.h

#pragma once

// ===== C++ ================================================================
#include <map>
#include <string>

// ===== Import/Includes ====================================================
#include <KMS/Text/File_UTF16.h>

namespace TRiLOGY
{

    class Object;

    class ObjectList
    {

    public:

        ObjectList(const char* aElementName, const wchar_t* aEndMark);

        ~ObjectList();

        unsigned int GetCount() const;

        const Object* FindObject_ByIndex(unsigned int aIndex) const;

        Object* FindObject_ByIndex(unsigned int aIndex);

        unsigned int Parse(const KMS::Text::File_UTF16& aFile_PC6, unsigned int aLineNo);

        void Verify(const KMS::Text::File_UTF16& aFile_CP6);

    protected:

        typedef std::map<unsigned int, Object*> ByIndex;
        typedef std::map<std::string , Object*> ByName;

        const char* GetElementName() const;

        void SetLineNo_End(unsigned int aLineNo);

        void AddObject(Object* aObject);

        virtual void AddObject(const wchar_t* aLine, unsigned int aLineNo);

        ByIndex mObjects_ByIndex;
        ByName  mObjects_ByName;

    private:

        ObjectList(const ObjectList&);

        const ObjectList& operator = (const ObjectList&);

        const char   * mElementName;
        const wchar_t* mEndMark;

        unsigned int mLineNo_End;

    };

}
