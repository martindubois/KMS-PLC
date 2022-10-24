
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
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

        ObjectList(const char* aElementName, const wchar_t* aEndMark, unsigned int aMaxQty);

        ~ObjectList();

        bool Apply();
        void Clear();

        unsigned int GetCount() const;

        const Object* FindObject_ByIndex(unsigned int aIndex) const;

        Object* FindObject_ByIndex(unsigned int aIndex);

        const Object* FindObject_ByName(const char* aName) const;

        Object* FindObject_ByName(const char* aName);

        unsigned int Parse(KMS::Text::File_UTF16* aFile_PC6, unsigned int aLineNo);

        void Verify(const KMS::Text::File_UTF16& aFile_CP6);

    protected:

        typedef std::map<unsigned int, Object*> ByIndex;
        typedef std::map<std::string , Object*> ByName;

        const char* GetElementName() const;

        KMS::Text::File_UTF16* GetFile_PC6();

        void SetLineNo_End(unsigned int aLineNo);

        void AddObject(Object* aObject);

        virtual void AddObject(const wchar_t* aLine, unsigned int aLineNo);

        void FindIndexAndLineNo(unsigned int* aIndex, unsigned int* aLineNo);

        unsigned int FindLineNo(unsigned int aIndex);

        ByIndex mObjects_ByIndex;
        ByName  mObjects_ByName;

    private:

        ObjectList(const ObjectList&);

        const ObjectList& operator = (const ObjectList&);

        const char   * mElementName;
        const wchar_t* mEndMark;

        KMS::Text::File_UTF16* mFile_PC6;

        unsigned int mLineNo_End;
        unsigned int mMaxQty;

    };

}
