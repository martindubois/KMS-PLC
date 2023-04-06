
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/EBPro/LabelList.h

#pragma once

// ===== C++ ================================================================
#include <vector>

// ===== Import =============================================================
#include <KMS/DI/Array.h>
#include <KMS/DI/Dictionary.h>
#include <KMS/DI/String_Expand.h>
#include <KMS/Text/File_ASCII.h>

// ===== Local ==============================================================
#include "../Address.h"

namespace EBPro
{

    class Address;

    class AddressList : public KMS::DI::Dictionary
    {

    public:

        typedef std::map<std::string, Address*> ByName;

        typedef std::vector<Address*> Internal;

        AddressList();

        ~AddressList();

        void Import();

        void Import(const ::AddressList& aAL);

        void Parse();

        void Read();

        void ValidateConfig() const;

        void Verify() const;

        Internal mAddresses;

        ByName mAddresses_ByName;

    private:

        AddressList(const AddressList&);

        const AddressList& operator = (const AddressList&);

        Address* Find_ByName(const char* aName);

        bool Import(const char* aName, AddressType aType, uint16_t aAddr);

        KMS::Text::File_ASCII mFile_CSV;

        // ===== Configurable attributes ====================================
        KMS::DI::String_Expand mExported;
        KMS::DI::Array         mSources;
        KMS::DI::String_Expand mToImport;

    };

}
