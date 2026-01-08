
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// Product   KMS-PLC
// File      EBPro/Config.cpp

#include <KMS/Base.h>

// ===== Local ==============================================================
#include "../Common/Parser.h"

#include "../Common/EBPro/Config.h"

// Configurations
// //////////////////////////////////////////////////////////////////////////

static const char* CONFIG_TXT = "EBPro_Config.txt";

namespace EBPro
{

    Config::Config()
    {
        Parser lParser(CONFIG_TXT);

        char lLine[LINE_LENGTH];

        while (lParser.GetNextLine(lLine, sizeof(lLine)))
        {
            unsigned int lIndex;
            char lName[NAME_LENGTH];

            if (2 == sscanf_s(lLine, "Language %u %[a-z]", &lIndex, lName SizeInfo(lName)))
            {
                SetLanguage(lIndex, lName);
            }
        }
    }

    const char* Config::GetLanguageName(unsigned int aIndex) const
    {
        assert(LANGUAGE_QTY > aIndex);

        return mLanguages_ByIndex[aIndex];
    }

    // Private
    // //////////////////////////////////////////////////////////////////////

    void Config::SetLanguage(unsigned int aIndex, const char* aName)
    {
        assert(LANGUAGE_QTY > aIndex);
        assert(nullptr != aName);

        mLanguages_ByName.insert(LanguageMap::value_type(aName, aIndex));

        strcpy_s(mLanguages_ByIndex[aIndex] SizeInfo(mLanguages_ByIndex[aIndex]), aName);
    }

}
