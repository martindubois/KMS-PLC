
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI_WordList.cpp

// TEST COVERAGE  2023-08-14  KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== Import =============================================================
#include <KMS/Cfg/MetaData.h>

// ===== Local ==============================================================
#include "../Common/TRiLOGI/WordList.h"

#include "TRiLOGI/Word.h"

using namespace KMS;

KMS_RESULT_STATIC(RESULT_ALREADY_EXIST);
KMS_RESULT_STATIC(RESULT_TOO_MANY);

// Constants
// /////////////////////////////////////////////////////////////////////////

#define OFFSET_MIN (   1)
#define OFFSET_MAX (3999)

#define OFFSET_MAX_DEFAULT        OFFSET_MAX
#define OFFSET_NEW_DEFAULT_LEGACY (999)
#define OFFSET_NEW_DEFAULT_NEW    (  2)

static const Cfg::MetaData MD_OFFSET_MAX("OffsetMax = {Max}");
static const Cfg::MetaData MD_OFFSET_NEW("OffsetNew = {New}");

namespace TRiLOGI
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    WordList::WordList()
        : mProjectType(ProjectType::LEGACY)
        // ===== Configurable attributes ====================================
        , mOffsetMax(OFFSET_MAX_DEFAULT)
        , mOffsetNew(OFFSET_NEW_DEFAULT_LEGACY)
    {
        AddEntry("OffsetMax", &mOffsetMax, false, &MD_OFFSET_MAX);
        AddEntry("OffsetNew", &mOffsetNew, false, &MD_OFFSET_NEW);
    }

    Object* WordList::AddWord(const char* aName, unsigned int aIndex, const char* aComment)
    {
        unsigned int lOffset;

        switch (mProjectType)
        {
        case ProjectType::LEGACY: lOffset = FindFreeOffset_Dec(); break;
        case ProjectType::NEW   : lOffset = FindFreeOffset_Inc(); break;

        default: assert(false);
        }

        auto lResult = new Word(aName, aIndex, lOffset, aComment);

        AddWord(lResult);

        return lResult;
    }

    // This method only throw exception as a result of a corrupted PC6 file
    void WordList::AddWord(Word* aWord)
    {
        assert(nullptr != aWord);

        char lMsg[64 + NAME_LENGTH];

        auto lN = aWord->GetName  ();
        auto lO = aWord->GetOffset();

        if (OFFSET_MIN > lO)
        {
            sprintf_s(lMsg, "The offset of the word named \"%s\" is below the minimum (NOT TESTED)", lN);
            KMS_EXCEPTION(RESULT_INVALID_VALUE, lMsg, lO);
        }

        if (OFFSET_MAX < lO)
        {
            sprintf_s(lMsg, "The offset of the word named \"%s\" is above the absolute maximum (NOT TESTED)", lN);
            KMS_EXCEPTION(RESULT_INVALID_VALUE, lMsg, lO);
        }

        if (mOffsetMax < lO)
        {
            sprintf_s(lMsg, "The offset of the word named \"%s\" is above the configured maximum (NOT TESTED)", lN);
            KMS_EXCEPTION(RESULT_INVALID_VALUE, lMsg, lO);
        }

        auto lBN = mWords_ByName.insert(ByName::value_type(lN, aWord));
        if (!lBN.second)
        {
            sprintf_s(lMsg, "A word named \"%s\" already exist (NOT TESTED)", lN);
            KMS_EXCEPTION(RESULT_ALREADY_EXIST, lMsg, lO);
        }

        auto lBO = mWords_ByOffset.insert(ByOffset::value_type(lO, aWord));
        if (!lBO.second)
        {
            sprintf_s(lMsg, "A word already exist at offset %u (NOT TESTED)", lO);
            KMS_EXCEPTION(RESULT_ALREADY_EXIST, lMsg, lN);
        }
    }

    void WordList::ClearList() { mWords_ByName.clear(); mWords_ByOffset.clear(); }

    void WordList::GetAddresses(const std::regex& aRegEx, AddressList* aOut) const
    {
        assert(nullptr != aOut);

        for (auto lVT : mWords_ByOffset)
        {
            auto lWord = lVT.second;
            assert(nullptr != lWord);

            auto lN = lWord->GetName();

            if ((!lWord->TestFlag(TRiLOGI::Object::FLAG_NOT_USED)) && std::regex_match(lN, aRegEx))
            {
                aOut->Add(lN, AddressType::MODBUS_RTU_4X, 1000 + lWord->GetOffset());
            }
        }
    }

    unsigned int WordList::GetCount() const { return static_cast<unsigned int>(mWords_ByName.size()); }

    const Word* WordList::Find_ByOffset(unsigned int aOffset) const
    {
        auto lIt = mWords_ByOffset.find(aOffset);
        if (mWords_ByOffset.end() == lIt)
        {
            return nullptr;
        }

        assert(nullptr != lIt->second);

        return lIt->second;
    }

    void WordList::SetProjectType(ProjectType aPT)
    {
        assert(ProjectType::QTY > aPT);

        mProjectType = aPT;

        switch (mProjectType)
        {
        case ProjectType::LEGACY: break; // NOT TESTED

        case ProjectType::NEW:
            mOffsetNew = OFFSET_NEW_DEFAULT_NEW;
            break;

        default: assert(false);
        }
    }

    // ===== DI::Container ==================================================

    void WordList::Validate() const
    {
        DI::Dictionary::Validate();

        // mOffsetMax
        KMS_EXCEPTION_ASSERT(OFFSET_MIN <  mOffsetMax, RESULT_INVALID_CONFIG, "Words.OffsetMax is below the minimum", "");
        KMS_EXCEPTION_ASSERT(OFFSET_MAX >= mOffsetMax, RESULT_INVALID_CONFIG, "Words.OffsetMax it above the absolute maximum", "");

        // mOffsetNew
        switch (mProjectType)
        {
        case ProjectType::LEGACY:
            KMS_EXCEPTION_ASSERT(OFFSET_MAX >= mOffsetNew, RESULT_INVALID_CONFIG, "Words.OffsetNew it above the absolute maximum", "");
            KMS_EXCEPTION_ASSERT(OFFSET_MIN <  mOffsetNew, RESULT_INVALID_CONFIG, "Words.OffsetNew is below the minimum", "");
            KMS_EXCEPTION_ASSERT(mOffsetMax >= mOffsetNew, RESULT_INVALID_CONFIG, "Words.OffsetNew is above the condfigured maximum", "");
            break;
        case ProjectType::NEW:
            KMS_EXCEPTION_ASSERT(OFFSET_MAX >  mOffsetNew, RESULT_INVALID_CONFIG, "Words.OffsetNew it above the absolute maximum", "");
            KMS_EXCEPTION_ASSERT(OFFSET_MIN <= mOffsetNew, RESULT_INVALID_CONFIG, "Words.OffsetNew is below the minimum", "");
            KMS_EXCEPTION_ASSERT(mOffsetMax >  mOffsetNew, RESULT_INVALID_CONFIG, "Words.OffsetNew is above the configured maximum", "");
            break;

        default: assert(false);
        }
    }

    // Private
    // //////////////////////////////////////////////////////////////////////

    unsigned int WordList::FindFreeOffset_Dec()
    {
        assert(mOffsetMax >= mOffsetNew);

        unsigned int lResult = mOffsetNew;

        for (auto lIt = mWords_ByOffset.rbegin(); lIt != mWords_ByOffset.rend(); lIt++)
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

        KMS_EXCEPTION_ASSERT(OFFSET_MIN <= lResult, RESULT_TOO_MANY, "Too many word (NOT TESTED)", "");

        return lResult;
    }

    unsigned int WordList::FindFreeOffset_Inc()
    {
        assert(mOffsetMax > mOffsetNew);

        unsigned int lResult = mOffsetNew;

        for (auto lIt = mWords_ByOffset.begin(); lIt != mWords_ByOffset.end(); lIt++)
        {
            if (lResult < lIt->first)
            {
                break;
            }

            if (lResult == lIt->first)
            {
                lResult = lIt->first + 1; // NOT TESTED
            }
        }

        KMS_EXCEPTION_ASSERT(mOffsetMax >= lResult, RESULT_TOO_MANY, "Too many word (NOT TESTED)", "");

        return lResult;
    }

}
