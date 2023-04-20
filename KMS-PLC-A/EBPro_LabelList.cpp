
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
#include <KMS/Console/Color.h>
#include <KMS/File/Folder.h> 
#include <KMS/Text/File_ASCII.h>

// ===== Local ==============================================================
#include "../Common/EBPro/Label.h"
#include "../Common/EBPro/LabelState.h"
#include "../Common/EBPro/Software.h"

#include "../Common/EBPro/LabelList.h"

#include "Convert.h"

using namespace KMS;

// Constants
// //////////////////////////////////////////////////////////////////////////

static const Cfg::MetaData MD_EXPORTED ("Exported = {Path}.lbl");
static const Cfg::MetaData MD_LANGUAGES("Languages += {id}");
static const Cfg::MetaData MD_SOURCES  ("Sources += {Path}.lbl.in0");
static const Cfg::MetaData MD_TO_IMPORT("ToImport = {Path}.lbl");

namespace EBPro
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    LabelList::LabelList(Software* aSoftware) : mSoftware(aSoftware)
    {
        assert(NULL != aSoftware);

        mLanguages.SetCreator(DI::String::Create);
        mSources  .SetCreator(DI::String_Expand::Create);

        AddEntry("Exported" , &mExported , false , &MD_EXPORTED);
        AddEntry("Languages", &mLanguages, false , &MD_LANGUAGES);
        AddEntry("Sources"  , &mSources  , false , &MD_SOURCES);
        AddEntry("ToImport" , &mToImport , false , &MD_TO_IMPORT);
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
        if (0 < mExported.GetLength())
        {
            std::string lPath = mExported.mInternal + ".in0";

            std::cout << "Exporting " << lPath << " ..." << std::endl;

            FILE* lFile;

            errno_t lErr = fopen_s(&lFile, lPath.c_str(), "w,ccs=UTF-8");

            char lMsg[64 + PATH_LENGTH];
            sprintf_s(lMsg, "Cannot open \"%s\" for writing", lPath.c_str());
            KMS_EXCEPTION_ASSERT(0 == lErr, APPLICATION_ERROR, lMsg, lErr);

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
        bool lChanged = false;

        for (const DI::Container::Entry& lEntry : mSources.mInternal)
        {
            const DI::String* lSource = dynamic_cast<const DI::String*>(lEntry.Get());
            assert(NULL != lSource);

            std::cout << "Importing " << lSource->Get() << " ..." << std::endl;

            Text::File_ASCII lFile;

            lFile.Read(File::Folder::CURRENT, lSource->Get());

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
                if (0 == strncmp("    STATE", lLine.c_str(), 9))
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

            std::cout << "Imported" << std::endl;
        }

        if (lChanged)
        {
            Save_ToImport_LBL();
        }
        else
        {
            if ((0 < mToImport.GetLength()) && (File::Folder::CURRENT.DoesFileExist(mToImport.Get())))
            {
                File::Folder lCurrent(File::Folder::Id::CURRENT);

                lCurrent.Delete(mToImport.Get());
            }
        }
    }

    void LabelList::Parse()
    {
        if (0 < mExported.GetLength())
        {
            std::cout << "Parsing " << mExported.Get() << " ..." << std::endl;

            FILE* lFile;

            errno_t lErr = fopen_s(&lFile, mExported.Get(), "rb");

            char lMsg[64 + PATH_LENGTH];
            sprintf_s(lMsg, "Cannot open \"%s\" for reading", mExported.Get());
            KMS_EXCEPTION_ASSERT(0 == lErr, APPLICATION_ERROR, lMsg, lErr);

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
        if (0 < mExported.GetLength())
        {
            const char* lStr = mExported.Get();

            char lMsg[64 + PATH_LENGTH];

            sprintf_s(lMsg, "\"%s\" does not exist", lStr);
            KMS_EXCEPTION_ASSERT(File::Folder::CURRENT.DoesFileExist(lStr), APPLICATION_ERROR, lMsg, "");

            KMS_EXCEPTION_ASSERT(0 < mLanguages.GetCount(), APPLICATION_USER_ERROR, "The configuration file must define language", "");

            for (const DI::Container::Entry& lEntry : mLanguages.mInternal)
            {
                const DI::String* lLanguage = dynamic_cast<const DI::String*>(lEntry.Get());
                assert(NULL != lLanguage);

                sprintf_s(lMsg, "\"%s\" is not a valid language id", lLanguage->Get());
                KMS_EXCEPTION_ASSERT(2 == lLanguage->GetLength(), APPLICATION_USER_ERROR, lMsg, "");
            }

            if (0 < mSources.GetCount())
            {
                for (const DI::Container::Entry& lEntry : mSources.mInternal)
                {
                    const DI::String* lSource = dynamic_cast<const DI::String*>(lEntry.Get());
                    assert(NULL != lSource);

                    lStr = lSource->Get();

                    sprintf_s(lMsg, "\"%s\" does not exist", lStr);
                    KMS_EXCEPTION_ASSERT(File::Folder::CURRENT.DoesFileExist(lStr), APPLICATION_USER_ERROR, lMsg, "");
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
                    std::cout << Console::Color::YELLOW << "WARNING  The label " << (*lItA)->mName.c_str() << " is present twice" << Console::Color::WHITE << std::endl;
                }
            }

            (*lItA)->Verify();
        }

        // TODO Verify more

        std::cout << "Verified" << std::endl;
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

        for (const DI::Container::Entry& lEntry : mLanguages.mInternal)
        {
            const DI::String* lId = dynamic_cast<const DI::String*>(lEntry.Get());
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
        assert(NULL != mSoftware);

        if (0 < mToImport.GetLength())
        {
            std::cout << "Saving " << mToImport.Get() << " ..." << std::endl;

            FILE* lFile;

            errno_t lErr = fopen_s(&lFile, mToImport.Get(), "wb");

            char lMsg[64 + PATH_LENGTH];
            sprintf_s(lMsg, "Cannot open \"%s\" for writing", mToImport.Get());
            KMS_EXCEPTION_ASSERT(0 == lErr, APPLICATION_ERROR, lMsg, lErr);

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

            mSoftware->ImportLabels(mToImport.Get(), mExported.Get());
        }
    }

}
