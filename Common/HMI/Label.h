
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/HMI/Label.h

#pragma once

// ===== C++ ================================================================
#include <string>
#include <vector>

class Parser;

namespace HMI
{

    class Label_State;

    class Label final
    {

    public:

        Label();

        ~Label();

        unsigned int GetStateCount() const;

        void GetName(std::string* aOut) const;

        void GetName(std::wstring* aOut) const;

        void GetString(unsigned int aState, const char* aLanguage, std::string* aOut) const;

        void GetString(unsigned int aState, const char* aLanguage, std::wstring* aOut) const;

        void SetName(const std::string& aName);

        void SetName(const std::wstring& aName);

        void SetStateCount(unsigned int aCount);

        void SetString(unsigned int aState, const char* aLanguage, const char* aString);

        void SetString(unsigned int aState, const char* aLanguage, const std::string& aString);

        void SetString(unsigned int aState, const char* aLanguage, const std::wstring& aString);

        void Clear();
        
        bool Read(Parser* aIn);

        void Write(std::ostream& aOut) const;

    private:

        void AddState(Label_State* aIn);

        const Label_State* GetState(unsigned int aState) const;

        Label_State* GetState(unsigned int aState);

        typedef std::vector<Label_State*> StateList;

        std::string mName;
        StateList   mStates;

    };

    // Public
    // //////////////////////////////////////////////////////////////////////

    inline void Label::GetName(std::string* aOut) const { *aOut = mName; }

    inline unsigned int Label::GetStateCount() const { return static_cast<unsigned int>(mStates.size()); }

    inline void Label::SetName(const std::string& aName) { mName = aName; }

}
