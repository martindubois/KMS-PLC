
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI_ObjectList.cpp

// TEST COVERAGE  2023-08-15  KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== C++ ================================================================
#include <codecvt>

// ===== Local ==============================================================
#include "../Common/TRiLOGI/ObjectList.h"

#include "TRiLOGI/Object.h"
#include "TRiLOGI/PC6.h"

using namespace KMS;

// Constants
// //////////////////////////////////////////////////////////////////////////

#define INDEX_NEW_DEFAULT_NEW (0)

namespace TRiLOGI
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    ObjectList::~ObjectList() { ClearList(); }

    void ObjectList::AddToFile(Text::File_UTF16* aFile_PC6)
    {
        assert(nullptr != aFile_PC6);

        for (const auto& lEntry : mObjects_ByIndex)
        {
            assert(nullptr != lEntry.second);

            wchar_t lLine[LINE_LENGTH];

            lEntry.second->GetLine(lLine, sizeof(lLine));

            aFile_PC6->AddLine(lLine);
        }
    }

    void ObjectList::ClearList()
    {
        for (const auto& lVT : mObjects_ByIndex)
        {
            assert(nullptr != lVT.second);

            delete lVT.second;
        }

        mObjects_ByIndex.clear();
        mObjects_ByName .clear();
    }

    unsigned int ObjectList::GetCount() const { return static_cast<unsigned int>(mObjects_ByIndex.size()); }

    const Object* ObjectList::FindObject_ByIndex(unsigned int aIndex) const
    {
        auto lIt = mObjects_ByIndex.find(aIndex);
        if (mObjects_ByIndex.end() == lIt)
        {
            return nullptr;
        }

        // NOT TESTED
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
        assert(nullptr != aName);

        auto lIt = mObjects_ByName.find(aName);
        if (mObjects_ByName.end() == lIt)
        {
            return nullptr;
        }

        assert(nullptr != lIt->second);

        return lIt->second;
    }

    void ObjectList::SetProjectType(ProjectType aPT) { mProjectType = aPT; }

    unsigned int ObjectList::Clean()
    {
        unsigned int lResult = 0;

        for (auto lIt = mObjects_ByIndex.begin(); lIt != mObjects_ByIndex.end(); )
        {
            auto lObject = lIt->second;
            assert(nullptr != lObject);

            if (lObject->TestFlag(Object::FLAG_NOT_USED))
            {
                lResult++;

                auto lRet = mObjects_ByName.erase(lObject->GetName());
                assert(1 == lRet);

                lIt = mObjects_ByIndex.erase(lIt);
            }
            else
            {
                lIt++;
            }
        }

        return lResult;
    }

    unsigned int ObjectList::Parse(Text::File_UTF16* aFile_PC6, unsigned int aLineNo, unsigned int aFlags)
    {
        assert(nullptr != aFile_PC6);

        auto lLineCount = aFile_PC6->GetLineCount();
        auto lLineNo    = aLineNo;

        for (; lLineNo < lLineCount; lLineNo++)
        {
            auto lLine = aFile_PC6->GetLine(lLineNo);
            assert(nullptr != lLine);

            if (PC6_SECTION_END_C == lLine[0])
            {
                lLineNo++;
                break;
            }

            AddObject(lLine, aFlags);
        }

        ::Console::Stats(GetCount(), GetElementName());

        return lLineNo;
    }

    void ObjectList::Verify(const Text::File_UTF16& aFile_PC6, const AddressList* aPublicAddresses)
    {
        assert(nullptr != mElementName);

        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> lConverter;

        unsigned int lCount = 0;

        for (auto& lVT : mObjects_ByIndex)
        {
            auto lObj = lVT.second;
            assert(nullptr != lObj);

            auto lName = lObj->GetName();
            assert(nullptr != lName);

            if ((nullptr == aPublicAddresses) || (!aPublicAddresses->DoesContain(lName)))
            {
                auto lIndex = lObj->GetIndex();

                switch (aFile_PC6.CountOccurrence(lConverter.from_bytes(lName).c_str()))
                {
                case 0: assert(false);

                case 1:
                    lCount++;
                    lObj->AddFlags(Object::FLAG_NOT_USED);

                    ::Console::Warning_Begin()
                        << "The " << mElementName << " named \"" << lName << "\" (" << lIndex << ") is useless";
                    ::Console::Warning_End();
                    break;

                case 2: // NOT TESTED
                    if (lObj->TestFlag(Object::FLAG_SINGLE_USE_WARNING))
                    {
                        ::Console::Warning_Begin()
                            << "The " << mElementName << " named \"" << lName << "\" (" << lIndex << ") is used only once";
                        ::Console::Warning_End();
                    }
                    else if (lObj->TestFlag(Object::FLAG_SINGLE_USE_INFO))
                    {
                        ::Console::Info_Begin()
                            << "The " << mElementName << " named \"" << lName << "\" (" << lIndex << ") is used only once";
                        ::Console::Info_End();
                    }
                    break;
                }
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
        : mElementName(aElementName)
        , mMaxQty(aMaxQty)
        , mProjectType(ProjectType::LEGACY)
    {
        assert(nullptr != aElementName);
        assert(0 < aMaxQty);
    }

    const char* ObjectList::GetElementName() const { return mElementName; }

    bool ObjectList::IsProjectLegacy() const { return ProjectType::LEGACY == mProjectType; }

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

    void ObjectList::AddObject(const wchar_t* aLine, unsigned int aFlags)
    {
        assert(nullptr != aLine);

        unsigned int lIndex;
        char         lName[NAME_LENGTH];

        auto lRet = swscanf_s(aLine, L"%u,%S", &lIndex, lName SizeInfo(lName));
        KMS_EXCEPTION_ASSERT(2 == lRet, APPLICATION_ERROR, "Invalid bit line (NOT TESTED)", lRet);

        auto lObject = new Object(lName, lIndex, aFlags);

        AddObject(lObject);
    }

    unsigned int ObjectList::FindFreeIndex()
    {
        if (mMaxQty <= mObjects_ByIndex.size())
        {
            char lMsg[64 + NAME_LENGTH];
            sprintf_s(lMsg, "Too many %s (NOT TESTED)", mElementName);
            KMS_EXCEPTION(APPLICATION_ERROR, lMsg, "");
        }

        unsigned int lResult;

        switch (mProjectType)
        {
        case ProjectType::LEGACY:
            lResult = mMaxQty - 1;

            for (auto lIt = mObjects_ByIndex.rbegin(); lIt != mObjects_ByIndex.rend(); lIt++)
            {
                if (lResult > lIt->first)
                {
                    break; // NOT TESTED
                }

                if (lResult == lIt->first)
                {
                    lResult = lIt->first - 1;
                }
            }
            break;

        case ProjectType::NEW:
            lResult = 0;

            for (auto lIt = mObjects_ByIndex.begin(); lIt != mObjects_ByIndex.end(); lIt++)
            {
                if (lResult < lIt->first)
                {
                    break;
                }

                if (lResult == lIt->first)
                {
                    lResult = lIt->first + 1;
                }
            }
            break;

        default: assert(false);
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
