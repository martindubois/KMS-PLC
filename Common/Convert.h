
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022-2206 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      Common/Convert.h

#pragma once

// Functions
// //////////////////////////////////////////////////////////////////////////

extern void ToASCII  (const std::wstring& aIn, char   * aOut, unsigned int aOutSize_byte);
extern void ToASCII  (const std::wstring& aIn, std::string * aOut);
extern void ToUnicode(const std::string & aIn, wchar_t* aOut, unsigned int aOutSize_byte);
extern void ToUnicode(const std::string & aIn, std::wstring* aOut);

extern void ToExportableString(const wchar_t* aIn, wchar_t* aOut);
extern void ToImportableString(const wchar_t* aIn, wchar_t* aOut);
