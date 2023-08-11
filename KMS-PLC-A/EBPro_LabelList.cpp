
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/EBPro_LabelList.cpp

#include "Component.h"

// ===== C++ ================================================================
#include <codecvt>

// ===== Import/Includes ====================================================
#include <KMS/Cfg/MetaData.h>
#include <KMS/File/Folder.h> 
#include <KMS/Text/File_ASCII.h>

// ===== Local ==============================================================
#include "../Common/EBPro/Software.h"

#include "../Common/EBPro/LabelList.h"

#include "Convert.h"

#include "EBPro/LabelState.h"
#include "EBPro/Label.h"

using namespace KMS;

// Constants
// //////////////////////////////////////////////////////////////////////////

static const Cfg::MetaData MD_LANGUAGES("Languages += {id}");

namespace EBPro
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    LabelList::LabelList(Software* aSoftware) : List(aSoftware)
    {
        assert(nullptr != aSoftware);

        mLanguages.SetCreator(DI::String::Create);

        AddEntry("Languages", &mLanguages, false , &MD_LANGUAGES);
    }

    void LabelList::Export() const
    {
        if (IsExportedConfigured())
        {
            auto lPath = std::string(GetExported()) + ".txt";

            ::Console::Progress_Begin("EBPro", "Exporting", lPath.c_str());
            {
                FILE* lFile;

                auto lErr = fopen_s(&lFile, lPath.c_str(), "w,ccs=UTF-8");
                if (0 != lErr)
                {
                    char lMsg[64 + PATH_LENGTH];
                    sprintf_s(lMsg, "Cannot open \"%s\" for writing", lPath.c_str());
                    KMS_EXCEPTION(APPLICATION_ERROR, lMsg, lErr);
                }

                assert(nullptr != lFile);

                for (auto lLabel : mLabels)
                {
                    lLabel->Export(lFile, mLanguages);
                }

                auto lRet = fclose(lFile);
                assert(0 == lRet);
            }
            ::Console::Progress_End("Exported");
        }
    }

    void LabelList::Parse()
    {
        if (IsExportedConfigured())
        {
            const char* lExported = GetExported();

            ::Console::Progress_Begin("EBPro", "Parsing", lExported);
            {
                FILE* lFile;

                auto lErr = fopen_s(&lFile, lExported, "rb");

                char lMsg[64 + PATH_LENGTH];
                sprintf_s(lMsg, "Cannot open \"%s\" for reading", lExported);
                KMS_EXCEPTION_ASSERT(0 == lErr, APPLICATION_ERROR, lMsg, lErr);

                assert(nullptr != lFile);

                uint16_t lLabelCount;

                auto lSize_byte = fread_s(&lLabelCount SizeInfo(lLabelCount) + 1, 1, sizeof(lLabelCount), lFile);
                KMS_EXCEPTION_ASSERT(sizeof(lLabelCount) == lSize_byte, APPLICATION_ERROR, "Cannot read the exported LBL file", lSize_byte);

                lSize_byte = fread_s(mHeader SizeInfo(mHeader) + 1, 1, sizeof(mHeader), lFile);
                KMS_EXCEPTION_ASSERT(sizeof(mHeader) == lSize_byte, APPLICATION_ERROR, "Cannot read the exported LBL file", lSize_byte);

                while (!feof(lFile))
                {
                    auto lLabel = new Label;

                    lLabel->Read(lFile);

                    mLabels.push_back(lLabel);
                }

                ::Console::Stats(mLabels.size(), "labels");

                KMS_EXCEPTION_ASSERT(lLabelCount == mLabels.size(), APPLICATION_ERROR, "Corrupted exported LBL file", lLabelCount);

                auto lRet = fclose(lFile);
                assert(0 == lRet);
            }
            ::Console::Progress_End("Parsed");
        }
    }

    void LabelList::Read() {}

    void LabelList::ValidateConfig() const
    {
        if (IsExportedConfigured())
        {
            KMS_EXCEPTION_ASSERT(0 < mLanguages.GetCount(), APPLICATION_USER_ERROR, "The configuration file must define language", "");

            for (auto& lEntry : mLanguages.mInternal)
            {
                auto lLanguage = dynamic_cast<const DI::String*>(lEntry.Get());
                assert(nullptr != lLanguage);

                if (2 != lLanguage->GetLength())
                {
                    char lMsg[64 + PATH_LENGTH];
                    sprintf_s(lMsg, "\"%s\" is not a valid language id", lLanguage->Get());
                    KMS_EXCEPTION(APPLICATION_USER_ERROR, lMsg, "");
                }
            }
        }

        List::ValidateConfig();
    }
    
    void LabelList::Verify() const
    {
        ::Console::Progress_Begin("EBPro", "Verifying labels");
        {
            for (Internal::const_iterator lItA = mLabels.begin(); lItA != mLabels.end(); lItA++)
            {
                for (Internal::const_iterator lItB = lItA + 1; lItB != mLabels.end(); lItB++)
                {
                    if ((*lItA)->mName == (*lItB)->mName)
                    {
                        ::Console::Warning_Begin()
                            << "The label " << (*lItA)->mName.c_str() << " is present twice";
                        ::Console::Warning_End();
                    }
                }

                (*lItA)->Verify();
            }

            // TODO Verify more
        }
        ::Console::Progress_End("Verified");
    }

    // ===== List =======================================================

    LabelList::~LabelList()
    {
        for (auto lLabel : mLabels)
        {
            assert(nullptr != lLabel);

            delete lLabel;
        }
    }

    // Protected
    // //////////////////////////////////////////////////////////////////

    // ===== List =======================================================

    bool LabelList::ImportSource(const char* aFileName)
    {
        Text::File_ASCII lFile;

        lFile.Read(File::Folder::CURRENT, aFileName);

        lFile.RemoveComments_Script();
        lFile.RemoveEmptyLines();

        Label      * lLabel = nullptr;
        char         lMsg[64];
        bool         lResult = false;
        LabelState * lState = nullptr;
        unsigned int lStateCount;

        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> lConverter;

        for (const auto& lLine : lFile.mLines)
        {
            char lLanguage[NAME_LENGTH];
            char lText[LINE_LENGTH];

            // TODO Support something else than 4 spaces.
            if (0 == strncmp("    STATE", lLine.c_str(), 9))
            {
                if (nullptr == lLabel)
                {
                    sprintf_s(lMsg, "Line %u  STATE outside of LABEL", lLine.GetUserLineNo());
                    KMS_EXCEPTION(APPLICATION_ERROR, lMsg, lLine.c_str());
                }

                lState = lLabel->FindOrCreate(lStateCount, &lResult);
                lStateCount++;
            }
            else if (1 == sscanf_s(lLine.c_str(), "LABEL %[^\n\r\t]", lText SizeInfo(lText)))
            {
                lLabel = FindOrCreate(lText, &lResult);
                lStateCount = 0;

                assert(nullptr != lLabel);
            }
            else if (2 == sscanf_s(lLine.c_str(), " %[a-z] %[^\n\r\t]", lLanguage SizeInfo(lLanguage), lText SizeInfo(lText)))
            {
                if (nullptr == lState)
                {
                    sprintf_s(lMsg, "Line %u  Text outside of STATE", lLine.GetUserLineNo());
                    KMS_EXCEPTION(APPLICATION_ERROR, lMsg, lLine.c_str());
                }

                std::wstring lString(lConverter.from_bytes(lText));

                wchar_t lImportable[LINE_LENGTH];

                ToImportableString(lString.c_str(), lImportable);

                lResult |= lState->Set(FindLanguageIndex(lLanguage), lImportable);
            }
        }

        return lResult;
    }

    void LabelList::SaveToImport()
    {
        assert(0xffff >= mLabels.size());

        const char* lToImport = GetToImport();

        FILE* lFile;

        auto lErr = fopen_s(&lFile, lToImport, "wb");
        if (0 != lErr)
        {
            char lMsg[64 + PATH_LENGTH];
            sprintf_s(lMsg, "Cannot open \"%s\" for writing", lToImport);
            KMS_EXCEPTION(APPLICATION_ERROR, lMsg, lErr);
        }

        assert(nullptr != lFile);

        auto lLabelCount = static_cast<uint16_t>(mLabels.size());

        auto lSize_byte = fwrite(&lLabelCount, 1, sizeof(lLabelCount), lFile);
        KMS_EXCEPTION_ASSERT(sizeof(lLabelCount) == lSize_byte, APPLICATION_ERROR, "Cannot write the output LBL file", lSize_byte);

        lSize_byte = fwrite(mHeader, 1, sizeof(mHeader), lFile);
        KMS_EXCEPTION_ASSERT(sizeof(mHeader) == lSize_byte, APPLICATION_ERROR, "Cannot write the output LBL file", lSize_byte);

        auto lFirst = true;

        uint8_t lMark[2] = { 0x01, 0x80 };

        for (auto lLabel : mLabels)
        {
            if (lFirst)
            {
                lFirst = false;
            }
            else
            {
                lSize_byte = fwrite(lMark, 1, sizeof(lMark), lFile);
                KMS_EXCEPTION_ASSERT(sizeof(lMark) == lSize_byte, APPLICATION_ERROR, "Cannot write the output LBL file", lSize_byte);
            }

            lLabel->Write(lFile);
        }

        auto lRet = fclose(lFile);
        assert(0 == lRet);

        GetSoftware()->ImportLabels(lToImport, GetExported());
    }

    // Private
    // //////////////////////////////////////////////////////////////////

    Label* LabelList::Create(const wchar_t* aName)
    {
        auto lResult = new Label(aName);

        mLabels.push_back(lResult);

        return lResult;
    }

    Label* LabelList::Find(const wchar_t* aName)
    {
        for (auto lLabel : mLabels)
        {
            assert(nullptr != lLabel);

            if (lLabel->mName == aName)
            {
                return lLabel;
            }
        }

        return nullptr;
    }

    Label* LabelList::FindOrCreate(const char* aName, bool* aChanged)
    {
        assert(nullptr != aName);
        assert(nullptr != aChanged);

        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> lConverter;

        std::wstring lName(lConverter.from_bytes(aName));

        auto lResult = Find(lName.c_str());
        if (nullptr == lResult)
        {
            ::Console::Change("New label", aName);

            *aChanged = true;

            lResult = Create(lName.c_str());
        }

        return lResult;
    }

    unsigned int LabelList::FindLanguageIndex(const char* aId) const
    {
        unsigned int lResult = 0;

        for (auto& lEntry : mLanguages.mInternal)
        {
            auto lId = dynamic_cast<const DI::String*>(lEntry.Get());
            assert(nullptr != lId);

            if (*lId == aId)
            {
                return lResult;
            }

            lResult++;
        }

        KMS_EXCEPTION(APPLICATION_ERROR, "Corrupted source file", aId);
    }

}
