
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/EBPro_FunctionList.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "../Common/EBPro/Software.h"

#include "../Common/EBPro/FunctionList.h"

#include "Console.h"

#include "EBPro/DataPtr.h"
#include "EBPro/Function.h"

using namespace KMS;

namespace EBPro
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    FunctionList::FunctionList(Software* aSoftware) : List(aSoftware), mFile(NULL), mFile_Data(NULL)
    {
        memset(&mParsed_Values, 0, sizeof(mParsed_Values));
    }

    void FunctionList::Parse()
    {
        if (NULL != mFile_Data)
        {
            Console::Progress_Begin("Parsing ", GetExported());

            DataPtr lPtr(mFile_Data, mFile->GetMappedSize());

            Header_Parse(&lPtr);

            while (!lPtr.IsAtEnd())
            {
                auto lFunction = new Function();
                lFunction->Read(&lPtr);

                Add(lFunction);
            }

            Console::Progress_End("Parsed");
        }
    }

    // ===== List ===========================================================

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

    // Protected
    // //////////////////////////////////////////////////////////////////////

    // ===== List ===========================================================

    bool FunctionList::ImportSource(const char* aFileName)
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
                    Console::Change("New function", lFunction->GetName());

                    Add(lFunction);
                    lResult = true;
                }
                else
                {
                    if (*lFunction != *lExisting)
                    {
                        Console::Change("Function changed", lFunction->GetName());

                        Replace(lFunction);
                        lResult = true;
                    }
                }
            }
        }

        return lResult;
    }

    void FunctionList::ReadExported()
    {
        mFile = new File::Binary(File::Folder::CURRENT, GetExported());
        assert(NULL != mFile);

        mFile_Data = reinterpret_cast<const char*>(mFile->Map());
        assert(NULL != mFile_Data);
    }

    void FunctionList::SaveToImport()
    {
        char        lPath[PATH_LENGTH];
        const char* lToImport = GetToImport();

        File::Folder::CURRENT.GetPath(lToImport, lPath, sizeof(lPath));

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

        GetSoftware()->ImportFunctions(lToImport, GetExported());
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
