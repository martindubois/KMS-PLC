
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGY_Project.cpp

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KMS/Cfg/MetaData.h>

// ===== Local ==============================================================
#include "../Common/TRiLOGY/Word.h"

#include "../Common/TRiLOGY/Project.h"

// Constants
// //////////////////////////////////////////////////////////////////////////

static const KMS::Cfg::MetaData MD_FILE_NAME_PC6        ("FileName_PC6 = {Path}");
static const KMS::Cfg::MetaData MD_SHARED_ADDRESS_REG_EX("SharedAddressRegEx = {RegEx}");
static const KMS::Cfg::MetaData MD_SOURCES              ("Sources += {Path}");

namespace TRiLOGY
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    Project::Project() : mInputs("input", 1), mOutputs("output", 1), mRelays("relay", 1025)
    {
        mSources.SetCreator(KMS::DI::String_Expand::Create);

        AddEntry("FileName_PC6"      , &mFileName_PC6      , false, &MD_FILE_NAME_PC6);
        AddEntry("SharedAddressRegEx", &mSharedAddressRegEx, false, &MD_SHARED_ADDRESS_REG_EX);
        AddEntry("Sources"           , &mSources           , false, &MD_SOURCES);
    }

    bool Project::IsValid() const { return 0 < mFile_PC6.GetLineCount(); }

    void Project::Export() const
    {
        // TODO
    }

    void Project::Import()
    {
        // TODO
    }

    void Project::Parse()
    {
        if (0 < mFile_PC6.GetLineCount())
        {
            std::cout << "Parsing " << mFileName_PC6.Get() << " ..." << std::endl;

            const wchar_t* lLine;
            unsigned int   lLineNo = 1;
            unsigned int   lLineCount = mFile_PC6.GetLineCount();

            lLineNo = mInputs    .Parse(mFile_PC6, lLineNo);
            lLineNo = mOutputs   .Parse(mFile_PC6, lLineNo);
            lLineNo = mRelays    .Parse(mFile_PC6, lLineNo);
            lLineNo = mTimers    .Parse(mFile_PC6, lLineNo);
            lLineNo = mSequencers.Parse(mFile_PC6, lLineNo);

            // ===== Circuits ===============================================
            for (; lLineNo < lLineCount; lLineNo++)
            {
                lLine = mFile_PC6.GetLine(lLineNo);
                if (0 == wcscmp(L"~END_CIRCUIT~\r", lLine)) { lLineNo++; break; }
            }

            lLineNo = mFunctions.Parse_Code(mFile_PC6, lLineNo);
            lLineNo = mFunctions.Parse_Name(mFile_PC6, lLineNo);

            // ===== Quick tags =============================================
            for (; lLineNo < lLineCount; lLineNo++)
            {
                lLine = mFile_PC6.GetLine(lLineNo);
                if (0 == wcscmp(L"~END_QUICKTAGS~\r", lLine)) { lLineNo++; break; }
            }

            lLineNo = mDefines.Parse(mFile_PC6, lLineNo);
        }
    }

    void Project::Read()
    {
        if (0 < mFileName_PC6.GetLength())
        {
            KMS::File::Folder lCurrent(KMS::File::Folder::Id::CURRENT);

            std::cout << "Reading " << mFileName_PC6.Get() << " ..." << std::endl;

            mFile_PC6.Read(lCurrent, mFileName_PC6.Get());

            std::cout << "Read" << std::endl;
        }
    }

    void Project::ValidateConfig() const
    {
        KMS::File::Folder lCurrent(KMS::File::Folder::Id::CURRENT);

        if (0 < mFileName_PC6.GetLength())
        {
            KMS_EXCEPTION_ASSERT(lCurrent.DoesFileExist(mFileName_PC6.Get()), CONFIG_VALUE, "The .PC6 file does not exist", mFileName_PC6.Get());
        }

        const KMS::DI::Array::Internal& lInternal = mSources.GetInternal();
        for (const KMS::DI::Container::Entry& lEntry : lInternal)
        {
            const KMS::DI::String* lSource = dynamic_cast<const KMS::DI::String*>(lEntry.Get());
            assert(NULL != lSource);

            KMS_EXCEPTION_ASSERT(lCurrent.DoesFileExist(lSource->Get()), CONFIG_VALUE, "The source file does not exist", lSource->Get());
        }
    }

    void Project::Verify()
    {
        if (0 < mFile_PC6.GetLineCount())
        {
            std::cout << "Verifying " << mFileName_PC6.Get() << " ..." << std::endl;

            // TODO For the verification, use a version of mFile_PC6 without
            //      comment

            mDefines   .Verify(mFile_PC6);
            mFunctions .Verify(mFile_PC6);
            mInputs    .Verify(mFile_PC6);
            mOutputs   .Verify(mFile_PC6);
            mRelays    .Verify(mFile_PC6);
            mSequencers.Verify(mFile_PC6);
            mTimers    .Verify(mFile_PC6);

            // TODO

            std::cout << "Verified" << std::endl;
        }
    }

    bool Project::VerifyAddress_1X(uint16_t aAddr) const
    {
        uint16_t               lAddr = aAddr;
        const TRiLOGY::Object* lObject;

        if (1025 > lAddr)
        {
            lObject = mInputs.FindObject_ByIndex(lAddr);
        }
        else
        {
            lAddr -= 1025;
            lObject = mRelays.FindObject_ByIndex(lAddr);
        }

        if (NULL == lObject)
        {
            return false;
        }

        if (lObject->TestFlag(TRiLOGY::Object::FLAG_NOT_USED))
        {
            return false;
        }

        return true;
    }

    bool Project::VerifyAddress_4X(uint16_t aAddr) const
    {
        uint16_t lAddr = aAddr;

        if (1000 > lAddr)
        {
            return false;
        }

        lAddr -= 1000;

        const Word* lWord = mDefines.mWords.Find_ByOffset(lAddr);
        if (NULL == lWord)
        {
            return false;
        }

        if (lWord->TestFlag(TRiLOGY::Object::FLAG_NOT_USED))
        {
            return false;
        }

        return true;
    }

    void Project::Write() const
    {
        // TODO
    }

    AddressList* Project::GetSharedAddresses() const
    {
        AddressList* lResult = NULL;

        if (0 < mSharedAddressRegEx.GetLength())
        {
            lResult = new AddressList;

            std::regex lRegEx(mSharedAddressRegEx.Get());

            mInputs .GetAddresses(lRegEx, lResult);
            mOutputs.GetAddresses(lRegEx, lResult);
            mRelays .GetAddresses(lRegEx, lResult);

            mDefines.mWords.GetAddresses(lRegEx, lResult);
        }

        return lResult;
    }

}
