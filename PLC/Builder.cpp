
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
    }

    void Builder::Build()
    {
        ::Builder::Build();

        Write();
    }

    void Builder::ReadSource(const char* aSource)
    {
        char        lLine[LINE_LENGTH];
        std::smatch lMatch;
        Parser      lParser(aSource);

        while (lParser.GetNextLine(lLine, sizeof(lLine)))
        {
            static const std::regex REGEX_DEFINE_0      ("^DEFINE (\\d+) (\\w+) (\\d+)$");
            static const std::regex REGEX_DEFINE_1      ("^DEFINE (\\d+) (\\w+) (\\w+)$");
            static const std::regex REGEX_DEFINE_2      ("^DEFINE (\\d+) (\\w+) (&h[0-9A-Fa-f]+)$");
            static const std::regex REGEX_DEFINE_3      ("^DEFINE (\\d+) (\\w+) (DM\\[\\d+\\])$");
            static const std::regex REGEX_DEFINE_AUTO_0 ("^DEFINE AUTO (\\w+) (\\d+)$");
            static const std::regex REGEX_DEFINE_AUTO_1 ("^DEFINE AUTO (\\w+) (\\w+)$");
            static const std::regex REGEX_DEFINE_AUTO_2 ("^DEFINE AUTO (\\w+) (&h[0-9A-Fa-f]+)$");
            static const std::regex REGEX_DEFINE_AUTO_3 ("^DEFINE AUTO (\\w+) (DM\\[\\d+\\])$");
            static const std::regex REGEX_DEFINE_TAIL_0 ("^DEFINE TAIL (\\w+) (\\d+)$");
            static const std::regex REGEX_DEFINE_TAIL_1 ("^DEFINE TAIL (\\w+) (\\w+)$");
            static const std::regex REGEX_DEFINE_TAIL_2 ("^DEFINE TAIL (\\w+) (&h[0-9A-Fa-f]+)$");
            static const std::regex REGEX_DEFINE_TAIL_3 ("^DEFINE TAIL (\\w+) (DM\\[\\d+\\])$");
            static const std::regex REGEX_FUNCTION      ("^FUNCTION (\\d+)$");
            static const std::regex REGEX_FUNCTION_LABEL("^FUNCTION_LABEL (\\d+) (\\w+)$");
            static const std::regex REGEX_INPUT         ("^INPUT (\\d+) (\\w+)$");
            static const std::regex REGEX_OUTPUT        ("^OUTPUT (\\d+) (\\w+)$");
            static const std::regex REGEX_RELAY         ("^RELAY (\\d+) (\\w+)$");
            static const std::regex REGEX_SEQUENCE      ("^SEQUENCE (\\d+) (\\w+) (\\d+)$");
            static const std::regex REGEX_TIMER         ("^TIMER (\\d+) (\\w+) (\\d+)$");

            std::string lLineStr(lLine);

            if (   std::regex_match(lLineStr, lMatch, REGEX_DEFINE_0)
                || std::regex_match(lLineStr, lMatch, REGEX_DEFINE_1)
                || std::regex_match(lLineStr, lMatch, REGEX_DEFINE_2)
                || std::regex_match(lLineStr, lMatch, REGEX_DEFINE_3))
            {
                Add_DEFINE(lMatch);
            }
            else if (  std::regex_match(lLineStr, lMatch, REGEX_DEFINE_AUTO_0)
                    || std::regex_match(lLineStr, lMatch, REGEX_DEFINE_AUTO_1)
                    || std::regex_match(lLineStr, lMatch, REGEX_DEFINE_AUTO_2)
                    || std::regex_match(lLineStr, lMatch, REGEX_DEFINE_AUTO_3)
                    || std::regex_match(lLineStr, lMatch, REGEX_DEFINE_TAIL_0)
                    || std::regex_match(lLineStr, lMatch, REGEX_DEFINE_TAIL_1)
                    || std::regex_match(lLineStr, lMatch, REGEX_DEFINE_TAIL_2)
                    || std::regex_match(lLineStr, lMatch, REGEX_DEFINE_TAIL_3))
            {
                Add_DEFINE_TAIL(lMatch);
            }
            else if (std::regex_match(lLineStr, lMatch, REGEX_FUNCTION      )) { Add_FUNCTION      (lMatch, &lParser); }
            else if (std::regex_match(lLineStr, lMatch, REGEX_FUNCTION_LABEL)) { Add_FUNCTION_LABEL(lMatch); }
            else if (std::regex_match(lLineStr, lMatch, REGEX_INPUT         )) { Add_INPUT         (lMatch); }
            else if (std::regex_match(lLineStr, lMatch, REGEX_OUTPUT        )) { Add_OUTPUT        (lMatch); }
            else if (std::regex_match(lLineStr, lMatch, REGEX_RELAY         )) { Add_RELAY         (lMatch); }
            else if (std::regex_match(lLineStr, lMatch, REGEX_SEQUENCE      )) { Add_SEQUENCE      (lMatch); }
            else if (std::regex_match(lLineStr, lMatch, REGEX_TIMER         )) { Add_TIMER         (lMatch); }
            else
            {
                Display_Error("Invalid source line (NOT TESTED)", lLine);

                KMS_EXCEPTION(RESULT_INVALID_FORMAT, "Invalid source format (NOT TESTED)", lLine);
            }
        }
    }

    // Protected
    // //////////////////////////////////////////////////////////////////////

    Builder::Builder() { AddSource(MAIN_TXT); }

    // Private
    // //////////////////////////////////////////////////////////////////////

    void Builder::Add_DEFINE(const std::smatch& aMatch)
    {
        Define lNew;

        lNew.SetIndexNameAndValue(aMatch);

        mDefines.insert(Define_Map::value_type(lNew.GetIndex(), lNew));
    }

    void Builder::Add_DEFINE_TAIL(const std::smatch& aMatch)
    {
        Define lNew;

        lNew.SetNameAndValue(aMatch);

        mDefines_Auto.push_back(lNew);
    }

    void Builder::Add_FUNCTION(const std::smatch& aMatch, Parser* aParser)
    {
        assert(nullptr != aParser);

        auto lNew = new Function;

        lNew->SetIndexAndName(aMatch);

        char lLine[LINE_LENGTH];

        while (aParser->GetNextLine_Code(lLine, sizeof(lLine)))
        {
            static const std::regex REGEX_FUNCTION_END("^FUNCTION_END$");

            std::string lLineStr(lLine);

            if (std::regex_match(lLineStr, REGEX_FUNCTION_END))
            {
                break;
            }

            lNew->AddLine(lLineStr);
        }

        mFunctions.insert(Function_Map::value_type(lNew->GetIndex(), lNew));
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
            Display_Warning("Label for missing funciton", lName.c_str());
        }
    }

    void Builder::Add_INPUT(const std::smatch& aMatch)
    {
        Element lNew;
        
        lNew.SetIndexAndName(aMatch);

        mInputs.insert(Element_Map::value_type(lNew.GetIndex(), lNew));
    }

    void Builder::Add_OUTPUT(const std::smatch& aMatch)
    {
        Element lNew;
        
        lNew.SetIndexAndName(aMatch);

        mOutputs.insert(Element_Map::value_type(lNew.GetIndex(), lNew));
    }

    void Builder::Add_RELAY(const std::smatch& aMatch)
    {
        Element lNew;
        
        lNew.SetIndexAndName(aMatch);

        mRelays.insert(Element_Map::value_type(lNew.GetIndex(), lNew));
    }

    void Builder::Add_SEQUENCE(const std::smatch& aMatch)
    {
        Sequence lNew;
        
        lNew.SetIndexNameAndValue(aMatch);

        mSequences.insert(Sequence_Map::value_type(lNew.GetIndex(), lNew));
    }

    void Builder::Add_TIMER(const std::smatch& aMatch)
    {
        Timer lNew;
        
        lNew.SetIndexNameAndValue(aMatch);

        mTimers.insert(Timer_Map::value_type(lNew.GetIndex(), lNew));
    }

}
