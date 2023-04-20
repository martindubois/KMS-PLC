
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/EBPro/Label.h

#pragma once

// ===== C++ ================================================================
#include <string>
#include <vector>

// ===== Import/Includes ====================================================
#include <KMS/DI/Array.h>

namespace EBPro
{

    class LabelState;

    class Label
    {

    public:

        typedef std::vector<LabelState*> StateList;

        Label(const wchar_t* aName = L"");

        ~Label();

        void Export(FILE* aFile, const KMS::DI::Array& aLanguages) const;

        void Read(FILE* aFile);

        void Verify() const;

        void Write(FILE* aFile) const;

        LabelState* FindOrCreate(unsigned int aState, bool* aChanged);

        std::wstring mName;

        StateList mStates;

    };

}
