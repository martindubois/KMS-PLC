
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// Product   KMS-PLC
// File      HMI/Label.cpp

#include "Component.h"

// ===== C++ ================================================================
#include <codecvt>

// ===== Import/Includes ====================================================
#include <KMS/Exception.h>

// ===== Local ==============================================================
#include "../Common/HMI/Label.h"

using namespace KMS;

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

static void Escape(const char* aIn, char* aOut, unsigned int aOutSize_byte);

static void Unescape(const char* aIn, char* aOut, unsigned int aOutSize_byte);

namespace HMI
{

    class Label_State
    {

    public:

        void GetString(const char* aLanguage, std::string* aOut) const;

        void SetString(const char* aLanguage, const std::string& aString);

        void Read();

        void Write(std::ostream& aOut) const;

    private:

        typedef std::map<std::string, std::string> StringMap;

        StringMap mStrings;

    };

    // Public
    // //////////////////////////////////////////////////////////////////////

    Label::Label() : mStateCount(0), mStates(nullptr) {}

    Label::~Label()
    {
        if (nullptr != mStates)
        {
            delete[] mStates;
        }
    }

    void Label::GetString(unsigned int aState, const char* aLanguage, std::string* aOut) const
    {
        auto lState = mStates + aState;

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

        mName = lConverter.to_bytes(aName);
    }

    void Label::SetStateCount(unsigned int aCount)
    {
        assert(0 < aCount);

        assert(0 == mStateCount);
        assert(nullptr == mStates);

        mStateCount = aCount;
        mStates = new Label_State[mStateCount];
    }

    void Label::SetString(unsigned int aState, const char* aLanguage, const char* aString)
    {
        auto lState = mStates + aState;

        lState->SetString(aLanguage, aString);
    }

    void Label::SetString(unsigned int aState, const char* aLanguage, const std::string& aString)
    {
        auto lState = mStates + aState;

        lState->SetString(aLanguage, aString);
    }

    void Label::SetString(unsigned int aState, const char* aLanguage, const std::wstring& aString)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> lConverter;

        SetString(aState, aLanguage, lConverter.to_bytes(aString));
    }

    void Label::Write(std::ostream& aOut) const
    {
        aOut << "LABEL " << mName << "\n";

        for (unsigned int i = 0; i < mStateCount; i++)
        {
            auto lState = mStates + i;

            lState->Write(aOut);
        }

        aOut << "END\n";
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

    void Label_State::Write(std::ostream& aOut) const
    {
        aOut << "    STATE\n";

        for (auto& lPair : mStrings)
        {
            char lString[LINE_LENGTH];

            Escape(lPair.second.c_str(), lString, sizeof(lString));

            aOut << "        " << lPair.first << "  " << lString << "\n";
        }

        aOut << "    END\n";
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

        default:
            aOut[lOut] = aIn[lIn]; lOut++;
        }

        lIn++;
    }

    KMS_EXCEPTION(RESULT_OUTPUT_TOO_SHORT, "String is too long", aIn);
}
