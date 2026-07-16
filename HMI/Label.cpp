
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      HMI/Label.cpp

#include "Component.h"

// ===== C++ ================================================================
#include <codecvt>
#include <regex>

// ===== Local ==============================================================
#include "../Common/Parser.h"
#include "../Common/Text.h"

#include "../Common/HMI/Label.h"

using namespace KMS;

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

static void Escape(const char* aIn, char* aOut, unsigned int aOutSize_byte);

static void Unescape(const char* aIn, char* aOut, unsigned int aOutSize_byte);

namespace HMI
{

    class Label_State final
    {

    public:

        void GetString(const char* aLanguage, std::string* aOut) const;

        void SetString(const char* aLanguage, const std::string& aString);

        void Read(Parser* aIn);

        void Write(std::ostream& aOut) const;

    private:

        typedef std::map<std::string, std::string> StringMap;

        StringMap mStrings;

    };

    // Public
    // //////////////////////////////////////////////////////////////////////

    Label::Label() {}

    Label::~Label() { Clear(); }

    void Label::GetName(std::wstring* aOut) const
    {
        assert(nullptr != aOut);

        std::string lOut;

        GetName(&lOut);

        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> lConverter;

        *aOut = lConverter.from_bytes(lOut);
    }

    void Label::GetString(unsigned int aState, const char* aLanguage, std::string* aOut) const
    {
        auto lState = GetState(aState);

        lState->GetString(aLanguage, aOut);
    }

    void Label::GetString(unsigned int aState, const char* aLanguage, std::wstring* aOut) const
    {
        assert(nullptr != aOut);

        std::string lOut;

        GetString(aState, aLanguage, &lOut);

        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> lConverter;

        *aOut = lConverter.from_bytes(lOut);
    }

    void Label::SetName(const std::wstring& aName)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> lConverter;

        SetName(lConverter.to_bytes(aName));
    }

    void Label::SetStateCount(unsigned int aCount)
    {
        assert(0 < aCount);

        while (mStates.size() < aCount)
        {
            AddState(new Label_State());
        }
    }

    void Label::SetString(unsigned int aState, const char* aLanguage, const char* aString)
    {
        auto lState = GetState(aState);

        lState->SetString(aLanguage, aString);
    }

    void Label::SetString(unsigned int aState, const char* aLanguage, const std::string& aString)
    {
        auto lState = GetState(aState);

        lState->SetString(aLanguage, aString);
    }

    void Label::SetString(unsigned int aState, const char* aLanguage, const std::wstring& aString)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> lConverter;

        SetString(aState, aLanguage, lConverter.to_bytes(aString));
    }

    void Label::Clear()
    {
        for (auto lState : mStates)
        {
            assert(nullptr != lState);

            delete lState;
        }

        mName.clear();
        mStates.clear();
    }

    bool Label::Read(Parser* aIn)
    {
        assert(nullptr != aIn);

        char lLine[LINE_LENGTH];

        bool lResult = aIn->GetNextLine(lLine, sizeof(lLine));
        if (lResult)
        {
            char lName[NAME_LENGTH];

            auto lRet = sscanf_s(lLine, "LABEL %[^\n\r]", lName SizeInfo(lName));
            KMS_EXCEPTION_ASSERT(1 == lRet, RESULT_INVALID_FORMAT, "Invalid input line", lLine);

            SetName(lName);

            while (aIn->GetNextLine(lLine, sizeof(lLine)))
            {
                if (0 == strncmp(lLine, "END", 3))
                {
                    break;
                }

                if (0 == strncmp(lLine, "STATE", 5))
                {
                    auto lState = new Label_State;

                    lState->Read(aIn);

                    AddState(lState);
                }
                else
                {
                    KMS_EXCEPTION(RESULT_INVALID_FORMAT, "Invalid input line", lLine);
                }
            }
        }

        return lResult;
    }

    void Label::Write(std::ostream& aOut) const
    {
        aOut << "LABEL " << mName << Text_EOL;

        for (auto lState : mStates)
        {
            assert(nullptr != lState);

            lState->Write(aOut);
        }

        aOut << "END\r\n";
    }

    // Private
    // //////////////////////////////////////////////////////////////////////

    void Label::AddState(Label_State* aState)
    {
        assert(nullptr != aState);

        mStates.push_back(aState);
    }

    const Label_State* Label::GetState(unsigned int aState) const
    {
        assert(mStates.size() > aState);

        auto lResult = mStates[aState];
        assert(nullptr != lResult);

        return lResult;
    }

    Label_State* Label::GetState(unsigned int aState)
    {
        assert(mStates.size() > aState);

        auto lResult = mStates[aState];
        assert(nullptr != lResult);

        return lResult;
    }

    // Internal
    // //////////////////////////////////////////////////////////////////////

    void Label_State::GetString(const char* aLanguage, std::string* aOut) const
    {
        assert(nullptr != aLanguage);
        assert(nullptr != aOut);

        auto lIt = mStrings.find(aLanguage);
        KMS_EXCEPTION_ASSERT(mStrings.end() != lIt, RESULT_INVALID_KEY, "The language is not supported", aLanguage);

        *aOut = lIt->second;
    }

    void Label_State::SetString(const char* aLanguage, const std::string& aString)
    {
        assert(nullptr != aLanguage);

        mStrings.insert(StringMap::value_type(aLanguage, aString));
    }

    void Label_State::Read(Parser* aIn)
    {
        assert(nullptr != aIn);

        char lLine  [LINE_LENGTH];
        char lString[LINE_LENGTH];

        std::smatch lMatch;

        while (aIn->GetNextLine(lLine, sizeof(lLine)))
        {
            static const std::regex REGEX("^([a-z]{2}) (.+)$");
            static const std::regex REGEX_EMPTY("^([a-z]{2})\\s{0,1}$");

            if (0 == strncmp("END", lLine, 3))
            {
                break;
            }

            std::string lLineStr(lLine);

            if (std::regex_match(lLineStr, lMatch, REGEX_EMPTY))
            {
                strcpy_s(lString, "");
            }
            else if (std::regex_match(lLineStr, lMatch, REGEX))
            {
                Unescape(lMatch[2].str().c_str(), lString, sizeof(lString));
            }
            else
            {
                KMS_EXCEPTION(RESULT_INVALID_FORMAT, "Invalid input line", lLine);
            }

            SetString(lMatch[1].str().c_str(), lString);
        }
    }

    void Label_State::Write(std::ostream& aOut) const
    {
        aOut << "    STATE\r\n";

        for (auto& lPair : mStrings)
        {
            char lString[LINE_LENGTH];

            Escape(lPair.second.c_str(), lString, sizeof(lString));

            aOut << "        " << lPair.first << " " << lString << Text_EOL;
        }

        aOut << "    END\r\n";
    }

}

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

