
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/TRiLOGI/Software.h
// Status    TEST

#pragma once

namespace TRiLOGI
{

    class Software
    {

    public:

        Software();

        void Edit(const char* aFile);

        void Program(const char* aFile, const char* aIPAddress, uint8_t aId);

    };

}
