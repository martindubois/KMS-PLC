
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
        char        lLine[LINE_LENGTH];
        std::smatch lMatch;
        Parser      lParser(aSource);

        while (lParser.GetNextLine(lLine, sizeof(lLine)))
        {
            static const std::regex REGEX_DEFINE_0      ("^DEFINE (\\d+) (\\w+) (\\d+)$");
            static const std::regex REGEX_DEFINE_1      ("^DEFINE (\\d+) (\\w+) (\\w+)$");
            static const std::regex REGEX_DEFINE_2      ("^DEFINE (\\d+) (\\w+) (&h[0-9A-Fa-f]+)$");
            static const std::regex REGEX_DEFINE_3      ("^DEFINE (\\d+) (\\w+) (DM\\[\\d+\\])$");
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
            else if (std::regex_match(lLineStr, lMatch, REGEX_FUNCTION      )) { Add_FUNCTION      (lMatch, &lParser); }
            else if (std::regex_match(lLineStr, lMatch, REGEX_FUNCTION_LABEL)) { Add_FUNCTION_LABEL(lMatch); }
            else if (std::regex_match(lLineStr, lMatch, REGEX_INPUT         )) { Add_INPUT         (lMatch); }
            else if (std::regex_match(lLineStr, lMatch, REGEX_OUTPUT        )) { Add_OUTPUT        (lMatch); }
            else if (std::regex_match(lLineStr, lMatch, REGEX_RELAY         )) { Add_RELAY         (lMatch); }
            else if (std::regex_match(lLineStr, lMatch, REGEX_SEQUENCE      )) { Add_SEQUENCE      (lMatch); }
            else if (std::regex_match(lLineStr, lMatch, REGEX_TIMER         )) { Add_TIMER         (lMatch); }
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
        assert(nullptr != aParser);

        auto lNew = new Function(aMatch);

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
