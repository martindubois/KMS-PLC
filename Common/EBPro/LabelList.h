
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
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
    class Software;

    class LabelList : public KMS::DI::Dictionary
    {

    public:

        LabelList(Software* aSoftware);

        ~LabelList();

        void Export() const;

        void Import();

        void Parse();

        void Read();

        void ValidateConfig() const;

        void Verify() const;

    // Internal

        typedef std::vector<Label*> Internal;

        Internal mLabels;

    private:

        NO_COPY(LabelList);

        Label* Create(const wchar_t* aName);

        Label* Find(const wchar_t* aName);

        Label* FindOrCreate(const char* aName, bool* aChanged);

        unsigned int FindLanguageIndex(const char* aId) const;

        void Save_ToImport_LBL() const;

        uint8_t mHeader[18];

        Software* mSoftware;

        // ===== Configurable attributes ====================================

        KMS::DI::String_Expand mExported;
        KMS::DI::String_Expand mToImport;

        KMS::DI::Array mLanguages;
        KMS::DI::Array mSources;

    };

}
