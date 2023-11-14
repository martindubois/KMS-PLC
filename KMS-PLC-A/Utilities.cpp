
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/Utilities.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "Utilities.h"

// Functions
// //////////////////////////////////////////////////////////////////////////

void Utl_RemoveSpecialChar(std::string* aInOut)
{
    assert(nullptr != aInOut);

    auto lLength = aInOut->size();

    for (unsigned int i = 0; i < lLength; i++)
    {
        auto lIn = (*aInOut)[i];

        switch (static_cast<uint8_t>(lIn))
        {
        case 0xa0:
        case 0xb0: (*aInOut)[i] = ' '; break;

        case 0xc7: (*aInOut)[i] = 'C'; break;

        case 0xc9: (*aInOut)[i] = 'E'; break;

        case 0xe0: (*aInOut)[i] = 'a'; break;

        case 0xe7: (*aInOut)[i] = 'c'; break;

        case 0xe8:
        case 0xe9:
        case 0xea: (*aInOut)[i] = 'e'; break;

        case 0xee: (*aInOut)[i] = 'i'; break;

        case 0xf4: (*aInOut)[i] = 'o'; break;

        case 0xfb: (*aInOut)[i] = 'u'; break;

        default:
            if (9 > lIn)
            {
                printf("Code ASCII %x\n", lIn);
                (*aInOut)[i] = ' ';
            }
        }
    }
}
