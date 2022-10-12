
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/EBPro/LabelState.h

#pragma once

// ===== C++ ================================================================
#include <string>
#include <vector>

// ===== Import/Includes ====================================================
#include <KMS/DI/Array.h>

namespace EBPro
{

    class LabelState
    {

    public:

        typedef std::vector<std::wstring> StringList;

        bool Set(unsigned int aLanguage, const wchar_t* aText);

        void Export(FILE* aFile, const KMS::DI::Array& aLanguages) const;

        StringList mStrings;

    };

}
