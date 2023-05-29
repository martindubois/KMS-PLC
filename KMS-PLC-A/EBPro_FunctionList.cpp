
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/EBPro_FunctionList.cpp

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KMS/Cfg/MetaData.h>

// ===== Local ==============================================================
#include "../Common/EBPro/Software.h"

#include "../Common/EBPro/FunctionList.h"

#include "EBPro/DataPtr.h"
#include "EBPro/Function.h"

using namespace KMS;

// Constants
// //////////////////////////////////////////////////////////////////////////

static const Cfg::MetaData MD_EXPORTED ("Exported = {Path}.mlb");
static const Cfg::MetaData MD_SOURCES  ("Sources += {Path}.mlb.in0");
static const Cfg::MetaData MD_TO_IMPORT("ToImport = {Path}.mlb");

namespace EBPro
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    FunctionList::FunctionList(Software* aSoftware) : mFile(NULL), mFile_Data(NULL), mSoftware(aSoftware)
    {
        assert(NULL != aSoftware);

        memset(&mParsed_Values, 0, sizeof(mParsed_Values));

        mSources.SetCreator(DI::String_Expand::Create);

        AddEntry("Exported", &mExported, false, &MD_EXPORTED);
        AddEntry("Sources" , &mSources , false, &MD_SOURCES);
        AddEntry("ToImport", &mToImport, false, &MD_TO_IMPORT);
    }

    FunctionList::~FunctionList()
    {
        for (auto lPair : mFunctions_ByName)
        {
            assert(NULL != lPair.second);

            delete lPair.second;
        }

        if (NULL != mFile)
        {
            delete mFile;
        }
    }

    void FunctionList::Import()
    {
        assert(NULL != mSoftware);

        if (0 < mSources.GetCount())
        {
            std::cout << "Importing sources ..." << std::endl;

            auto lChanged = false;

            for (const auto& lEntry : mSources.mInternal)
            {
                const DI::String* lSource = dynamic_cast<const DI::String*>(lEntry.Get());
                assert(NULL != lSource);

                lChanged |= Import(lSource->Get());
            }

            if (lChanged)
            {
                std::cout << "Imported\n";

                char lPath[PATH_LENGTH];

                File::Folder::CURRENT.GetPath(mToImport.Get(), lPath, sizeof(lPath));

                FILE* lFile;

                auto lRet = fopen_s(&lFile, lPath, "wb");

                char lMsg[64 + PATH_LENGTH];
                sprintf_s(lMsg, "Cannot open \"%s\" for writting", lPath);
                KMS_EXCEPTION_ASSERT(0 == lRet, APPLICATION_ERROR, lMsg, lRet);

                Header_Write(lFile);

                for (const auto lPair : mFunctions_ByName)
                {
                    assert(NULL != lPair.second);

                    lPair.second->Write(lFile);
                }

                fprintf(lFile, "\r\n");

                lRet = fclose(lFile);
                assert(0 == lRet);

                mSoftware->ImportFunctions(mToImport.Get(), mExported.Get());
            }
            else
            {
                std::cout << "Imported - No change" << std::endl;
            }
        }
    }

    void FunctionList::Parse()
    {
        if (NULL != mFile_Data)
        {
            std::cout << "Parsing " << mExported.Get() << " ..." << std::endl;

            DataPtr lPtr(mFile_Data, mFile->GetMappedSize());

            Header_Parse(&lPtr);

            while (!lPtr.IsAtEnd())
            {
                auto lFunction = new Function();
                lFunction->Read(&lPtr);

                Add(lFunction);
            }

            std::cout << "Parsed" << std::endl;
        }
    }

    void FunctionList::Read()
    {
        if (0 < mExported.GetLength())
        {
            std::cout << "Reading " << mExported.Get() << " ..." << std::endl;

            mFile = new File::Binary(File::Folder::CURRENT, mExported);
            assert(NULL != mFile);

            mFile_Data = reinterpret_cast<const char*>(mFile->Map());
            assert(NULL != mFile_Data);

            std::cout << "Read" << std::endl;
        }
    }

    void FunctionList::ValidateConfig() const
    {
        char lMsg[64 + PATH_LENGTH];

        if (0 < mExported.GetLength())
        {
            auto lStr = mExported.Get();

            sprintf_s(lMsg, "\"%s\" does not exist", lStr);
            KMS_EXCEPTION_ASSERT(File::Folder::CURRENT.DoesFileExist(lStr), APPLICATION_USER_ERROR, lMsg, "");
        }

        for (auto& lEntry : mSources.mInternal)
        {
            auto lSource = dynamic_cast<const DI::String*>(lEntry.Get());
            assert(NULL != lSource);

            sprintf_s(lMsg, "\"%s\" does not exist", lSource->Get());
            KMS_EXCEPTION_ASSERT(File::Folder::CURRENT.DoesFileExist(lSource->Get()), APPLICATION_USER_ERROR, lMsg, "");
        }

        if (0 < mSources.GetCount())
        {
            KMS_EXCEPTION_ASSERT(0 < mToImport.GetLength(), APPLICATION_USER_ERROR, "Sources are present, ToImport cannot be empty", "");
        }
    }

    // Private
    // //////////////////////////////////////////////////////////////////////

    void FunctionList::Add(Function* aIn)
    {
        assert(NULL != aIn);

        mFunctions_ByName.insert(ByName::value_type(aIn->GetName(), aIn));
    }

    Function* FunctionList::FindByName(const char* aName)
    {
        assert(NULL != aName);

        auto lIt = mFunctions_ByName.find(aName);
        if (mFunctions_ByName.end() == lIt)
        {
            return NULL;
        }

        assert(NULL != lIt->second);

        return lIt->second;
    }

    void FunctionList::Header_Parse(DataPtr* aPtr)
    {
        assert(NULL != aPtr);

        aPtr->Read(&mParsed_Type);
        aPtr->Verify(14);
        aPtr->Verify(1048577);
        aPtr->Read(&mParsed_Name);
        aPtr->Read(mParsed_Values + 0);
        aPtr->Read(mParsed_Values + 1);
    }

    void FunctionList::Header_Write(FILE* aOut)
    {
        DataPtr::Write(aOut, mParsed_Type);
        DataPtr::Write(aOut, 14);
        DataPtr::Write(aOut, 1048577);
        DataPtr::Write(aOut, mParsed_Name);
        DataPtr::Write(aOut, mParsed_Values[0]);
        DataPtr::Write(aOut, mParsed_Values[1]);
    }

    bool FunctionList::Import(const char* aFileName)
    {
        Text::File_ASCII lFile;

        lFile.Read(File::Folder::CURRENT, aFileName);

        lFile.RemoveComments_Script();

        unsigned int lLineNo = 0;
        auto         lResult = false;

        while (lFile.GetLineCount() > lLineNo)
        {
            if (0 == strncmp(lFile.GetLine(lLineNo), "FUNCTION", 8))
            {
                auto lFunction = new Function();

                lLineNo = lFunction->Read(lFile, lLineNo);

                auto lExisting = FindByName(lFunction->GetName());
                if (NULL == lExisting)
                {
                    Add(lFunction);
                    lResult = true;
                }
                else
                {
                    if (*lFunction != *lExisting)
                    {
                        Replace(lFunction);
                        lResult = true;
                    }
                }
            }
        }

        return lResult;
    }

    void FunctionList::Replace(Function* aIn)
    {
        assert(NULL != aIn);

        auto lIt = mFunctions_ByName.find(aIn->GetName());
        assert(mFunctions_ByName.end() != lIt);
        assert(NULL != lIt->second);

        delete lIt->second;

        lIt->second = aIn;
    }

}
