
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/EBPro/LabelList.h

#pragma once

// ===== C++ ================================================================
#include <vector>

// ===== Import =============================================================
#include <KMS/DI/Array.h>
#include <KMS/DI/Dictionary.h>
#include <KMS/DI/String_Expand.h>

namespace EBPro
{

    class Label;

    class LabelList : public KMS::DI::Dictionary
    {

    public:

        LabelList();

        ~LabelList();

        void Export() const;

        void Import();

        void Parse();

        void Read();

        void ValidateConfig() const;

        void Verify() const;

        void Write() const;

    // Internal

        typedef std::vector<Label*> Internal;

        Internal mLabels;

    private:

        LabelList(const LabelList&);

        const LabelList& operator = (const LabelList&);

        Label* Create(const wchar_t* aName);

        Label* Find(const wchar_t* aName);

        Label* FindOrCreate(const char* aName, bool* aChanged);

        unsigned int FindLanguageIndex(const char* aId) const;

        void Save_ToImport_LBL() const;

        uint8_t mHeader[18];

        // ===== Configurable attributes ====================================

        KMS::DI::String_Expand mExported_LBL;
        KMS::DI::String_Expand mToImport_LBL;

        KMS::DI::Array mLanguages;
        KMS::DI::Array mSources;

    };

}
