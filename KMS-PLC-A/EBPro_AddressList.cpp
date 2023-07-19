
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/EBPro_AddressList.cpp

// TEST COVERAGE 2023-06-01 KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KMS/Convert.h>
#include <KMS/Enum.h>

// ===== Local ==============================================================
#include "../Common/EBPro/Software.h"

#include "../Common/EBPro/AddressList.h"

#include "Console.h"

#include "EBPro/Address.h"

using namespace KMS;

// Constants
// //////////////////////////////////////////////////////////////////////////

static const char* ADDRESS_TYPE_NAMES[] =
{
    "LB",
    "LW",
    "LW_BIT",
    "RW_A",
    "RW_A_BIT",
    "MODBUS_RTU_1X",
    "MODBUS_RTU_4X",
    "UNKNOWN",
};

namespace EBPro
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    AddressList::AddressList(Software* aSoftware) : List(aSoftware) {}

    // NOT TESTED
    void AddressList::ImportAddresses(const ::AddressList& aAL)
    {
        auto lChanged = false;

        if (!aAL.empty())
        {
            ValidateConfig_ToImport();

            for (auto& lA : aAL)
            {
                char lLine[LINE_LENGTH];

                auto lAddress = Find_ByName(lA.GetName());
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

            if (lChanged)
            {
                SaveToImport();
            }
        }
    }

    void AddressList::Parse()
    {
        if (!mFile_CSV.mLines.empty())
        {
            Console::Progress_Begin("Parsing", GetExported());

            unsigned int lLineNo = 0;

            for (const auto lLine : mFile_CSV.mLines)
            {
                auto lA = new Address(lLine.c_str(), lLineNo);

                mAddresses.push_back(lA);

                mAddresses_ByName.insert(ByName::value_type(lA->GetName(), lA));

                lLineNo++;
            }

            Console::Progress_End("Parsed");
        }
    }

    // NOT TESTED  The 2 warnings are not tested
    void AddressList::Verify() const
    {
        Console::Progress_Begin("Verifying addresses");

        for (auto lItA = mAddresses.begin(); lItA != mAddresses.end(); lItA++)
        {
            for (auto lItB = lItA + 1; lItB != mAddresses.end(); lItB++)
            {
                if (0 == strcmp((*lItA)->GetName(), (*lItB)->GetName()))
                {
                    Console::Warning_Begin();
                    std::cout << "Line " << (*lItA)->GetLineNo() << " and " << (*lItB)->GetLineNo();
                    std::cout << "  2 addresses are named \"" << (*lItA)->GetName() << "\"";
                    Console::Warning_End();
                }

                if (   ((*lItA)->GetType() == (*lItB)->GetType())
                    && (0 == strcmp((*lItA)->GetAddress(), (*lItB)->GetAddress())))
                {
                    Console::Warning_Begin();
                    std::cout << "Line " << (*lItA)->GetLineNo() << " and " << (*lItB)->GetLineNo();
                    std::cout << "  The addresses named \"" << (*lItA)->GetName() << "\" and \"" << (*lItB)->GetName();
                    std::cout << "\" are the same (" << (*lItA)->GetAddress() << ")";
                    Console::Warning_End();
                }
            }
        }

        Console::Progress_End("Verified");
    }

    // ===== List ===========================================================

    AddressList::~AddressList()
    {
        for (auto lAddress : mAddresses)
        {
            assert(NULL != lAddress);

            delete lAddress;
        }
    }

    bool AddressList::ImportSource(const char* aFileName)
    {
        Text::File_ASCII lFile;

        lFile.Read(File::Folder::CURRENT, aFileName);

        lFile.RemoveComments_Script();
        lFile.RemoveEmptyLines();

        bool lResult = false;

        for (const auto& lLine : lFile.mLines)
        {
            char lName[NAME_LENGTH];
            char lType[NAME_LENGTH];
            char lAddr[NAME_LENGTH];

            if (3 == sscanf_s(lLine.c_str(), "ADDRESS %[^ \n\r\t] %[^ \n\r\t] %[^ \n\r\t]", lName SizeInfo(lName), lType SizeInfo(lType), lAddr SizeInfo(lAddr)))
            {
                Enum<AddressType, ADDRESS_TYPE_NAMES> lTypeValue(lType);

                lResult |= ImportAddress(lName, lTypeValue, lAddr);
            }
            else
            {
                Console::Warning_Begin();
                std::cout << "Line " << lLine.GetUserLineNo() << " ignored - " << lLine.c_str();
                Console::Warning_End();
            }
        }

        return lResult;
    }

    void AddressList::ReadExported()
    {
        mFile_CSV.Read(File::Folder::CURRENT, GetExported());
    }

    void AddressList::SaveToImport()
    {
        const char* lToImport = GetToImport();

        mFile_CSV.Write(File::Folder::CURRENT, lToImport);

        GetSoftware()->ImportAddresses(lToImport, GetExported());
    }

    // Private
    // //////////////////////////////////////////////////////////////////////

    Address* AddressList::Find_ByName(const char* aName)
    {
        auto lIt = mAddresses_ByName.find(aName);
        if (mAddresses_ByName.end() == lIt)
        {
            return NULL;
        }

        assert(NULL != lIt->second);

        return lIt->second;
    }

    bool AddressList::ImportAddress(const char* aName, AddressType aType, const char* aAddr)
    {
        bool lResult = false;

        char lLine[LINE_LENGTH];

        auto lAddress = Find_ByName(aName);
        if (NULL == lAddress)
        {
            Console::Change("New address", aName);

            lResult = true;

            lAddress = new Address(aName, aType, aAddr, mFile_CSV.GetLineCount());

            lAddress->GetLine(lLine, sizeof(lLine));

            mAddresses.push_back(lAddress);

            mAddresses_ByName.insert(ByName::value_type(aName, lAddress));

            mFile_CSV.AddLine(lLine);
        }
        else
        {
            lResult = lAddress->Set(aType, aAddr);
            if (lResult)
            {
                // NOT TESTED
                lAddress->GetLine(lLine, sizeof(lLine));

                mFile_CSV.ReplaceLine(lAddress->GetLineNo(), lLine);
            }
        }

        return lResult;
    }

}
