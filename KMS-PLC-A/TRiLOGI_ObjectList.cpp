
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI_ObjectList.cpp

// TEST COVERAGE  2023-08-08  KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== C++ ================================================================
#include <codecvt>

// ===== Local ==============================================================
#include "../Common/TRiLOGI/ObjectList.h"

#include "TRiLOGI/Object.h"

using namespace KMS;

namespace TRiLOGI
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    ObjectList::~ObjectList() { ClearList(); }

    bool ObjectList::Apply()
    {
        assert(nullptr != mFile_PC6);

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

    void ObjectList::ClearList()
    {
        mFile_PC6 = nullptr;

        for (const auto& lVT : mObjects_ByIndex)
        {
            assert(nullptr != lVT.second);

            delete lVT.second;
        }

        mObjects_ByIndex.clear();
        mObjects_ByName .clear();
    }

    unsigned int ObjectList::GetCount() const { return static_cast<unsigned int>(mObjects_ByIndex.size()); }

    void ObjectList::SetFile(Text::File_UTF16* aFile_PC6, unsigned int aLineNo_End)
    {
        assert(nullptr != aFile_PC6);
        assert(0 < aLineNo_End);

        mFile_PC6   = aFile_PC6;
        mLineNo_End = aLineNo_End;
    }

    const Object* ObjectList::FindObject_ByIndex(unsigned int aIndex) const
    {
        auto lIt = mObjects_ByIndex.find(aIndex);
        if (mObjects_ByIndex.end() == lIt)
        {
            return nullptr;
        }

        assert(nullptr != lIt->second);

        return lIt->second;
    }

    Object* ObjectList::FindObject_ByIndex(unsigned int aIndex)
    {
        auto lIt = mObjects_ByIndex.find(aIndex);
        if (mObjects_ByIndex.end() == lIt)
        {
            return nullptr;
        }

        assert(nullptr != lIt->second);

        return lIt->second;
    }

    Object* ObjectList::FindObject_ByName(const char* aName)
    {
        auto lIt = mObjects_ByName.find(aName);
        if (mObjects_ByName.end() == lIt)
        {
            return nullptr;
        }

        assert(nullptr != lIt->second);

        return lIt->second;
    }

    unsigned int ObjectList::Clean()
    {
        assert(nullptr != mFile_PC6);

        unsigned int lResult = 0;

        for (auto lIt = mObjects_ByIndex.rbegin(); lIt != mObjects_ByIndex.rend(); lIt++)
        {
            auto lObject = lIt->second;
            assert(nullptr != lObject);

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
            assert(nullptr != lLine);

            if (L'~' == lLine[0])
            {
                SetFile(aFile_PC6, lLineNo);
                lLineNo++;
                break;
            }

            AddObject(lLine, lLineNo, aFlags);
        }

        ::Console::Stats(GetCount(), GetElementName());

        return lLineNo;
    }

    void ObjectList::Verify(const Text::File_UTF16& aFile_PC6)
    {
        assert(nullptr != mElementName);

        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> lConverter;

        unsigned int lCount = 0;

        for (auto& lVT : mObjects_ByIndex)
        {
            auto lObj = lVT.second;
            assert(nullptr != lObj);

            auto lIndex  = lObj->GetIndex();
            auto lLineNo = lObj->GetLineNo();
            auto lName   = lObj->GetName();
            assert(nullptr != lName);

            switch (aFile_PC6.CountOccurrence(lConverter.from_bytes(lName).c_str()))
            {
            case 0: assert(false);

            case 1:
                lCount++;
                lObj->AddFlags(Object::FLAG_NOT_USED);

                ::Console::Warning_Begin(lLineNo)
                    << "The " << mElementName << " named \"" << lName << "\" (" << lIndex << ") is useless";
                ::Console::Warning_End();
                break;

            case 2:
                // NOT TESTED
                if (lObj->TestFlag(Object::FLAG_SINGLE_USE_WARNING))
                {
                    ::Console::Warning_Begin(lLineNo)
                        << "The " << mElementName << " named \"" << lName << "\" (" << lIndex << ") is used only once";
                    ::Console::Warning_End();
                }
                else if (lObj->TestFlag(Object::FLAG_SINGLE_USE_INFO))
                {
                    ::Console::Info_Begin(lLineNo)
                        << "The " << mElementName << " named \"" << lName << "\" (" << lIndex << ") is used only once";
                    ::Console::Info_End();
                }
                break;
            }
        }

        if (0 < lCount)
        {
            ::Console::Warning_Begin()
                << lCount << " " << mElementName << "s not used";
            ::Console::Warning_End();
        }
    }

    // Protected
    // //////////////////////////////////////////////////////////////////////

    ObjectList::ObjectList(const char* aElementName, unsigned int aMaxQty)
        : mElementName(aElementName), mFile_PC6(nullptr), mLineNo_End(0), mMaxQty(aMaxQty)
    {
        assert(nullptr != aElementName);
        assert(0 < aMaxQty);
    }

    const char* ObjectList::GetElementName() const { return mElementName; }

    Text::File_UTF16* ObjectList::GetFile_PC6() { return mFile_PC6; }

    void ObjectList::AddObject(Object* aObject)
    {
        assert(nullptr != aObject);

        char lMsg[64 + NAME_LENGTH];

        auto lBI = mObjects_ByIndex.insert(ObjectList::ByIndex::value_type(aObject->GetIndex(), aObject));
        if (!lBI.second)
        {
            sprintf_s(lMsg, "An object with index %u already exist (NOT TESTED)", aObject->GetIndex());
            KMS_EXCEPTION(APPLICATION_ERROR, lMsg, "");
        }

        auto lBN = mObjects_ByName.insert(ObjectList::ByName::value_type(aObject->GetName(), aObject));
        if (!lBN.second)
        {
            sprintf_s(lMsg, "An object named \"%s\" already exist (NOT TESTED)", aObject->GetName());
            KMS_EXCEPTION(APPLICATION_ERROR, lMsg, "");
        }
    }

    void ObjectList::AddObject(const wchar_t* aLine, unsigned int aLineNo, unsigned int aFlags)
    {
        assert(nullptr != aLine);

        unsigned int lIndex;
        char         lName[NAME_LENGTH];

        auto lRet = swscanf_s(aLine, L"%u,%S", &lIndex, lName SizeInfo(lName));
        if (2 != lRet)
        {
            char lMsg[64];
            sprintf_s(lMsg, "Line %u  Invalid bit line (NOT TESTED)", aLineNo);
            KMS_EXCEPTION(APPLICATION_ERROR, lMsg, lRet);
        }

        auto lObject = new Object(lName, lIndex, aLineNo, aFlags);

        AddObject(lObject);
    }

    void ObjectList::FindIndexAndLineNo(unsigned int* aIndex, unsigned int* aLineNo)
    {
        assert(nullptr != aIndex);
        assert(nullptr != aLineNo);
        
        if (mMaxQty <= mObjects_ByIndex.size())
        {
            char lMsg[64 + NAME_LENGTH];
            sprintf_s(lMsg, "Too many %s (NOT TESTED)", mElementName);
            KMS_EXCEPTION(APPLICATION_ERROR, lMsg, "");
        }

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

            // NOT TESTED
            lResult = lIt->second->GetLineNo();
        }

        return lResult;
    }

    // NOT TESTED
    void ObjectList::Replace(Object* aOld, Object* aNew)
    {
        assert(nullptr != aOld);
        assert(nullptr != aNew);
        
        auto lByIndex = mObjects_ByIndex.find(aOld->GetIndex());
        auto lByName  = mObjects_ByName .find(aOld->GetName ());

        lByIndex->second = aNew;
        lByName ->second = aNew;

        delete aOld;
    }

}
