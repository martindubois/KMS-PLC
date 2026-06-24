
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      PLC/Builder.cpp

#include "Component.h"

// ===== C++ ================================================================
#include <regex>

// ===== Local ==============================================================
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

    void Builder::Build() { ReadSource(MAIN_TXT); }

    void Builder::ReadSource(const char* aSource)
    {
        static const std::regex sRegex_DEFINE_0      ("^DEFINE (\\d+) (\\w+) (\\d+)\r\n$");
        static const std::regex sRegex_DEFINE_1      ("^DEFINE (\\d+) (\\w+) (\\w+)\r\n$");
        static const std::regex sRegex_DEFINE_2      ("^DEFINE (\\d+) (\\w+) (&h[0-9A-Fa-f]+)\r\n$");
        static const std::regex sRegex_DEFINE_3      ("^DEFINE (\\d+) (\\w+) (DM\\[\\d+\\])\r\n$");
        static const std::regex sRegex_FUNCTION      ("^FUNCTION (\\d+)\r\n$");
        static const std::regex sRegex_FUNCTION_LABEL("^FUNCTION_LABEL (\\d+) (\\w+)\r\n$");
        static const std::regex sRegex_INPUT         ("^INPUT (\\d+) (\\w+)\r\n$");
        static const std::regex sRegex_OUTPUT        ("^OUTPUT (\\d+) (\\w+)\r\n$");
        static const std::regex sRegex_RELAY         ("^RELAY (\\d+) (\\w+)\r\n$");
        static const std::regex sRegex_SEQUENCE      ("^SEQUENCE (\\d+) (\\w+) (\\d+)\r\n$");
        static const std::regex sRegex_TIMER         ("^TIMER (\\d+) (\\w+) (\\d+)\r\n$");

        char        lLine[LINE_LENGTH];
        std::smatch lMatch;
        Parser      lParser(aSource);

        while (lParser.GetNextLine(lLine, sizeof(lLine)))
        {
            std::string lLineStr(lLine);

            if (   std::regex_match(lLineStr, lMatch, sRegex_DEFINE_0)
                || std::regex_match(lLineStr, lMatch, sRegex_DEFINE_1)
                || std::regex_match(lLineStr, lMatch, sRegex_DEFINE_2)
                || std::regex_match(lLineStr, lMatch, sRegex_DEFINE_3))
            {
                Add_DEFINE(lMatch);
            }
            else if (std::regex_match(lLineStr, lMatch, sRegex_FUNCTION      )) { Add_FUNCTION      (lMatch, &lParser); }
            else if (std::regex_match(lLineStr, lMatch, sRegex_FUNCTION_LABEL)) { Add_FUNCTION_LABEL(lMatch); }
            else if (std::regex_match(lLineStr, lMatch, sRegex_INPUT         )) { Add_INPUT         (lMatch); }
            else if (std::regex_match(lLineStr, lMatch, sRegex_OUTPUT        )) { Add_OUTPUT        (lMatch); }
            else if (std::regex_match(lLineStr, lMatch, sRegex_RELAY         )) { Add_RELAY         (lMatch); }
            else if (std::regex_match(lLineStr, lMatch, sRegex_SEQUENCE      )) { Add_SEQUENCE      (lMatch); }
            else if (std::regex_match(lLineStr, lMatch, sRegex_TIMER         )) { Add_TIMER         (lMatch); }
            else
            {
                KMS_EXCEPTION(RESULT_INVALID_FORMAT, "Invalid source format (NOT TESTED)", lLine);
            }
        }
    }

    // Protected
    // //////////////////////////////////////////////////////////////////////

    Builder::Builder() {}

    // Private
    // //////////////////////////////////////////////////////////////////////

    void Builder::Add_DEFINE(const std::smatch& aMatch)
    {
        Define lNew(aMatch);

        mDefines.insert(Define_List::value_type(lNew.GetIndex(), lNew));
    }

    void Builder::Add_FUNCTION(const std::smatch& aMatch, Parser* aParser)
    {
        static const std::regex sRegex_FUNCTION_END("^FUNCTION_END\r\n$");

        assert(nullptr != aParser);

        auto lNew = new Function(aMatch);

        char lLine[LINE_LENGTH];

        while (aParser->GetNextLine(lLine, sizeof(lLine)))
        {
            std::string lLineStr(lLine);

            if (std::regex_match(lLineStr, sRegex_FUNCTION_END))
            {
                break;
            }

            lNew->AddLine(lLineStr);
        }

        mFunctions.insert(Function_List::value_type(lNew->GetIndex(), lNew));
    }

    void Builder::Add_FUNCTION_LABEL(const std::smatch& aMatch)
    {
        auto lIndex = Convert::ToUInt16(aMatch[1].str().c_str());

        auto lIt = mFunctions.find(lIndex);
        if (mFunctions.end() != lIt)
        {
            lIt->second->SetName(aMatch[2].str());
        }
        else
        {
            // TODO  Warning
        }
    }

    void Builder::Add_INPUT(const std::smatch& aMatch)
    {
        Element lNew(aMatch);

        mInputs.insert(Element_List::value_type(lNew.GetIndex(), lNew));
    }

    void Builder::Add_OUTPUT(const std::smatch& aMatch)
    {
        Element lNew(aMatch);

        mOutputs.insert(Element_List::value_type(lNew.GetIndex(), lNew));
    }

    void Builder::Add_RELAY(const std::smatch& aMatch)
    {
        Element lNew(aMatch);

        mRelays.insert(Element_List::value_type(lNew.GetIndex(), lNew));
    }

    void Builder::Add_SEQUENCE(const std::smatch& aMatch)
    {
        Sequence lNew(aMatch);

        mSequences.insert(Sequence_List::value_type(lNew.GetIndex(), lNew));
    }

    void Builder::Add_TIMER(const std::smatch& aMatch)
    {
        Timer lNew(aMatch);

        mTimers.insert(Timer_List::value_type(lNew.GetIndex(), lNew));
    }

}
