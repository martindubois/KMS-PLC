
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/EBPro_LabelState.cpp

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KMS/DI/String.h>

// ===== Local ==============================================================
#include "Console.h"
#include "Convert.h"

#include "EBPro/LabelState.h"

using namespace KMS;

namespace EBPro
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    bool LabelState::Set(unsigned int aLanguage, const wchar_t* aText)
    {
        assert(NULL != aText);

        bool lResult = false;

        if (mStrings.size() > aLanguage)
        {
            if (mStrings[aLanguage] != aText)
            {
                ::Console::Change("Label changed", aText);

                lResult = true;

                mStrings[aLanguage] = aText;
            }
        }
        else
        {
            ::Console::Change("Label new language", aText);

            lResult = true;

            while (mStrings.size() < aLanguage)
            {
                mStrings.push_back(L"");
            }

            mStrings.push_back(aText);
        }

        return lResult;
    }

    void LabelState::Export(FILE* aFile, const DI::Array& aLanguages) const
    {
        assert(NULL != aFile);

        fwprintf(aFile, L"    STATE\n");

        unsigned int lIndex = 0;

        for (auto& lStr : mStrings)
        {
            if (0 < lStr.size())
            {
                const DI::Object* lObj = aLanguages.GetEntry_R(lIndex);
                if (NULL != lObj)
                {
                    const DI::String* lLanguage = dynamic_cast<const DI::String*>(lObj);
                    assert(NULL != lLanguage);

                    wchar_t lExportable[2048];

                    ToExportableString(lStr.c_str(), lExportable);

                    fwprintf(aFile, L"        %S  %s\n", lLanguage->Get(), lExportable);
                }
            }

            lIndex++;
        }

    }

}
