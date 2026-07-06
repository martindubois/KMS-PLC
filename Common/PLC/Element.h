
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/PLC/Define.h

#pragma once

// ===== C++ ================================================================
#include <map>
#include <regex>
#include <string>

// ===== Import =============================================================
#include <KMS/Convert.h>

namespace PLC
{

    class Element
    {

    public:

        static const uint16_t INVALID_INDEX;

        Element();

        uint16_t GetIndex() const;

        const char* GetName() const;

        void GetName(wchar_t* aOut, unsigned int aOutSize_byte) const;

        bool IsNameEmpty() const;

        void SetIndex(uint16_t aIndex);

        void SetIndexAndName(const std::smatch& aMatch);

        void SetName(const std::smatch& aMatch);

        void SetName(const std::string& aName);

    private:

        uint16_t    mIndex;
        std::string mName;

    };

    typedef std::map<unsigned int, Element> Element_Map;

    // Public
    // //////////////////////////////////////////////////////////////////////

    inline uint16_t Element::GetIndex() const { return mIndex; }

    inline const char* Element::GetName() const { return mName.c_str(); }

    inline bool Element::IsNameEmpty() const { return mName.empty(); }

    inline void Element::SetName(const std::smatch& aMatch) { mName = aMatch[1].str(); }

    inline void Element::SetName(const std::string& aName) { mName = aName; }

}
