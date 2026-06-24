
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

        Element(const std::smatch& aMatch);

        uint16_t GetIndex() const;

        const char* GetName() const;

        void GetName(wchar_t* aOut, unsigned int aOutSize_byte) const;

        void SetName(const std::string& aName);

    private:

        uint16_t    mIndex;
        std::string mName;

    };

    typedef std::map<unsigned int, Element> Element_List;

    // Public
    // //////////////////////////////////////////////////////////////////////

    inline uint16_t Element::GetIndex() const { return mIndex; }

    inline const char* Element::GetName() const { return mName.c_str(); }

    inline void Element::SetName(const std::string& aName) { mName = aName; }

}
