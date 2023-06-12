
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/EBPro/LabelList.h

#pragma once

// ===== C++ ================================================================
#include <vector>

// ===== Import =============================================================
#include <KMS/Text/File_ASCII.h>

// ===== Local ==============================================================
#include "../Address.h"

#include "List.h"

namespace EBPro
{

    class Address;

    class AddressList : public List
    {

    public:

        typedef std::map<std::string, Address*> ByName;

        typedef std::vector<Address*> Internal;

        AddressList(Software* aSoftware);

        void ImportAddresses(const ::AddressList& aAL);

        void Parse();

        void Verify() const;

        // ===== List =======================================================
        virtual ~AddressList();

        Internal mAddresses;

        ByName mAddresses_ByName;

    protected:

        // ===== List =======================================================
        virtual bool ImportSource(const char* aFileName);
        virtual void ReadExported();
        virtual void SaveToImport();

    private:

        NO_COPY(AddressList);

        Address* Find_ByName(const char* aName);

        bool ImportAddress(const char* aName, AddressType aType, const char* aAddr);

        KMS::Text::File_ASCII mFile_CSV;

    };

}
