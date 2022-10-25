
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/EBPro_LabelList.cpp

#include "Component.h"

// ===== C++ ================================================================
#include <codecvt>

// ===== Import/Includes ====================================================
#include <KMS/Cfg/MetaData.h>
#include <KMS/Console/Color.h>
#include <KMS/File/Folder.h> 
#include <KMS/Text/File_ASCII.h>

// ===== Local ==============================================================
#include "../Common/EBPro/Label.h"
#include "../Common/EBPro/LabelState.h"

#include "../Common/EBPro/LabelList.h"

#include "Convert.h"

// Constants
// //////////////////////////////////////////////////////////////////////////

static const KMS::Cfg::MetaData MD_EXPORTED_LBL    ("Exported_LBL = {Path}");
static const KMS::Cfg::MetaData MD_LANGUAGES       ("LANGUAGES += {id}");
static const KMS::Cfg::MetaData MD_SOURCES         ("Sources += {Path}");
static const KMS::Cfg::MetaData MD_TO_IMPORT_LBL   ("ToImport_LBL = {Path}");

namespace EBPro
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    LabelList::LabelList()
    {
        mLanguages.SetCreator(KMS::DI::String::Create);
        mSources  .SetCreator(KMS::DI::String_Expand::Create);

        AddEntry("Exported_LBL"    , &mExported_LBL, false , &MD_EXPORTED_LBL);
        AddEntry("Languages"       , &mLanguages   , false , &MD_LANGUAGES);
        AddEntry("Sources"         , &mSources     , false , &MD_SOURCES);
        AddEntry("ToImport_LBL"    , &mToImport_LBL, false , &MD_TO_IMPORT_LBL);
    }

    LabelList::~LabelList()
    {
        for (Label* lLabel : mLabels)
        {
            assert(NULL != lLabel);

            delete lLabel;
        }
    }

    void LabelList::Export() const
    {
        if (0 < mExported_LBL.GetLength())
        {
            std::string lPath = mExported_LBL.mInternal + ".in0";

            std::cout << "Exporting " << lPath << " ..." << std::endl;

            FILE* lFile;

            errno_t lErr = fopen_s(&lFile, lPath.c_str(), "w,ccs=UTF-8");
            KMS_EXCEPTION_ASSERT(0 == lErr, APPLICATION_ERROR, "Cannot open the output LBL.IN0 file", lPath.c_str());

            assert(NULL != lFile);

            for (const Label* lLabel : mLabels)
            {
                lLabel->Export(lFile, mLanguages);
            }

            int lRet = fclose(lFile);
            assert(0 == lRet);

            std::cout << "Exported" << std::endl;
        }
    }

    void LabelList::Import()
    {
        KMS::File::Folder lCurrent(KMS::File::Folder::Id::CURRENT);

        bool lChanged = false;

        for (const KMS::DI::Container::Entry& lEntry : mSources.mInternal)
        {
            const KMS::DI::String* lSource = dynamic_cast<const KMS::DI::String*>(lEntry.Get());
            assert(NULL != lSource);

            std::cout << "Importing " << lSource->Get() << " ..." << std::endl;

            KMS::Text::File_ASCII lFile;

            lFile.Read(lCurrent, lSource->Get());

            lFile.RemoveComments_Script();
            lFile.RemoveEmptyLines();

            Label      * lLabel = NULL;
            LabelState * lState = NULL;
            unsigned int lStateCount;

            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> lConverter;

            for (const std::string& lLine : lFile.mLines)
            {
                char lLanguage[NAME_LENGTH];
                char lText[LINE_LENGTH];

                // TODO Support something else than 4 spaces.
                if (0 == strcmp("    STATE", lLine.c_str()))
                {
                    KMS_EXCEPTION_ASSERT(NULL != lLabel, APPLICATION_ERROR, "Corruted source file", lLine.c_str());

                    lState = lLabel->FindOrCreate(lStateCount, &lChanged);
                    lStateCount++;
                }
                else if (1 == sscanf_s(lLine.c_str(), "LABEL %[^\n\r\t]", lText SizeInfo(lText)))
                {
                    lLabel      = FindOrCreate(lText, &lChanged);
                    lStateCount = 0;

                    assert(NULL != lLabel);
                }
                else if (2 == sscanf_s(lLine.c_str(), " %[a-z] %[^\n\r\t]", lLanguage SizeInfo(lLanguage), lText SizeInfo(lText)))
                {
                    KMS_EXCEPTION_ASSERT(NULL != lState, APPLICATION_ERROR, "Corruted source file", lLine.c_str());

                    std::wstring lString(lConverter.from_bytes(lText));

                    wchar_t lImportable[LINE_LENGTH];

                    ToImportableString(lString.c_str(), lImportable);

                    lChanged |= lState->Set(FindLanguageIndex(lLanguage), lImportable);
                }
            }

            std::cout << "Imported";
        }

        if (lChanged)
        {
            Save_ToImport_LBL();
        }
        else
        {
            if (0 < mToImport_LBL.GetLength())
            {
                lCurrent.Delete(mToImport_LBL.Get());
            }
        }
    }

    void LabelList::Parse()
    {
        if (0 < mExported_LBL.GetLength())
        {
            std::cout << "Parsing " << mExported_LBL.Get() << " ..." << std::endl;

            FILE* lFile;

            errno_t lErr = fopen_s(&lFile, mExported_LBL.Get(), "rb");
            KMS_EXCEPTION_ASSERT(0 == lErr, APPLICATION_ERROR, "Cannot open the exported LBL file", mExported_LBL.Get());

            assert(NULL != lFile);

            uint16_t lLabelCount;

            size_t lSize_byte = fread_s(&lLabelCount SizeInfo(lLabelCount) + 1, 1, sizeof(lLabelCount), lFile);
            KMS_EXCEPTION_ASSERT(sizeof(lLabelCount) == lSize_byte, APPLICATION_ERROR, "Cannot read the exported LBL file", lSize_byte);

            lSize_byte = fread_s(mHeader SizeInfo(mHeader) + 1, 1, sizeof(mHeader), lFile);
            KMS_EXCEPTION_ASSERT(sizeof(mHeader) == lSize_byte, APPLICATION_ERROR, "Cannot read the exported LBL file", lSize_byte);

            while (!feof(lFile))
            {
                Label* lLabel = new Label;

                lLabel->Read(lFile);

                mLabels.push_back(lLabel);
            }

            std::cout << "    " << mLabels.size() << " labels" << std::endl;

            KMS_EXCEPTION_ASSERT(lLabelCount == mLabels.size(), APPLICATION_ERROR, "Corrupted exported LBL file", lLabelCount);

            int lRet = fclose(lFile);
            assert(0 == lRet);

            std::cout << "Parsed" << std::endl;
        }
    }

    void LabelList::Read() {}

    void LabelList::ValidateConfig() const
    {
        KMS::File::Folder lCurrent(KMS::File::Folder::Id::CURRENT);

        if (0 < mExported_LBL.GetLength())
        {
            const char* lStr = mExported_LBL.Get();

            KMS_EXCEPTION_ASSERT(lCurrent.DoesFileExist(lStr), APPLICATION_ERROR, "The exported LBL file does not exist", lStr);

            KMS_EXCEPTION_ASSERT(0 < mLanguages.GetCount(), APPLICATION_USER_ERROR, "The configuration file must define language", "");

            for (const KMS::DI::Container::Entry& lEntry : mLanguages.mInternal)
            {
                const KMS::DI::String* lLanguage = dynamic_cast<const KMS::DI::String*>(lEntry.Get());
                assert(NULL != lLanguage);

                KMS_EXCEPTION_ASSERT(2 == lLanguage->GetLength(), APPLICATION_USER_ERROR, "The language id must be 2 characteres", lLanguage->Get());
            }

            if (0 < mSources.GetCount())
            {
                for (const KMS::DI::Container::Entry& lEntry : mSources.mInternal)
                {
                    const KMS::DI::String* lSource = dynamic_cast<const KMS::DI::String*>(lEntry.Get());
                    assert(NULL != lSource);

                    lStr = lSource->Get();

                    KMS_EXCEPTION_ASSERT(lCurrent.DoesFileExist(lStr), APPLICATION_USER_ERROR, "The source file does not exist", lStr);
                }
            }
        }
    }
    
    void LabelList::Verify() const
    {
        std::cout << "Verifying labels ..." << std::endl;

        for (Internal::const_iterator lItA = mLabels.begin(); lItA != mLabels.end(); lItA++)
        {
            for (Internal::const_iterator lItB = lItA + 1; lItB != mLabels.end(); lItB++)
            {
                if ((*lItA)->mName == (*lItB)->mName)
                {
                    std::cout << KMS::Console::Color::YELLOW << "WARNING  The label " << (*lItA)->mName.c_str() << " is present twice" << KMS::Console::Color::WHITE << std::endl;
                }
            }

            (*lItA)->Verify();
        }

        // TODO Verify more

        std::cout << "Verified" << std::endl;
    }

    void LabelList::Write() const
    {
    }

    // Private
    // //////////////////////////////////////////////////////////////////

    Label* LabelList::Create(const wchar_t* aName)
    {
        Label* lResult = new Label(aName);

        mLabels.push_back(lResult);

        return lResult;
    }

    Label* LabelList::Find(const wchar_t* aName)
    {
        for (Label* lLabel : mLabels)
        {
            assert(NULL != lLabel);

            if (lLabel->mName == aName)
            {
                return lLabel;
            }
        }

        return NULL;
    }

    Label* LabelList::FindOrCreate(const char* aName, bool* aChanged)
    {
        assert(NULL != aName);
        assert(NULL != aChanged);

        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> lConverter;

        std::wstring lName(lConverter.from_bytes(aName));

        Label* lResult = Find(lName.c_str());
        if (NULL == lResult)
        {
            *aChanged = true;

            lResult = Create(lName.c_str());
        }

        return lResult;
    }

    unsigned int LabelList::FindLanguageIndex(const char* aId) const
    {
        unsigned int lResult = 0;

        for (const KMS::DI::Container::Entry& lEntry : mLanguages.mInternal)
        {
            const KMS::DI::String* lId = dynamic_cast<const KMS::DI::String*>(lEntry.Get());
            assert(NULL != lId);

            if (*lId == aId)
            {
                return lResult;
            }

            lResult++;
        }

        KMS_EXCEPTION(APPLICATION_ERROR, "Corrupted source file", aId);
    }

    void LabelList::Save_ToImport_LBL() const
    {
        assert(0xffff >= mLabels.size());

        if (0 < mToImport_LBL.GetLength())
        {
            std::cout << "Saving " << mToImport_LBL.Get() << " ..." << std::endl;

            FILE* lFile;

            errno_t lErr = fopen_s(&lFile, mToImport_LBL.Get(), "wb");
            KMS_EXCEPTION_ASSERT(0 == lErr, APPLICATION_ERROR, "Cannot open the output LBL file", mToImport_LBL.Get());

            assert(NULL != lFile);

            uint16_t lLabelCount = static_cast<uint16_t>(mLabels.size());

            size_t lSize_byte = fwrite(&lLabelCount, 1, sizeof(lLabelCount), lFile);
            KMS_EXCEPTION_ASSERT(sizeof(lLabelCount) == lSize_byte, APPLICATION_ERROR, "Cannot write the output LBL file", lSize_byte);

            lSize_byte = fwrite(mHeader, 1, sizeof(mHeader), lFile);
            KMS_EXCEPTION_ASSERT(sizeof(mHeader) == lSize_byte, APPLICATION_ERROR, "Cannot write the output LBL file", lSize_byte);

            bool lFirst = true;

            uint8_t lMark[2] = { 0x01, 0x80 };

            for (const Label* lLabel : mLabels)
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

            int lRet = fclose(lFile);
            assert(0 == lRet);

            std::cout << "Saved" << std::endl;
        }
    }

}
