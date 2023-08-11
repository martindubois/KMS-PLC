
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI_WordList.cpp

// TEST COVERAGE  2023-08-10  KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== Import =============================================================
#include <KMS/Cfg/MetaData.h>

// ===== Local ==============================================================
#include "../Common/TRiLOGI/WordList.h"

#include "TRiLOGI/Word.h"

using namespace KMS;

// Constants
// /////////////////////////////////////////////////////////////////////////

#define OFFSET_MIN (   1)
#define OFFSET_MAX (3999)

#define OFFSET_MAX_DEFAULT OFFSET_MAX
#define OFFSET_NEW_DEFAULT (999)

static const Cfg::MetaData MD_OFFSET_MAX("OffsetMax = {Max}");
static const Cfg::MetaData MD_OFFSET_NEW("OffsetNew = {New}");

namespace TRiLOGI
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    WordList::WordList()
        // ===== Configurable attributes ====================================
        : mOffsetMax(OFFSET_MAX_DEFAULT)
        , mOffsetNew(OFFSET_NEW_DEFAULT)
    {
        AddEntry("OffsetMax", &mOffsetMax, false, &MD_OFFSET_MAX);
        AddEntry("OffsetNew", &mOffsetNew, false, &MD_OFFSET_NEW);
    }

    Object* WordList::AddWord(const char* aName, unsigned int aIndex, unsigned int aLineNo, const char* aComment, unsigned int aFlags)
    {
        unsigned int lOffset = mOffsetNew;

        for (auto lIt = mWords_ByOffset.rbegin(); lIt != mWords_ByOffset.rend(); lIt++)
        {
            if (lOffset > lIt->first)
            {
                break;
            }

            if (lOffset == lIt->first)
            {
                lOffset = lIt->first - 1;
            }
        }

        if (OFFSET_MIN > lOffset)
        {
            char lMsg[64];
            sprintf_s(lMsg, "Line %u  Too many word (NOT TESTED)", aLineNo);
            KMS_EXCEPTION(APPLICATION_ERROR, lMsg, aName);
        }

        auto lResult = new Word(aName, aIndex, aLineNo, lOffset, aComment, aFlags);

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
            KMS_EXCEPTION(APPLICATION_ERROR, lMsg, lO);
        }

        if (OFFSET_MAX < lO)
        {
            sprintf_s(lMsg, "The offset of the word named \"%s\" is above the absolute maximum (NOT TESTED)", lN);
            KMS_EXCEPTION(APPLICATION_ERROR, lMsg, lO);
        }

        if (mOffsetMax < lO)
        {
            sprintf_s(lMsg, "The offset of the word named \"%s\" is above the configured maximum (NOT TESTED)", lN);
            KMS_EXCEPTION(APPLICATION_ERROR, lMsg, lO);
        }

        auto lBN = mWords_ByName.insert(ByName::value_type(lN, aWord));
        if (!lBN.second)
        {
            sprintf_s(lMsg, "A word named \"%s\" already exist (NOT TESTED)", lN);
            KMS_EXCEPTION(APPLICATION_ERROR, lMsg, lO);
        }

        auto lBO = mWords_ByOffset.insert(ByOffset::value_type(lO, aWord));
        if (!lBO.second)
        {
            sprintf_s(lMsg, "A word already exist at offset %u (NOT TESTED)", lO);
            KMS_EXCEPTION(APPLICATION_ERROR, lMsg, lN);
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

            if ((!lWord->TestFlag(TRiLOGI::Object::FLAG_NOT_USED)) && std::regex_match(lWord->GetName(), aRegEx))
            {
                aOut->push_back(Address(lWord->GetName(), AddressType::MODBUS_RTU_4X, 1000 + lWord->GetOffset()));
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

    void WordList::ValidateConfig() const
    {
        // mOffsetMax
        KMS_EXCEPTION_ASSERT(OFFSET_MIN <  mOffsetMax, APPLICATION_USER_ERROR, "Words.OffsetMax is below the minimum", "");
        KMS_EXCEPTION_ASSERT(OFFSET_MAX >= mOffsetMax, APPLICATION_USER_ERROR, "Words.OffsetMax it above the absolute maximum", "");

        // mOffsetNew
        KMS_EXCEPTION_ASSERT(OFFSET_MIN < mOffsetNew , APPLICATION_USER_ERROR, "Words.OffsetNew is below the minimum", "");
        KMS_EXCEPTION_ASSERT(OFFSET_MAX >= mOffsetMax, APPLICATION_USER_ERROR, "Words.OffsetNew it above the absolute maximum", "");
        KMS_EXCEPTION_ASSERT(mOffsetMax >= mOffsetNew, APPLICATION_USER_ERROR, "Words.OffsetNew is above the condfigured maximum", "");
    }

}
