
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI_WordList.cpp

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
        unsigned int lOffset = OFFSET_MAX;

        for (ByOffset::reverse_iterator lIt = mWords_ByOffset.rbegin(); lIt != mWords_ByOffset.rend(); lIt++)
        {
            if (lOffset > lIt->first)
            {
                Word* lWord = new Word(aName, aIndex, aLineNo, lOffset, aComment, aFlags);

                AddWord(lWord);

                return lWord;
            }

            lOffset = lIt->first - 1;
        }

        char lMsg[64];
        sprintf_s(lMsg, "Line %u  Too many word", aLineNo);
        KMS_EXCEPTION(APPLICATION_ERROR, lMsg, aName);
    }

    void WordList::AddWord(Word* aWord)
    {
        assert(NULL != aWord);

        char lMsg[64 + NAME_LENGTH];

        std::pair<ByName::iterator, bool> lBN = mWords_ByName.insert(ByName::value_type(aWord->GetName(), aWord));

        sprintf_s(lMsg, "A word named \"%s\" already exist", aWord->GetName());
        KMS_EXCEPTION_ASSERT(lBN.second, APPLICATION_ERROR, lMsg, "");

        std::pair<ByOffset::iterator, bool> lBO = mWords_ByOffset.insert(ByOffset::value_type(aWord->GetOffset(), aWord));

        sprintf_s(lMsg, "A word already exist at offset %u", aWord->GetOffset());
        KMS_EXCEPTION_ASSERT(lBO.second, APPLICATION_ERROR, lMsg, "");
    }

    void WordList::Clear() { mWords_ByName.clear(); mWords_ByOffset.clear(); }

    void WordList::GetAddresses(const std::regex& aRegEx, AddressList* aOut) const
    {
        assert(NULL != aOut);

        for (ByOffset::value_type lVT : mWords_ByOffset)
        {
            Word* lWord = lVT.second;
            assert(NULL != lWord);

            if ((!lWord->TestFlag(Object::FLAG_NOT_USED)) && std::regex_match(lWord->GetName(), aRegEx))
            {
                aOut->push_back(Address(lWord->GetName(), AddressType::MODBUS_RTU_4X, 1000 + lWord->GetOffset()));
            }
        }
    }

    unsigned int WordList::GetCount() const { return static_cast<unsigned int>(mWords_ByName.size()); }

    const Word* WordList::Find_ByOffset(unsigned int aOffset) const
    {
        ByOffset::const_iterator lIt = mWords_ByOffset.find(aOffset);
        if (mWords_ByOffset.end() == lIt)
        {
            return NULL;
        }

        assert(NULL != lIt->second);

        return lIt->second;
    }

}
