
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

using namespace KMS;

// Constants
// //////////////////////////////////////////////////////////////////////////

static const Cfg::MetaData MD_EXPORTED ("Exported = {Path}.csv");
static const Cfg::MetaData MD_SOURCES  ("Sources += {Path}.in0");
static const Cfg::MetaData MD_TO_IMPORT("ToImport = {Path}.csv");

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

static void Instruction_ToImport(const char* aImp, const char* aExp);

static AddressType ToAddressType(const char* aIn);

namespace EBPro
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    AddressList::AddressList()
    {
        mSources.SetCreator(DI::String_Expand::Create);

        AddEntry("Exported", &mExported, false, &MD_EXPORTED);
        AddEntry("Sources" , &mSources, false, &MD_SOURCES);
        AddEntry("ToImport", &mToImport, false, &MD_TO_IMPORT);
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
        for (const DI::Container::Entry& lEntry : mSources.mInternal)
        {
            const DI::String* lSource = dynamic_cast<const DI::String*>(lEntry.Get());
            assert(NULL != lSource);

            std::cout << "Importing " << lSource->Get() << " ..." << std::endl;

            Text::File_ASCII lFile;

            lFile.Read(File::Folder::CURRENT, lSource->Get());

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
                    lChanged |= Import(lName, ToAddressType(lType), Convert::ToUInt16(lAddr));
                }
            }

            std::cout << "Imported" << std::endl;

            if (lChanged && (0 < mToImport.GetLength()))
            {
                mFile_CSV.Write(File::Folder::CURRENT, mToImport.Get());

                Instruction_ToImport(mToImport.Get(), mExported.Get());
            }
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

        if (lChanged && (0 < mToImport.GetLength()))
        {
            mFile_CSV.Write(File::Folder::CURRENT, mToImport.Get());

            Instruction_ToImport(mToImport.Get(), mExported.Get());
        }
    }

    void AddressList::Parse()
    {
        if (!mFile_CSV.mLines.empty())
        {
            std::cout << "Parsing " << mExported.Get() << " ..." << std::endl;

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
        if (0 < mExported.GetLength())
        {
            std::cout << "Reading " << mExported.Get() << " ..." << std::endl;

            mFile_CSV.Read(File::Folder::CURRENT, mExported.Get());

            std::cout << "Read" << std::endl;
        }
    }

    void AddressList::ValidateConfig() const
    {
        if (0 < mExported.GetLength())
        {
            const char* lStr = mExported.Get();

            char lMsg[64 + PATH_LENGTH];
            sprintf_s(lMsg, "\"%s\" does not exist", lStr);
            KMS_EXCEPTION_ASSERT(File::Folder::CURRENT.DoesFileExist(lStr), APPLICATION_USER_ERROR, lMsg, "");
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
                    std::cout << Console::Color::YELLOW << "    WARNING  Line " << (*lItA)->GetLineNo() << " and " << (*lItB)->GetLineNo();
                    std::cout << "  2 addresses are named \"" << (*lItA)->GetName() << "\"" << Console::Color::WHITE << std::endl;
                }

                if (   ((*lItA)->GetType() == (*lItB)->GetType())
                    && (0 == strcmp((*lItA)->GetAddress(), (*lItB)->GetAddress())))
                {
                    std::cout << Console::Color::YELLOW << "    WARNING  Line " << (*lItA)->GetLineNo() << " and " << (*lItB)->GetLineNo();
                    std::cout << "  The addresses named \"" << (*lItA)->GetName() << "\" and \"" << (*lItB)->GetName();
                    std::cout << "\" are the same (" << (*lItA)->GetAddress() << ")" << Console::Color::WHITE << std::endl;
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

void Instruction_ToImport(const char* aImp, const char* aExp)
{
    assert(NULL != aImp);
    assert(NULL != aExp);

    std::cout << "INSTRUCTION\n";
    std::cout << "    Tool \"EBPro\"\n";
    //                     1           2          3          4          5          6          7
    //            1234567890 12345678 9012345678 90123456 7890123456 78901234 567890 123456789012   3456789
    std::cout << "    - Tab \"Project\" - Group \"Library\" - Click \"Address\" ==> \"Address Tag\n";
    //            1234567890123 45678901234567890 12345678901234567890123456789012345678901234567   89
    std::cout << "      Library\" dialog                                                      [ ]\n";
    //            1234567890123456 78901234567890 123456 78901 234567890123456789012345678901234567   89
    std::cout << "        - Click \"Import CSV...\" ==> \"Open\" dialog                         [ ]\n";
    std::cout << "            - Select \"" << aImp << "\"\n";
    //            123456789012345678901234 56789 012345678901234567890123456789012345678901234567   89
    std::cout << "              and click \"Open\"                                            [ ]\n";
    //            1234567890123456 78901234567890 123456 78901 234567890123456789012345678901234567   89
    std::cout << "        - Click \"Export CSV...\" ==> \"Open\" dialog                         [ ]\n";
    std::cout << "            - Select \"" << aExp << "\"\n";
    //            123456789012345678901234 56789 012345678901234567890123456789012345678901234567   89
    std::cout << "              and click \"Open\"                                            [ ]\n";
    //            1234567890123456 78901 234 56789 012345678901234567890123456789012345678901234567   89
    std::cout << "        - Click \"Exit\"                                                      [ ]\n";
    std::cout << "Presse ENTER to continue" << std::endl;

    char lLine[LINE_LENGTH];
    fgets(lLine, sizeof(lLine), stdin);
}

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