void Escape(const char* aIn, char* aOut, unsigned int aOutSize_byte)
{
    unsigned int lIn = 0;
    unsigned int lOut = 0;

    while (lOut < aOutSize_byte)
    {
        switch (aIn[lIn])
        {
        case '\0':
            aOut[lOut] = aIn[lIn];
            return;

        case '\n':
            aOut[lOut] = '\\'; lOut++;
            aOut[lOut] = 'n'; lOut++;
            break;

        case '\r':
            aOut[lOut] = '\\'; lOut++;
            aOut[lOut] = 'r'; lOut++;
            break;

        case '\\':
            aOut[lOut] = '\\'; lOut++;
            aOut[lOut] = '\\'; lOut++;
            break;

        default:
            aOut[lOut] = aIn[lIn]; lOut++;
        }

        lIn++;
    }

    KMS_EXCEPTION(RESULT_OUTPUT_TOO_SHORT, "String is too long", aIn);
}

void Unescape(const char* aIn, char* aOut, unsigned int aOutSize_byte)
{
    unsigned int lIn = 0;
    unsigned int lOut = 0;

    while (lOut < aOutSize_byte)
    {
        switch (aIn[lIn])
        {
        case '\0':
            aOut[lOut] = aIn[lIn];
            return;

        case '\\':
            lIn++;
            switch (aIn[lIn])
            {
            case '\\': aOut[lOut] = '\\'; break;
            case 'n': aOut[lOut] = '\n'; break;
            case 'r': aOut[lOut] = '\r'; break;

            default: KMS_EXCEPTION(RESULT_INVALID_FORMAT, "Invalid escape sequence", aIn);
            }

            lOut++;
            break;

        default:
            aOut[lOut] = aIn[lIn]; lOut++;
        }

        lIn++;
    }

    KMS_EXCEPTION(RESULT_OUTPUT_TOO_SHORT, "String is too long", aIn);
}
