
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// Product   KMS-PLC
// File      Common/HMI/Label.h

#pragma once

// ===== C++ ================================================================
#include <map>
#include <string>

namespace HMI
{

    class Label_State;

    class Label
    {

    public:

        Label();

        ~Label();

        void GetString(unsigned int aState, const char* aLanguage, std::string* aOut) const;

        void GetString(unsigned int aState, const char* aLanguage, std::wstring* aOut) const;

        void SetName(const std::wstring& aName);

        void SetStateCount(unsigned int aCount);

        void SetString(unsigned int aState, const char* aLanguage, const char* aString);

        void SetString(unsigned int aState, const char* aLanguage, const std::string& aString);

        void SetString(unsigned int aState, const char* aLanguage, const std::wstring& aString);

        void Read();
        
        void Write(std::ostream& aOut) const;

    private:

        std::string  mName;
        unsigned int mStateCount;
        Label_State* mStates;

    };

}
