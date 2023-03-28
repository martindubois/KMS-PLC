
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/EBPro_AddressList.cpp

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KMS/Cfg/MetaData.h>
#include <KMS/Console/Color.h>
#include <KMS/Convert.h>

// ===== Local ==============================================================
#include "../Common/EBPro/Address.h"

#include "../Common/EBPro/AddressList.h"

// Constants
// //////////////////////////////////////////////////////////////////////////

static const KMS::Cfg::MetaData MD_EXPORTED_CSV ("Exported_CSV = {Path}");
static const KMS::Cfg::MetaData MD_SOURCES      ("Sources += {Path}");
static const KMS::Cfg::MetaData MD_TO_IMPORT_CSV("ToImport_CSV = {Path}");

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

static AddressType ToAddressType(const char* aIn);

namespace EBPro
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    AddressList::AddressList()
    {
        mSources.SetCreator(KMS::DI::String_Expand::Create);

        AddEntry("Exported_CSV", &mExported_CSV, false, &MD_EXPORTED_CSV);
        AddEntry("Sources"     , &mSources     , false, &MD_SOURCES);
        AddEntry("ToImport_CSV", &mToImport_CSV, false, &MD_TO_IMPORT_CSV);
    }

    AddressList::~AddressList()
    {
        for (Address* lAddress : mAddresses)
        {
            assert(NULL != lAddress);

            delete lAddress;
        }
    }

    void AddressList::Import()
    {
        for (const KMS::DI::Container::Entry& lEntry : mSources.mInternal)
        {
            const KMS::DI::String* lSource = dynamic_cast<const KMS::DI::String*>(lEntry.Get());
            assert(NULL != lSource);

            std::cout << "Importing " << lSource->Get() << " ..." << std::endl;

            KMS::Text::File_ASCII lFile;

            lFile.Read(KMS::File::Folder::CURRENT, lSource->Get());

            lFile.RemoveComments_Script();
            lFile.RemoveEmptyLines();

            bool lChanged = false;

            for (const std::string& lLine : lFile.mLines)
            {
                char lName[NAME_LENGTH];
                char lType[NAME_LENGTH];
                char lAddr[NAME_LENGTH];

                if (3 == sscanf_s(lLine.c_str(), "ADDRESS %[^ \n\r\t] %[^ \n\r\t] %[^ \n\r\t]", lName SizeInfo(lName), lType SizeInfo(lType), lAddr SizeInfo(lAddr)))
                {
                    lChanged |= Import(lName, ToAddressType(lType), KMS::Convert::ToUInt16(lAddr));
                }
            }

            if (lChanged && (0 < mToImport_CSV.GetLength()))
            {
                mFile_CSV.Write(KMS::File::Folder::CURRENT, mToImport_CSV.Get());
            }

            std::cout << "Imported" << std::endl;
        }
    }

    void AddressList::Import(const ::AddressList& aAL)
    {
        bool lChanged = false;

        for (const ::Address& lA : aAL)
        {
            char lLine[LINE_LENGTH];

            Address* lAddress = Find_ByName(lA.GetName());
            if (NULL == lAddress)
            {
                lChanged = true;

                lAddress = new Address(lA.GetName(), lA.GetType(), lA.GetAddress(), mFile_CSV.GetLineCount());

                lAddress->GetLine(lLine, sizeof(lLine));

                mFile_CSV.AddLine(lLine);
            }
            else
            {
                if (lAddress->Set(lA.GetType(), lA.GetAddress()))
                {
                    lChanged = true;

                    lAddress->GetLine(lLine, sizeof(lLine));

                    mFile_CSV.ReplaceLine(lAddress->GetLineNo(), lLine);
                }
            }
        }

        if (lChanged && (0 < mToImport_CSV.GetLength()))
        {
            mFile_CSV.Write(KMS::File::Folder::CURRENT, mToImport_CSV.Get());
        }
    }

    void AddressList::Parse()
    {
        if (!mFile_CSV.mLines.empty())
        {
            std::cout << "Parsing " << mExported_CSV.Get() << " ..." << std::endl;

            unsigned int lLineNo = 0;

            for (const std::string lLine : mFile_CSV.mLines)
            {
                Address* lA = new Address(lLine.c_str(), lLineNo);

                mAddresses.push_back(lA);

                mAddresses_ByName.insert(ByName::value_type(lA->GetName(), lA));

                lLineNo++;
            }

            std::cout << "Parsed" << std::endl;
        }
    }

    void AddressList::Read()
    {
        KMS::File::Folder lCurrent(KMS::File::Folder::Id::CURRENT);

        if (0 < mExported_CSV.GetLength())
        {
            std::cout << "Reading " << mExported_CSV.Get() << " ..." << std::endl;

            mFile_CSV.Read(lCurrent, mExported_CSV.Get());

            std::cout << "Read" << std::endl;
        }
    }

    void AddressList::ValidateConfig() const
    {
        KMS::File::Folder lCurrent(KMS::File::Folder::Id::CURRENT);

        if (0 < mExported_CSV.GetLength())
        {
            const char* lStr = mExported_CSV.Get();

            char lMsg[64 + PATH_LENGTH];
            sprintf_s(lMsg, "\"%s\" does not exist", lStr);
            KMS_EXCEPTION_ASSERT(lCurrent.DoesFileExist(lStr), APPLICATION_USER_ERROR, lMsg, "");
        }
    }

    void AddressList::Verify() const
    {
        std::cout << "Verifying addresses ..." << std::endl;

        for (Internal::const_iterator lItA = mAddresses.begin(); lItA != mAddresses.end(); lItA++)
        {
            for (Internal::const_iterator lItB = lItA + 1; lItB != mAddresses.end(); lItB++)
            {
                if (0 == strcmp((*lItA)->GetName(), (*lItB)->GetName()))
                {
                    std::cout << KMS::Console::Color::YELLOW << "    WARNING  2 addresses are named ";
                    std::cout << (*lItA)->GetName() << KMS::Console::Color::WHITE << std::endl;
                }

                if (   ((*lItA)->GetType() == (*lItB)->GetType())
                    && (0 == strcmp((*lItA)->GetAddress(), (*lItB)->GetAddress())))
                {
                    std::cout << KMS::Console::Color::YELLOW << "    WARNING  The addresses named " << (*lItA)->GetName() << " and ";
                    std::cout << (*lItB)->GetName() << " are the same (" << (*lItA)->GetAddress() << ")" << KMS::Console::Color::WHITE << std::endl;
                }
            }
        }

        std::cout << "Verified" << std::endl;
    }

    // Private
    // //////////////////////////////////////////////////////////////////////

    Address* AddressList::Find_ByName(const char* aName)
    {
        ByName::iterator lIt = mAddresses_ByName.find(aName);
        if (mAddresses_ByName.end() == lIt)
        {
            return NULL;
        }

        assert(NULL != lIt->second);

        return lIt->second;
    }

    bool AddressList::Import(const char* aName, AddressType aType, uint16_t aAddr)
    {
        bool lResult = false;

        char lLine[LINE_LENGTH];

        Address* lAddress = Find_ByName(aName);
        if (NULL == lAddress)
        {
            lResult = true;

            lAddress = new Address(aName, aType, aAddr, mFile_CSV.GetLineCount());

            lAddress->GetLine(lLine, sizeof(lLine));

            mAddresses.push_back(lAddress);

            mAddresses_ByName.insert(ByName::value_type(aName, lAddress));

            mFile_CSV.AddLine(lLine);
        }
        else
        {
            if (lAddress->Set(aType, aAddr))
            {
                lResult = true;

                lAddress->GetLine(lLine, sizeof(lLine));

                mFile_CSV.ReplaceLine(lAddress->GetLineNo(), lLine);
            }
        }

        return lResult;
    }

}

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

AddressType ToAddressType(const char* aIn)
{
    assert(NULL != aIn);

    if (0 == strcmp("LB"           , aIn)) { return AddressType::LOCAL_HMI_LB      ; }
    if (0 == strcmp("LW"           , aIn)) { return AddressType::LOCAL_HMI_LW      ; }
    if (0 == strcmp("LW_BIT"       , aIn)) { return AddressType::LOCAL_HMI_LW_BIT  ; }
    if (0 == strcmp("RW_A"         , aIn)) { return AddressType::LOCAL_HMI_RW_A    ; }
    if (0 == strcmp("RW_A_BIT"     , aIn)) { return AddressType::LOCAL_HMI_RW_A_BIT; }
    if (0 == strcmp("MODBUS_RTU_1X", aIn)) { return AddressType::MODBUS_RTU_1X     ; }
    if (0 == strcmp("MODBUS_RTU_4X", aIn)) { return AddressType::MODBUS_RTU_4X     ; }

    char lMsg[64 + NAME_LENGTH];
    sprintf_s(lMsg, "\"%s\" is not a valid address type", aIn);

    KMS_EXCEPTION(APPLICATION_USER_ERROR, lMsg, "");
}
