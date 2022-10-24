
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGY_ObjectList.cpp

#include "Component.h"

// ===== C++ ================================================================
#include <codecvt>

// ===== Import/Includes ====================================================
#include <KMS/Console/Color.h>

// ===== Local ==============================================================
#include "../Common/TRiLOGY/Object.h"

#include "../Common/TRiLOGY/ObjectList.h"

namespace TRiLOGY
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    ObjectList::ObjectList(const char* aElementName, const wchar_t* aEndMark, unsigned int aMaxQty)
        : mElementName(aElementName), mEndMark(aEndMark), mFile_PC6(NULL), mLineNo_End(0), mMaxQty(aMaxQty)
    {
        assert(NULL != aElementName);
        assert(NULL != aEndMark);
    }

    ObjectList::~ObjectList() { Clear(); }

    bool ObjectList::Apply()
    {
        assert(NULL != mFile_PC6);

        bool lResult = false;

        for (ByIndex::reverse_iterator lIt = mObjects_ByIndex.rbegin(); lIt != mObjects_ByIndex.rend(); lIt++)
        {
            if (lIt->second->TestFlag(Object::FLAG_TO_INSERT))
            {
                lResult = true;

                wchar_t lLine[LINE_LENGTH];

                lIt->second->GetLine(lLine, sizeof(lLine));

                mFile_PC6->InsertLine(lIt->second->GetLineNo(), lLine);
            }
        }

        return lResult;
    }

    void ObjectList::Clear()
    {
        mFile_PC6 = NULL;

        for (const ByIndex::value_type& lVT : mObjects_ByIndex)
        {
            assert(NULL != lVT.second);

            delete lVT.second;
        }

        mObjects_ByIndex.clear();
        mObjects_ByName .clear();
    }

    unsigned int ObjectList::GetCount() const { return static_cast<unsigned int>(mObjects_ByIndex.size()); }

    const Object* ObjectList::FindObject_ByIndex(unsigned int aIndex) const
    {
        ByIndex::const_iterator lIt = mObjects_ByIndex.find(aIndex);
        if (mObjects_ByIndex.end() == lIt)
        {
            return NULL;
        }

        assert(NULL != lIt->second);

        return lIt->second;
    }

    Object* ObjectList::FindObject_ByIndex(unsigned int aIndex)
    {
        ByIndex::iterator lIt = mObjects_ByIndex.find(aIndex);
        if (mObjects_ByIndex.end() == lIt)
        {
            return NULL;
        }

        assert(NULL != lIt->second);

        return lIt->second;
    }

    Object* ObjectList::FindObject_ByName(const char* aName)
    {
        ByName::iterator lIt = mObjects_ByName.find(aName);
        if (mObjects_ByName.end() == lIt)
        {
            return NULL;
        }

        assert(NULL != lIt->second);

        return lIt->second;
    }

    unsigned int ObjectList::Parse(KMS::Text::File_UTF16* aFile_PC6, unsigned int aLineNo)
    {
        assert(NULL != mEndMark);

        mFile_PC6 = aFile_PC6;

        unsigned int lLineCount = aFile_PC6->GetLineCount();
        unsigned int lLineNo    = aLineNo;

        for (; lLineNo < lLineCount; lLineNo++)
        {
            const wchar_t* lLine = aFile_PC6->GetLine(lLineNo);
            assert(NULL != lLine);

            if (0 == wcscmp(mEndMark, lLine)) { SetLineNo_End(lLineNo); lLineNo++; break; }

            AddObject(lLine, lLineNo);
        }

        std::cout << "    " << GetCount() << " " << GetElementName() << "s" << std::endl;

        return lLineNo;
    }

    void ObjectList::Verify(const KMS::Text::File_UTF16& aFile_PC6)
    {
        assert(NULL != mElementName);

        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> lConverter;

        unsigned int lCount = 0;

        for (ByIndex::value_type& lVT : mObjects_ByIndex)
        {
            Object* lObject = lVT.second;
            assert(NULL != lObject);

            switch (aFile_PC6.CountOccurrence(lConverter.from_bytes(lObject->GetName()).c_str()))
            {
            case 0: assert(false);

            case 1:
                lCount++;
                lObject->AddFlags(Object::FLAG_NOT_USED);
                std::cout << KMS::Console::Color::YELLOW << "    WARNING  The " << mElementName << " named " << lObject->GetName();
                std::cout << " (" << lObject->GetIndex() << ") is useless" << KMS::Console::Color::WHITE << std::endl;
                break;

            case 2:
                std::cout << "    INFO  The " << mElementName << " named " << lObject->GetName();
                std::cout << " (" << lObject->GetIndex() << ") is used only once" << std::endl;
                break;
            }
        }

        if (0 < lCount)
        {
            std::cout << KMS::Console::Color::YELLOW << "    WARNING  " << lCount << " " << mElementName << "s not used" << KMS::Console::Color::WHITE << std::endl;
        }
    }

    // Protected
    // //////////////////////////////////////////////////////////////////////

    const char* ObjectList::GetElementName() const { return mElementName; }

    KMS::Text::File_UTF16* ObjectList::GetFile_PC6() { return mFile_PC6; }

    void ObjectList::SetLineNo_End(unsigned int aLineNo) { mLineNo_End = aLineNo; }

    void ObjectList::AddObject(Object* aObject)
    {
        assert(NULL != aObject);

        std::pair<ByIndex::iterator, bool> lBI = mObjects_ByIndex.insert(ObjectList::ByIndex::value_type(aObject->GetIndex(), aObject));
        KMS_EXCEPTION_ASSERT(lBI.second, APPLICATION_ERROR, "An object with the same index already exist", aObject->GetIndex());

        std::pair<ByName::iterator, bool> lBN = mObjects_ByName.insert(ObjectList::ByName::value_type(aObject->GetName(), aObject));
        KMS_EXCEPTION_ASSERT(lBN.second, APPLICATION_ERROR, "An object with the same name already exist", aObject->GetName());
    }

    void ObjectList::AddObject(const wchar_t* aLine, unsigned int aLineNo)
    {
        assert(NULL != aLine);

        unsigned int lIndex;
        char         lName[NAME_LENGTH];

        int lRet = swscanf_s(aLine, L"%u,%S", &lIndex, lName SizeInfo(lName));
        KMS_EXCEPTION_ASSERT(2 == lRet, APPLICATION_ERROR, "Invalid bit line", lRet);

        Object* lObject = new Object(lName, lIndex, aLineNo, 0);

        AddObject(lObject);
    }

    void ObjectList::FindIndexAndLineNo(unsigned int* aIndex, unsigned int* aLineNo)
    {
        unsigned int lIndex  = mMaxQty - 1;
        unsigned int lLineNo = mLineNo_End;

        for (ByIndex::reverse_iterator lIt = mObjects_ByIndex.rbegin(); lIt != mObjects_ByIndex.rend(); lIt++)
        {
            if (lIndex > lIt->first)
            {
                *aIndex  = lIndex;
                *aLineNo = lLineNo;
                return;
            }

            lIndex  = lIt->first - 1;
            lLineNo = lIt->second->GetLineNo();
        }

        KMS_EXCEPTION(APPLICATION_ERROR, "Too many", mElementName);
    }

    unsigned int ObjectList::FindLineNo(unsigned int aIndex)
    {
        unsigned int lLineNo = mLineNo_End;

        for (ByIndex::reverse_iterator lIt = mObjects_ByIndex.rbegin(); lIt != mObjects_ByIndex.rend(); lIt++)
        {
            if (aIndex > lIt->first)
            {
                return lLineNo;
            }

            lLineNo = lIt->second->GetLineNo();
        }

        KMS_EXCEPTION(APPLICATION_ERROR, "Too many", mElementName);
    }

}
