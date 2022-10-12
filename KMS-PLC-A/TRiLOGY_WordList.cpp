
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGY_WordList.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "../Common/TRiLOGY/Word.h"

#include "../Common/TRiLOGY/WordList.h"

namespace TRiLOGY
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    WordList::WordList() {}

    Object* WordList::AddWord(const char* aName, unsigned int aIndex, unsigned int aLineNo, unsigned int aOffset)
    {
        assert(NULL != aName);

        Word* lResult = new Word(aName, aIndex, aLineNo, aOffset);

        std::pair<ByName::iterator, bool> lBN = mWords_ByName.insert(ByName::value_type(aName, lResult));
        KMS_EXCEPTION_ASSERT(lBN.second, APPLICATION, "A word with the same name already exist", aName);

        std::pair<ByOffset::iterator, bool> lBO = mWords_ByOffset.insert(ByOffset::value_type(aOffset, lResult));
        KMS_EXCEPTION_ASSERT(lBO.second, APPLICATION, "A word with the same offset already exist", aOffset);

        return lResult;
    }

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
