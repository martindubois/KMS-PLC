
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/EBPro/LabelList.h
// Status    DEV

#pragma once

// ===== C++ ================================================================
#include <vector>

// ===== Local ==============================================================
#include "List.h"

namespace EBPro
{

    class Label;

    class LabelList : public List
    {

    private:

        KMS::DI::Array mLanguages;

    public:

        LabelList(Software* aSoftware);

        void Export() const;

        void Parse();

        void ValidateConfig() const;

        void Verify() const;

        // ===== List =======================================================
        virtual ~LabelList();
        virtual void Read();

    // Internal

        typedef std::vector<Label*> Internal;

        Internal mLabels;

    protected:

        // ===== List =======================================================
        virtual bool ImportSource(const char* aFileName);
        virtual void SaveToImport();

    private:

        NO_COPY(LabelList);

        Label* Create(const wchar_t* aName);

        Label* Find(const wchar_t* aName);

        Label* FindOrCreate(const char* aName, bool* aChanged);

        unsigned int FindLanguageIndex(const char* aId) const;

        uint8_t mHeader[18];

    };

}
