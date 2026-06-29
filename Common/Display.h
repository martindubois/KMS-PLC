
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2026 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/Display.h

#pragma once

// Functions
// //////////////////////////////////////////////////////////////////////////

extern void Display_Error(const char* aMsg, const char* aInfo = nullptr);

extern void Display_Error(const char* aMsg, const wchar_t* aInfo);

extern void Display_Warning(const char* aMsg, const char* aInfo = nullptr);

extern void Display_Warning(const char* aMsg, const wchar_t* aInfo);
