
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// Product   KMS-PLC
// File      Common/EBPro/Config.h

#pragma once

// ===== C++ ================================================================
#include <map>
#include <string>

namespace EBPro
{

    class Config
    {

    public:

        constexpr static unsigned int LANGUAGE_QTY = 8;

        Config();

        unsigned int GetLanguageIndex(const char* aName) const;

        const char* GetLanguageName(unsigned int aIndex) const;

    private:

        void SetLanguage(unsigned int aIndex, const char* aName);

        typedef std::map<std::string, unsigned int> LanguageMap;

        LanguageMap mLanguages_ByName;

        char mLanguages_ByIndex[LANGUAGE_QTY][4];

    };

}
