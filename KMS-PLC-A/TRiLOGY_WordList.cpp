
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGY_WordList.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "../Common/TRiLOGY/Word.h"

#include "../Common/TRiLOGY/WordList.h"

// Constants
// /////////////////////////////////////////////////////////////////////////

#define OFFSET_MAX (3999)

namespace TRiLOGY
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

        KMS_EXCEPTION(APPLICATION_ERROR, "Too many word", aName);
    }

    void WordList::AddWord(Word* aWord)
    {
        assert(NULL != aWord);

        std::pair<ByName::iterator, bool> lBN = mWords_ByName.insert(ByName::value_type(aWord->GetName(), aWord));
        KMS_EXCEPTION_ASSERT(lBN.second, APPLICATION_ERROR, "A word with the same name already exist", aWord->GetName());

        std::pair<ByOffset::iterator, bool> lBO = mWords_ByOffset.insert(ByOffset::value_type(aWord->GetOffset(), aWord));
        KMS_EXCEPTION_ASSERT(lBO.second, APPLICATION_ERROR, "A word with the same offset already exist", aWord->GetOffset());
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
