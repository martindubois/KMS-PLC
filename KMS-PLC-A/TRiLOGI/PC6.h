
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/TRiLOGI/PC6.h

#pragma once

// Constants
// //////////////////////////////////////////////////////////////////////////

#define PC6_FILE_HEADER L"\x00f8\x00f5TRiLOGI Ver 5.0"

#define PC6_FUNCTION_MARK L"\xc8\r"

#define PC6_SECTION_END   L"~\r"
#define PC6_SECTION_END_C L'~'

#define PC6_SECTION_END_BREAKPOINT  L"~END_BREAKPOINTS~\r"
#define PC6_SECTION_END_CIRCUIT     L"~END_CIRCUIT~\r"
#define PC6_SECTION_END_CUSTFN      L"~END_CUSTFN~\r"
#define PC6_SECTION_END_CUSTFNLABEL L"~END_CUSTFNLABEL~\r"
#define PC6_SECTION_END_DEFINES     L"~END_DEFINES~\r"
#define PC6_SECTION_END_QUICKTAGS   L"~END_QUICKTAGS~\r"
