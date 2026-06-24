
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2024 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/TRiLOGI/PC6.h
// Status    DEV

#pragma once

// ===== C++ ================================================================
#include <codecvt>

namespace TRiLOGI
{

    static const wchar_t* PC6_BEGIN_FUNCTION   = L"\xc8";
    static const wchar_t* PC6_BEGIN_FUNCTION_R = L"\xc8\r";

    static const wchar_t* PC6_END                  = L"~";
    static const wchar_t* PC6_END_R                = L"~\r";
    static const wchar_t* PC6_END_BREAKPOINT       = L"~END_BREAKPOINTS~";
    static const wchar_t* PC6_END_BREAKPOINT_R     = L"~END_BREAKPOINTS~\r";
    static const wchar_t* PC6_END_CIRCUIT_R        = L"~END_CIRCUIT~\r";
    static const wchar_t* PC6_END_DEFINE           = L"~END_DEFINES~";
    static const wchar_t* PC6_END_DEFINE_R         = L"~END_DEFINES~\r";
    static const wchar_t* PC6_END_FUNCTION         = L"~END_CUSTFN~";
    static const wchar_t* PC6_END_FUNCTION_R       = L"~END_CUSTFN~\r";
    static const wchar_t* PC6_END_FUNCTION_LABEL   = L"~END_CUSTFNLABEL~";
    static const wchar_t* PC6_END_FUNCTION_LABEL_R = L"~END_CUSTFNLABEL~\r";
    static const wchar_t* PC6_END_LAST_IP_ADDR     = L"~END_LASTIPADDR~";
    static const wchar_t* PC6_END_LAST_IP_ADDR_R   = L"~END_LASTIPADDR~\r";
    static const wchar_t* PC6_END_TAG              = L"~END_QUICKTAGS~";
    static const wchar_t* PC6_END_TAG_R            = L"~END_QUICKTAGS~\r";

    static const wchar_t* PC6_HEADER = L"\xf8\xf5TRiLOGI Ver 5.0";

    inline void ConfigStream(std::wifstream& aIn)
    {
        aIn.imbue(std::locale(aIn.getloc(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::consume_header>));
    }

    inline void ConfigStream(std::wofstream& aOut)
    {
        aOut.imbue(std::locale(aOut.getloc(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::generate_header>));
    }

}
