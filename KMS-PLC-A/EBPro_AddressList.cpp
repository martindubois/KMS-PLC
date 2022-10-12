
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/EBPro_AddressList.cpp

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KMS/Cfg/MetaData.h>
#include <KMS/Console/Color.h>

// ===== Local ==============================================================
#include "../Common/EBPro/Address.h"

#include "../Common/EBPro/AddressList.h"

// Constants
// //////////////////////////////////////////////////////////////////////////

static const KMS::Cfg::MetaData MD_EXPORTED_CSV ("Exported_CSV = {Path}");
static const KMS::Cfg::MetaData MD_TO_IMPORT_CSV("ToImport_CSV = {Path}");

namespace EBPro
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    AddressList::AddressList()
    {
        AddEntry("Exported_CSV", &mExported_CSV, false, &MD_EXPORTED_CSV);
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

            KMS_EXCEPTION_ASSERT(lCurrent.DoesFileExist(lStr), CONFIG_VALUE, "The exported CSV file does not exist", lStr);
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

}
