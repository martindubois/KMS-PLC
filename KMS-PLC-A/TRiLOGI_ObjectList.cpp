
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI_ObjectList.cpp

#include "Component.h"

// ===== C++ ================================================================
#include <codecvt>

// ===== Local ==============================================================
#include "../Common/TRiLOGI/ObjectList.h"

#include "Console.h"

#include "TRiLOGI/Object.h"

using namespace KMS;

namespace TRiLOGI
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    ObjectList::ObjectList(const char* aElementName, unsigned int aMaxQty)
        : mElementName(aElementName), mFile_PC6(NULL), mLineNo_End(0), mMaxQty(aMaxQty)
    {
        assert(NULL != aElementName);
        assert(0 < aMaxQty);
    }

    ObjectList::~ObjectList() { Clear(); }

    bool ObjectList::Apply()
    {
        assert(NULL != mFile_PC6);

        auto lResult = false;

        for (auto lIt = mObjects_ByIndex.rbegin(); lIt != mObjects_ByIndex.rend(); lIt++)
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

        for (const auto& lVT : mObjects_ByIndex)
        {
            assert(NULL != lVT.second);

            delete lVT.second;
        }

        mObjects_ByIndex.clear();
        mObjects_ByName .clear();
    }

    unsigned int ObjectList::GetCount() const { return static_cast<unsigned int>(mObjects_ByIndex.size()); }

    void ObjectList::SetFile(Text::File_UTF16* aFile_PC6, unsigned int aLineNo_End)
    {
        assert(NULL != aFile_PC6);
        assert(0 < aLineNo_End);

        mFile_PC6   = aFile_PC6;
        mLineNo_End = aLineNo_End;
    }

    const Object* ObjectList::FindObject_ByIndex(unsigned int aIndex) const
    {
        auto lIt = mObjects_ByIndex.find(aIndex);
        if (mObjects_ByIndex.end() == lIt)
        {
            return NULL;
        }

        assert(NULL != lIt->second);

        return lIt->second;
    }

    Object* ObjectList::FindObject_ByIndex(unsigned int aIndex)
    {
        auto lIt = mObjects_ByIndex.find(aIndex);
        if (mObjects_ByIndex.end() == lIt)
        {
            return NULL;
        }

        assert(NULL != lIt->second);

        return lIt->second;
    }

    Object* ObjectList::FindObject_ByName(const char* aName)
    {
        auto lIt = mObjects_ByName.find(aName);
        if (mObjects_ByName.end() == lIt)
        {
            return NULL;
        }

        assert(NULL != lIt->second);

        return lIt->second;
    }

    unsigned int ObjectList::Clean()
    {
        assert(NULL != mFile_PC6);

        unsigned int lResult = 0;

        for (auto lIt = mObjects_ByIndex.rbegin(); lIt != mObjects_ByIndex.rend(); lIt++)
        {
            auto lObject = lIt->second;
            assert(NULL != lObject);

            if (lObject->TestFlag(Object::FLAG_NOT_USED))
            {
                lResult++;

                mFile_PC6->RemoveLines(lObject->GetLineNo(), 1);
            }
        }

        return lResult;
    }

    unsigned int ObjectList::Parse(Text::File_UTF16* aFile_PC6, unsigned int aLineNo, unsigned int aFlags)
    {
        auto lLineCount = aFile_PC6->GetLineCount();
        auto lLineNo    = aLineNo;

        for (; lLineNo < lLineCount; lLineNo++)
        {
            auto lLine = aFile_PC6->GetLine(lLineNo);
            assert(NULL != lLine);

            if (L'~' == lLine[0])
            {
                SetFile(aFile_PC6, lLineNo);
                lLineNo++;
                break;
            }

            AddObject(lLine, lLineNo, aFlags);
        }

        Console::Stats(GetCount(), GetElementName());

        return lLineNo;
    }

    void ObjectList::Verify(const Text::File_UTF16& aFile_PC6)
    {
        assert(NULL != mElementName);

        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> lConverter;

        unsigned int lCount = 0;

        for (auto& lVT : mObjects_ByIndex)
        {
            auto lObj = lVT.second;
            assert(NULL != lObj);

            auto lIndex  = lObj->GetIndex();
            auto lLineNo = lObj->GetLineNo();
            auto lName   = lObj->GetName();
            assert(NULL != lName);

            switch (aFile_PC6.CountOccurrence(lConverter.from_bytes(lName).c_str()))
            {
            case 0: assert(false);

            case 1:
                lCount++;
                lObj->AddFlags(Object::FLAG_NOT_USED);

                Console::Warning_Begin(lLineNo);
                std::cout << "The " << mElementName << " named \"" << lName << "\" (" << lIndex << ") is useless";
                Console::Warning_End();
                break;

            case 2:
                if (lObj->TestFlag(Object::FLAG_SINGLE_USE_WARNING))
                {
                    Console::Warning_Begin(lLineNo);
                    std::cout << "The " << mElementName << " named \"" << lName << "\" (" << lIndex << ") is used only once";
                    Console::Warning_End();
                }
                else if (lObj->TestFlag(Object::FLAG_SINGLE_USE_INFO))
                {
                    Console::Info_Begin(lLineNo);
                    std::cout << "The " << mElementName << " named \"" << lName << "\" (" << lIndex << ") is used only once";
                    Console::Info_End();
                }
                break;
            }
        }

        if (0 < lCount)
        {
            Console::Warning_Begin();
            std::cout << lCount << " " << mElementName << "s not used";
            Console::Warning_End();
        }
    }

    // Protected
    // //////////////////////////////////////////////////////////////////////

    const char* ObjectList::GetElementName() const { return mElementName; }

    Text::File_UTF16* ObjectList::GetFile_PC6() { return mFile_PC6; }

    void ObjectList::AddObject(Object* aObject)
    {
        assert(NULL != aObject);

        char lMsg[64 + NAME_LENGTH];

        auto lBI = mObjects_ByIndex.insert(ObjectList::ByIndex::value_type(aObject->GetIndex(), aObject));

        sprintf_s(lMsg, "An object with index %u already exist", aObject->GetIndex());
        KMS_EXCEPTION_ASSERT(lBI.second, APPLICATION_ERROR, lMsg, "");

        auto lBN = mObjects_ByName.insert(ObjectList::ByName::value_type(aObject->GetName(), aObject));

        sprintf_s(lMsg, "An object named \"%s\" already exist", aObject->GetName());
        KMS_EXCEPTION_ASSERT(lBN.second, APPLICATION_ERROR, lMsg, "");
    }

    void ObjectList::AddObject(const wchar_t* aLine, unsigned int aLineNo, unsigned int aFlags)
    {
        assert(NULL != aLine);

        unsigned int lIndex;
        char         lName[NAME_LENGTH];

        auto lRet = swscanf_s(aLine, L"%u,%S", &lIndex, lName SizeInfo(lName));

        char lMsg[64];
        sprintf_s(lMsg, "Line %u  Invalid bit line", aLineNo);
        KMS_EXCEPTION_ASSERT(2 == lRet, APPLICATION_ERROR, lMsg, lRet);

        auto lObject = new Object(lName, lIndex, aLineNo, aFlags);

        AddObject(lObject);
    }

    void ObjectList::FindIndexAndLineNo(unsigned int* aIndex, unsigned int* aLineNo)
    {
        assert(NULL != aIndex);
        assert(NULL != aLineNo);
        
        char lMsg[64 + NAME_LENGTH];
        sprintf_s(lMsg, "Too many %s", mElementName);
        KMS_EXCEPTION_ASSERT(mMaxQty > mObjects_ByIndex.size(), APPLICATION_ERROR, lMsg, "");

        auto lIndex  = mMaxQty - 1;
        auto lLineNo = mLineNo_End;

        for (auto lIt = mObjects_ByIndex.rbegin(); lIt != mObjects_ByIndex.rend(); lIt++)
        {
            if (lIndex > lIt->first)
            {
                break;
            }

            lIndex  = lIt->first - 1;
            lLineNo = lIt->second->GetLineNo();
        }

        *aIndex = lIndex;
        *aLineNo = lLineNo;
    }

    unsigned int ObjectList::FindLineNo(unsigned int aIndex)
    {
        auto lResult = mLineNo_End;

        for (auto lIt = mObjects_ByIndex.rbegin(); lIt != mObjects_ByIndex.rend(); lIt++)
        {
            if (aIndex > lIt->first)
            {
                break;
            }

            lResult = lIt->second->GetLineNo();
        }

        return lResult;
    }

    void ObjectList::Replace(Object* aOld, Object* aNew)
    {
        auto lByIndex = mObjects_ByIndex.find(aOld->GetIndex());
        auto lByName  = mObjects_ByName .find(aOld->GetName ());

        lByIndex->second = aNew;
        lByName ->second = aNew;

        delete aOld;
    }

}
