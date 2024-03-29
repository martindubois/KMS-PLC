
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/EBPro_AddressList.cpp

// TEST COVERAGE  2023-09-14  KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KMS/Convert.h>
#include <KMS/Enum.h>

// ===== Local ==============================================================
#include "../Common/EBPro/Software.h"

#include "../Common/EBPro/AddressList.h"

#include "EBPro/Address.h"

using namespace KMS;

namespace EBPro
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    AddressList::AddressList(Software* aSoftware) : List(aSoftware) {}

    void AddressList::ImportAddresses(const ::AddressList& aAL)
    {
        auto lChanged = false;

        if ((!aAL.empty()) && IsToImportConfigured())
        {
            // NOT TESTED
            for (auto& lA : aAL)
            {
                char lLine[LINE_LENGTH];

                auto lAddress = Find_ByName(lA.GetName());
                if (nullptr == lAddress)
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
            ::Console::Progress_Begin("EBPro", "Parsing", GetExported());
            {
                unsigned int lLineNo = 0;

                for (const auto lLine : mFile_CSV.mLines)
                {
                    auto lA = new Address(lLine.c_str(), lLineNo);

                    mAddresses.push_back(lA);

                    mAddresses_ByName.insert(ByName::value_type(lA->GetName(), lA));

                    lLineNo++;
                }
            }
            ::Console::Progress_End("Parsed");
        }
    }

    void AddressList::Verify() const
    {
        ::Console::Progress_Begin("EBPro", "Verifying addresses");
        {
            for (auto lItA = mAddresses.begin(); lItA != mAddresses.end(); lItA++)
            {
                // NOT TESTED
                for (auto lItB = lItA + 1; lItB != mAddresses.end(); lItB++)
                {
                    if (0 == strcmp((*lItA)->GetName(), (*lItB)->GetName()))
                    {
                        ::Console::Warning_Begin((*lItA)->GetLineNo(), (*lItB)->GetLineNo())
                            << "2 addresses are named \"" << (*lItA)->GetName() << "\" (NOT TESTED)";
                        ::Console::Warning_End();
                    }

                    if (   ((*lItA)->GetType() == (*lItB)->GetType())
                        && (0 == strcmp((*lItA)->GetAddress(), (*lItB)->GetAddress())))
                    {
                        ::Console::Warning_Begin((*lItA)->GetLineNo(), (*lItB)->GetLineNo())
                            << "The addresses named \"" << (*lItA)->GetName() << "\" and \"" << (*lItB)->GetName()
                            << "\" are the same (" << (*lItA)->GetAddress() << ") (NOT TESTED)";
                        ::Console::Warning_End();
                    }
                }
            }
        }
        ::Console::Progress_End("Verified");
    }

    // ===== List ===========================================================

    AddressList::~AddressList()
    {
        for (auto lAddress : mAddresses)
        {
            assert(nullptr != lAddress);

            delete lAddress;
        }
    }

    bool AddressList::ImportSource(const char* aFileName)
    {
        Text::File_ASCII lFile;

        lFile.Read(File::Folder::CURRENT, aFileName);

        lFile.RemoveComments_Script();
        lFile.RemoveEmptyLines();

        auto lResult = false;

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
                ::Console::Warning_IgnoredLine(lLine.GetUserLineNo(), lLine.c_str());
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
        assert(nullptr != aName);

        auto lIt = mAddresses_ByName.find(aName);
        if (mAddresses_ByName.end() == lIt)
        {
            return nullptr;
        }

        assert(nullptr != lIt->second);

        return lIt->second;
    }

    bool AddressList::ImportAddress(const char* aName, AddressType aType, const char* aAddr)
    {
        auto lResult = false;

        char lLine[LINE_LENGTH];

        auto lAddress = Find_ByName(aName);
        if (nullptr == lAddress)
        {
            ::Console::Change("New address", aName);

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
