
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGY_DefineList.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "../Common/TRiLOGY/Object.h"

#include "../Common/TRiLOGY/DefineList.h"

namespace TRiLOGY
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    DefineList::DefineList() : ObjectList("define", L"~END_DEFINES~\r") {}

    unsigned int DefineList::Parse(const KMS::Text::File_UTF16& aFile_PC6, unsigned int aLineNo)
    {
        unsigned int lResult = ObjectList::Parse(aFile_PC6, aLineNo);

        std::cout << "    " << mConstants.GetCount() << " constants" << std::endl;
        std::cout << "    " << mWords.GetCount() << " words" << std::endl;

        return lResult;
    }

    void DefineList::Verify(const KMS::Text::File_UTF16& aFile_PC6)
    {
        ObjectList::Verify(aFile_PC6);

        mConstants.Verify();
    }

    // Protected
    // //////////////////////////////////////////////////////////////////////

    void DefineList::AddObject(const wchar_t* aLine, unsigned int aLineNo)
    {
        assert(NULL != aLine);

        unsigned int lIndex;
        char         lText[LINE_LENGTH];

        int lRet = swscanf_s(aLine, L"%u,%S", &lIndex, lText SizeInfo(lText));
        KMS_EXCEPTION_ASSERT(2 == lRet, APPLICATION, "Invalid define line", lRet);

        Object     * lDefine;
        char         lName[NAME_LENGTH];
        unsigned int lOffset;
        char         lValue[NAME_LENGTH];

        memset(&lValue, 0, sizeof(lValue));

        if (2 == sscanf_s(lText, "%[^,],DM[%u]", &lName SizeInfo(lName), &lOffset))
        {
            lDefine = mWords.AddWord(lName, lIndex, aLineNo, lOffset);
        }
        else if (1 <= sscanf_s(lText, "%[^,],%[^\n\r\t]", &lName SizeInfo(lName), lValue SizeInfo(lValue)))
        {
            lDefine = mConstants.AddConstant(lName, lIndex, aLineNo, lValue);
        }
        else
        {
            KMS_EXCEPTION(APPLICATION, "Invalid define line", "");
        }

        ObjectList::AddObject(lDefine);
    }

}
