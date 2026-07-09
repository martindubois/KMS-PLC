
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/PLC/Builder.h

#pragma once

// ===== C++ ================================================================
#include <regex>

// ===== Common =============================================================
#include "../Common/Builder.h"
#include "../Common/Parser.h"
#include "../Common/PLC/Define.h"
#include "../Common/PLC/Function.h"
#include "../Common/PLC/Sequence.h"
#include "../Common/PLC/Timer.h"

namespace PLC
{

    class Builder : public ::Builder
    {

    public:

        // ===== ::Builder ==================================================
        virtual ~Builder();
        virtual void Build() override;
        
    protected:

        Builder();

        virtual void Write() = 0;

        // ===== ::Builder ==================================================
        virtual void ReadSource(const char* aSource) override;

        Define_Map   mDefines;
        Define_List  mDefines_Auto;
        Function_Map mFunctions;
        Element_Map  mInputs;
        Element_Map  mOutputs;
        Element_Map  mRelays;
        Sequence_Map mSequences;
        Timer_Map    mTimers;

    private:

        void Add_DEFINE        (const std::smatch& aMatch);
        void Add_DEFINE_TAIL   (const std::smatch& aMatch);
        void Add_FUNCTION      (const std::smatch& aMatch, Parser* aParser);
        void Add_FUNCTION_LABEL(const std::smatch& aMatch);
        void Add_INPUT         (const std::smatch& aMatch);
        void Add_OUTPUT        (const std::smatch& aMatch);
        void Add_RELAY         (const std::smatch& aMatch);
        void Add_SEQUENCE      (const std::smatch& aMatch);
        void Add_TIMER         (const std::smatch& aMatch);

    };

}
