
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/TRiLOGI/ObjectList.h
// Status    TEST

#pragma once

// ===== C++ ================================================================
#include <map>
#include <string>

// ===== Import/Includes ====================================================
#include <KMS/Text/File_UTF16.h>

// ===== Local ==============================================================
#include "../AddressList.h"

namespace TRiLOGI
{

    class Object;

    class ObjectList
    {

    public:

        ~ObjectList();

        void AddToFile(KMS::Text::File_UTF16* aFile_PC6);
        void ClearList();

        unsigned int GetCount() const;

        const Object* FindObject_ByIndex(unsigned int aIndex) const;

        Object* FindObject_ByIndex(unsigned int aIndex);

        const Object* FindObject_ByName(const char* aName) const;

        Object* FindObject_ByName(const char* aName);

        unsigned int Clean();

        unsigned int Parse(KMS::Text::File_UTF16* aFile_PC6, unsigned int aLineNo, unsigned int aFlags);

        void Verify(const KMS::Text::File_UTF16& aFile_CP6, const AddressList* aPublicAddresses);

        void SetProjectType(ProjectType aPT);

    protected:

        typedef std::map<unsigned int, Object*> ByIndex;
        typedef std::map<std::string , Object*> ByName;

        ObjectList(const char* aElementName, unsigned int aMaxQty);

        const char* GetElementName() const;

        bool IsProjectLegacy() const;

        void AddObject(Object* aObject);

        virtual void AddObject(const wchar_t* aLine, unsigned int aFlags);

        unsigned int FindFreeIndex();

        void Replace(Object* aOld, Object* aNew);

        ByIndex mObjects_ByIndex;
        ByName  mObjects_ByName;

    private:

        NO_COPY(ObjectList);

        const char   * mElementName;

        unsigned int mMaxQty;

        ProjectType mProjectType;

    };

}
