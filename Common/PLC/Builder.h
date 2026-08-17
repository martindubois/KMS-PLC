
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
#include "../Common/IndexMonitor.h"
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

        static constexpr unsigned int MONITOR_INPUT    = 0;
        static constexpr unsigned int MONITOR_FUNCTION = 1;
        static constexpr unsigned int MONITOR_OUTPUT   = 2;
        static constexpr unsigned int MONITOR_RELAY    = 3;
        static constexpr unsigned int MONITOR_SEQUENCE = 4;
        static constexpr unsigned int MONITOR_TIMER    = 5;
        static constexpr unsigned int MONITOR_WORD     = 6;

        Builder();

        void Merge_FUNCTIONS();

        virtual void Write() = 0;

        // ===== ::Builder ==================================================
        virtual void ReadSource(const char* aSource) override;

        Define_Map   mDefines;
        Define_List  mDefines_Auto;
        Function_Map mFunctions;
        Function_List mFunctions_Auto;
        Element_Map  mInputs;
        Element_Map  mOutputs;
        Element_Map  mRelays;
        Element_List mRelays_Auto;
        Sequence_Map mSequences;
        Timer_Map    mTimers;
        Timer_List   mTimers_Auto;

        IndexMonitor mIndexMonitors[7];

    private:

        void Add_DEFINE        (const std::smatch& aMatch);
        void Add_DEFINE_Tail   (const std::smatch& aMatch);
        void Add_FUNCTION      (const std::smatch& aMatch, Parser* aParser);
        void Add_FUNCTION_Tail (const std::smatch& aMatch, Parser* aParser);
        void Add_FUNCTION_LABEL(const std::smatch& aMatch);
        void Add_INPUT         (const std::smatch& aMatch);
        void Add_OUTPUT        (const std::smatch& aMatch);
        void Add_RELAY         (const std::smatch& aMatch);
        void Add_RELAY_Head    (const std::smatch& aMatch);
        void Add_RELAY_Tail    (const std::smatch& aMatch);
        void Add_SEQUENCE      (const std::smatch& aMatch);
        void Add_TIMER         (const std::smatch& aMatch);
        void Add_TIMER_Tail    (const std::smatch& aMatch);

    };

}
