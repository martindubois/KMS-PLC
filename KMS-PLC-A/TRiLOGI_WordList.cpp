
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI_WordList.cpp

// TEST COVERAGE  2023-08-08  KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== Local ==============================================================
#include "../Common/TRiLOGI/WordList.h"

#include "TRiLOGI/Word.h"

// Constants
// /////////////////////////////////////////////////////////////////////////

#define OFFSET_MAX (3999)

namespace TRiLOGI
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    WordList::WordList() {}

    Object* WordList::AddWord(const char* aName, unsigned int aIndex, unsigned int aLineNo, const char* aComment, unsigned int aFlags)
    {
        if (OFFSET_MAX <= mWords_ByOffset.size())
        {
            char lMsg[64];
            sprintf_s(lMsg, "Line %u  Too many word (NOT TESTED)", aLineNo);
            KMS_EXCEPTION(APPLICATION_ERROR, lMsg, aName);
        }

        unsigned int lOffset = OFFSET_MAX;

        for (auto lIt = mWords_ByOffset.rbegin(); lIt != mWords_ByOffset.rend(); lIt++)
        {
            if (lOffset > lIt->first)
            {
                break;
            }

            lOffset = lIt->first - 1;
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

        auto lBN = mWords_ByName.insert(ByName::value_type(aWord->GetName(), aWord));
        if (!lBN.second)
        {
            sprintf_s(lMsg, "A word named \"%s\" already exist (NOT TESTED)", aWord->GetName());
            KMS_EXCEPTION(APPLICATION_ERROR, lMsg, "");
        }

        auto lBO = mWords_ByOffset.insert(ByOffset::value_type(aWord->GetOffset(), aWord));
        if (!lBO.second)
        {
            sprintf_s(lMsg, "A word already exist at offset %u (NOT TESTED)", aWord->GetOffset());
            KMS_EXCEPTION(APPLICATION_ERROR, lMsg, "");
        }
    }

    void WordList::Clear() { mWords_ByName.clear(); mWords_ByOffset.clear(); }

    void WordList::GetAddresses(const std::regex& aRegEx, AddressList* aOut) const
    {
        assert(nullptr != aOut);

        for (auto lVT : mWords_ByOffset)
        {
            auto lWord = lVT.second;
            assert(nullptr != lWord);

            if ((!lWord->TestFlag(Object::FLAG_NOT_USED)) && std::regex_match(lWord->GetName(), aRegEx))
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

}
