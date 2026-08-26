
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      PLC/Builder.cpp

#include "Component.h"

// ===== C++ ================================================================
#include <regex>

// ===== Local ==============================================================
#include "../Common/Display.h"
#include "../Common/Parser.h"
#include "../Common/PLC/PLC.h"

#include "../Common/PLC/Builder.h"

using namespace KMS;

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

static void Parse_FUNCTION(PLC::Function* aFunction, Parser* aParser);

namespace PLC
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    Builder::~Builder()
    {
        for (auto& lF : mFunctions)
        {
            assert(nullptr != lF.second);

            delete lF.second;
        }

        for (auto lF : mFunctions_Auto)
        {
            assert(nullptr != lF);

            delete lF;
        }
    }

    void Builder::Build()
    {
        ::Builder::Build();

        Write();
    }

    #define REGEX_NAME_C1    "(\\w+)"
    #define REGEX_NAME_20_C1 "(\\w{1,20})"

    void Builder::ReadSource(const char* aSource)
    {
        char        lLine[LINE_LENGTH];
        std::smatch lMatch;
        Parser      lParser(aSource);

        while (lParser.GetNextLine(lLine, sizeof(lLine)))
        {
            static const std::regex REGEX_DEFINE_0_C3      ("^DEFINE (\\d+) " REGEX_NAME_C1 " (\\d+)$");
            static const std::regex REGEX_DEFINE_1_C3      ("^DEFINE (\\d+) " REGEX_NAME_C1 " (\\w+)$");
            static const std::regex REGEX_DEFINE_2_C3      ("^DEFINE (\\d+) " REGEX_NAME_C1 " (&h[0-9A-Fa-f]+)$");
            static const std::regex REGEX_DEFINE_3_C4      ("^DEFINE (\\d+) " REGEX_NAME_C1 " (DM\\[(\\d+)\\])$");
            static const std::regex REGEX_DEFINE_AUTO_0_C2 ("^DEFINE AUTO " REGEX_NAME_C1 " (\\d+)$");
            static const std::regex REGEX_DEFINE_AUTO_1_C2 ("^DEFINE AUTO " REGEX_NAME_C1 " (\\w+)$");
            static const std::regex REGEX_DEFINE_AUTO_2_C2 ("^DEFINE AUTO " REGEX_NAME_C1 " (&h[0-9A-Fa-f]+)$");
            static const std::regex REGEX_DEFINE_AUTO_3_C3 ("^DEFINE AUTO " REGEX_NAME_C1 " (DM\\[(\\d+)\\])$");
            static const std::regex REGEX_DEFINE_TAIL_0_C2 ("^DEFINE TAIL " REGEX_NAME_C1 " (\\d+)$");
            static const std::regex REGEX_DEFINE_TAIL_1_C2 ("^DEFINE TAIL " REGEX_NAME_C1 " (\\w+)$");
            static const std::regex REGEX_DEFINE_TAIL_2_C2 ("^DEFINE TAIL " REGEX_NAME_C1 " (&h[0-9A-Fa-f]+)$");
            static const std::regex REGEX_DEFINE_TAIL_3_C3 ("^DEFINE TAIL " REGEX_NAME_C1 " (DM\\[(\\d+)\\])$");
            static const std::regex REGEX_FUNCTION_C1      ("^FUNCTION (\\d+)$");
            static const std::regex REGEX_FUNCTION_C2      ("^FUNCTION (\\d+) " REGEX_NAME_C1 "$");
            static const std::regex REGEX_FUNCTION_AUTO_C1 ("^FUNCTION AUTO " REGEX_NAME_C1 "$");
            static const std::regex REGEX_FUNCTION_TAIL_C1 ("^FUNCTION TAIL " REGEX_NAME_C1 "$");
            static const std::regex REGEX_FUNCTION_LABEL_C2("^FUNCTION_LABEL (\\d+) " REGEX_NAME_C1 "$");
            static const std::regex REGEX_INPUT_C2         ("^INPUT (\\d+) " REGEX_NAME_C1 "$");
            static const std::regex REGEX_OUTPUT_C2        ("^OUTPUT (\\d+) " REGEX_NAME_C1 "$");
            static const std::regex REGEX_RELAY_C2         ("^RELAY (\\d+) " REGEX_NAME_20_C1 "$");
            static const std::regex REGEX_RELAY_AUTO_C1    ("^RELAY AUTO " REGEX_NAME_20_C1 "$");
            static const std::regex REGEX_RELAY_HEAD_C1    ("^RELAY HEAD " REGEX_NAME_20_C1 "$");
            static const std::regex REGEX_RELAY_TAIL_C1    ("^RELAY TAIL " REGEX_NAME_20_C1 "$");
            static const std::regex REGEX_SEQUENCE_C3      ("^SEQUENCE (\\d+) " REGEX_NAME_C1 " (\\d+)$");
            static const std::regex REGEX_TIMER_C3         ("^TIMER (\\d+) " REGEX_NAME_C1 " (\\d+)$");
            static const std::regex REGEX_TIMER_AUTO_C2    ("^TIMER AUTO " REGEX_NAME_C1 " (\\d+)$");
            static const std::regex REGEX_TIMER_TAIL_C2    ("^TIMER TAIL " REGEX_NAME_C1 " (\\d+)$");

            std::string lLineStr(lLine);

            if (   std::regex_match(lLineStr, lMatch, REGEX_DEFINE_0_C3)
                || std::regex_match(lLineStr, lMatch, REGEX_DEFINE_1_C3)
                || std::regex_match(lLineStr, lMatch, REGEX_DEFINE_2_C3)
                || std::regex_match(lLineStr, lMatch, REGEX_DEFINE_3_C4))
            {
                Add_DEFINE(lMatch);
            }
            else if (  std::regex_match(lLineStr, lMatch, REGEX_DEFINE_AUTO_0_C2)
                    || std::regex_match(lLineStr, lMatch, REGEX_DEFINE_AUTO_1_C2)
                    || std::regex_match(lLineStr, lMatch, REGEX_DEFINE_AUTO_2_C2)
                    || std::regex_match(lLineStr, lMatch, REGEX_DEFINE_AUTO_3_C3)
                    || std::regex_match(lLineStr, lMatch, REGEX_DEFINE_TAIL_0_C2)
                    || std::regex_match(lLineStr, lMatch, REGEX_DEFINE_TAIL_1_C2)
                    || std::regex_match(lLineStr, lMatch, REGEX_DEFINE_TAIL_2_C2)
                    || std::regex_match(lLineStr, lMatch, REGEX_DEFINE_TAIL_3_C3))
            {
                Add_DEFINE_Tail(lMatch);
            }
            else if (std::regex_match(lLineStr, lMatch, REGEX_FUNCTION_C1)
                ||   std::regex_match(lLineStr, lMatch, REGEX_FUNCTION_C2))
            {
                Add_FUNCTION(lMatch, &lParser);
            }
            else if (std::regex_match(lLineStr, lMatch, REGEX_FUNCTION_AUTO_C1)
                ||   std::regex_match(lLineStr, lMatch, REGEX_FUNCTION_TAIL_C1))
            {
                Add_FUNCTION_Tail(lMatch, &lParser);
            }
            else if (std::regex_match(lLineStr, lMatch, REGEX_RELAY_AUTO_C1)
                ||   std::regex_match(lLineStr, lMatch, REGEX_RELAY_TAIL_C1))
            {
                Add_RELAY_Tail(lMatch);
            }
            else if (std::regex_match(lLineStr, lMatch, REGEX_TIMER_AUTO_C2)
                ||   std::regex_match(lLineStr, lMatch, REGEX_TIMER_TAIL_C2))
            {
                Add_TIMER_Tail(lMatch);
            }
            else if (std::regex_match(lLineStr, lMatch, REGEX_FUNCTION_LABEL_C2)) { Add_FUNCTION_LABEL(lMatch); }
            else if (std::regex_match(lLineStr, lMatch, REGEX_INPUT_C2         )) { Add_INPUT         (lMatch); }
            else if (std::regex_match(lLineStr, lMatch, REGEX_OUTPUT_C2        )) { Add_OUTPUT        (lMatch); }
            else if (std::regex_match(lLineStr, lMatch, REGEX_RELAY_C2         )) { Add_RELAY         (lMatch); }
            else if (std::regex_match(lLineStr, lMatch, REGEX_RELAY_HEAD_C1    )) { Add_RELAY_Head    (lMatch); }
            else if (std::regex_match(lLineStr, lMatch, REGEX_SEQUENCE_C3      )) { Add_SEQUENCE      (lMatch); }
            else if (std::regex_match(lLineStr, lMatch, REGEX_TIMER_C3         )) { Add_TIMER         (lMatch); }
            else
            {
                Display_Error("Invalid source line (NOT TESTED)", lLine);

                KMS_EXCEPTION(RESULT_INVALID_FORMAT, "Invalid source format (NOT TESTED)", lLine);
            }
        }
    }

    // Protected
    // //////////////////////////////////////////////////////////////////////

    Builder::Builder()
    {
        mIndexMonitors[MONITOR_INPUT   ].Init("INPUT"   , 0,    7);
        mIndexMonitors[MONITOR_FUNCTION].Init("FUNCTION", 0,  255);
        mIndexMonitors[MONITOR_OUTPUT  ].Init("OUTPUT"  , 0,    7);
        mIndexMonitors[MONITOR_RELAY   ].Init("RELAY"   , 0,  511);
        mIndexMonitors[MONITOR_SEQUENCE].Init("SEQUENCE", 0,    7);
        mIndexMonitors[MONITOR_TIMER   ].Init("TIMER"   , 0,   63);
        mIndexMonitors[MONITOR_WORD    ].Init("WORD"    , 1, 3999);

        AddSource(MAIN_TXT);
    }

    void Builder::Merge_FUNCTIONS()
    {
        for (auto lFunction : mFunctions_Auto)
        {
            auto lIndex = lFunction->GetIndex();

            auto [lIt, lRet] = mFunctions.insert(Function_Map::value_type(lIndex, lFunction));
            assert(lRet);
        }

        mFunctions_Auto.clear();
    }

    // Private
    // //////////////////////////////////////////////////////////////////////

    void Builder::Add_DEFINE(const std::smatch& aMatch)
    {
        Define lNew;

        lNew.SetIndexNameAndValue(aMatch);

        if (5 <= aMatch.size())
        {
            mIndexMonitors[MONITOR_WORD].MarkUsed(Convert::ToUInt32(aMatch[4].str().c_str()));
        }

        auto [lIt, lRet] = mDefines.insert(Define_Map::value_type(lNew.GetIndex(), lNew));
        KMS_EXCEPTION_ASSERT(lRet, RESULT_INVALID_INDEX, "Duplicated DEFINE index", lNew.GetName());
    }

    void Builder::Add_DEFINE_Tail(const std::smatch& aMatch)
    {
        Define lNew;

        lNew.SetNameAndValue(aMatch);

        if (4 <= aMatch.size())
        {
            mIndexMonitors[MONITOR_WORD].MarkUsed(Convert::ToUInt32(aMatch[3].str().c_str()));
        }

        mDefines_Auto.push_back(lNew);
    }

    void Builder::Add_FUNCTION(const std::smatch& aMatch, Parser* aParser)
    {
        auto lNew = new Function;

        lNew->SetIndexAndName(aMatch);

        mIndexMonitors[MONITOR_FUNCTION].MarkUsed(lNew->GetIndex());

        Parse_FUNCTION(lNew, aParser);

        auto [lIt, lRet] = mFunctions.insert(Function_Map::value_type(lNew->GetIndex(), lNew));
        KMS_EXCEPTION_ASSERT(lRet, RESULT_INVALID_INDEX, "Duplicated FUNCTION index", lNew->GetName());
    }

    void Builder::Add_FUNCTION_Tail(const std::smatch& aMatch, Parser* aParser)
    {
        auto lNew = new Function;

        lNew->SetName(aMatch);

        Parse_FUNCTION(lNew, aParser);

        mFunctions_Auto.push_back(lNew);
    }

    void Builder::Add_FUNCTION_LABEL(const std::smatch& aMatch)
    {
        auto lIndex = Convert::ToUInt16(aMatch[1].str().c_str());
        auto lName  = aMatch[2].str();

        auto lIt = mFunctions.find(lIndex);
        if (mFunctions.end() != lIt)
        {
            lIt->second->SetName(lName);
        }
        else
        {
            Display_Warning("Label for missing function", lName.c_str());
        }
    }

    void Builder::Add_INPUT(const std::smatch& aMatch)
    {
        Element lNew;
        
        lNew.SetIndexAndName(aMatch);

        mIndexMonitors[MONITOR_INPUT].MarkUsed(lNew.GetIndex());

        auto [lIt, lRet] = mInputs.insert(Element_Map::value_type(lNew.GetIndex(), lNew));
        KMS_EXCEPTION_ASSERT(lRet, RESULT_INVALID_INDEX, "Duplicated INPUT index", lNew.GetName());
    }

    void Builder::Add_OUTPUT(const std::smatch& aMatch)
    {
        Element lNew;
        
        lNew.SetIndexAndName(aMatch);

        mIndexMonitors[MONITOR_OUTPUT].MarkUsed(lNew.GetIndex());

        auto [lIt, lRet] = mOutputs.insert(Element_Map::value_type(lNew.GetIndex(), lNew));
        KMS_EXCEPTION_ASSERT(lRet, RESULT_INVALID_INDEX, "Duplicated OUTPUT index", lNew.GetName());
    }

    void Builder::Add_RELAY(const std::smatch& aMatch)
    {
        Element lNew;
        
        lNew.SetIndexAndName(aMatch);

        mIndexMonitors[MONITOR_RELAY].MarkUsed(lNew.GetIndex());

        auto [lIt, lRet] = mRelays.insert(Element_Map::value_type(lNew.GetIndex(), lNew));
        KMS_EXCEPTION_ASSERT(lRet, RESULT_INVALID_INDEX, "Duplicated RELAY index", lNew.GetName());
    }

    void Builder::Add_RELAY_Head(const std::smatch& aMatch)
    {
        Element lNew;

        lNew.SetName(aMatch);

        mRelays_Auto.push_front(lNew);
    }

    void Builder::Add_RELAY_Tail(const std::smatch& aMatch)
    {
        Element lNew;

        lNew.SetName(aMatch);

        mRelays_Auto.push_back(lNew);
    }

    void Builder::Add_SEQUENCE(const std::smatch& aMatch)
    {
        Sequence lNew;
        
        lNew.SetIndexNameAndValue(aMatch);

        mIndexMonitors[MONITOR_SEQUENCE].MarkUsed(lNew.GetIndex());

        auto [lIt, lRet] = mSequences.insert(Sequence_Map::value_type(lNew.GetIndex(), lNew));
        KMS_EXCEPTION_ASSERT(lRet, RESULT_INVALID_INDEX, "Duplicated SEQUENCE index", lNew.GetName());
    }

    void Builder::Add_TIMER(const std::smatch& aMatch)
    {
        Timer lNew;
        
        lNew.SetIndexNameAndValue(aMatch);

        mIndexMonitors[MONITOR_TIMER].MarkUsed(lNew.GetIndex());

        auto [lIt, lRet] = mTimers.insert(Timer_Map::value_type(lNew.GetIndex(), lNew));
        KMS_EXCEPTION_ASSERT(lRet, RESULT_INVALID_INDEX, "Duplicated TIMER index", lNew.GetName());
    }

    void Builder::Add_TIMER_Tail(const std::smatch& aMatch)
    {
        Timer lNew;

        lNew.SetNameAndValue(aMatch);

        mTimers_Auto.push_back(lNew);
    }
}

// Static functions
// //////////////////////////////////////////////////////////////////////////

void Parse_FUNCTION(PLC::Function* aFunction, Parser* aParser)
{
    assert(nullptr != aFunction);
    assert(nullptr != aParser);

    char lLine[LINE_LENGTH];

    while (aParser->GetNextLine_Code(lLine, sizeof(lLine)))
    {
        static const std::regex REGEX_FUNCTION_END("^FUNCTION_END$");

        std::string lLineStr(lLine);

        if (std::regex_match(lLineStr, REGEX_FUNCTION_END))
        {
            break;
        }

        aFunction->AddLine(lLineStr);
    }
}
