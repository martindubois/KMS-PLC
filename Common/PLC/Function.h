
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/PLC/Function.h

#pragma once

// ===== C++ ================================================================
#include <list>

// ===== Local ==============================================================
#include "../Common/PLC/Element.h"

namespace PLC
{

    class Function final : public Element
    {
    
    public:

        Function();

        size_t GetSize() const;

        void AddLine(const std::string& aLine);

        std::list<std::string> mLines;

    };

    typedef std::list<Function*>              Function_List;
    typedef std::map<unsigned int, Function*> Function_Map;

    // Public
    // //////////////////////////////////////////////////////////////////////

    inline Function::Function() {}

    inline size_t Function::GetSize() const
    {
        size_t lResult_byte = 0;

        for (auto& lL : mLines)
        {
            lResult_byte += lL.size() - sizeof(wchar_t);
        }

        return lResult_byte;
    }

    inline void Function::AddLine(const std::string& aLine) { mLines.push_back(aLine); }

}
