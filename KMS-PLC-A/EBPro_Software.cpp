
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/EBPro_Software.cpp

#include "Component.h"

// ===== Import/Includes ====================================================
#include <KMS/Console/HumanScript.h>

// ===== Local ==============================================================
#include "../Common/EBPro/Software.h"

using namespace KMS;

namespace EBPro
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    Software::Software() : mProcess(nullptr) {}

    Software::~Software()
    {
        if (nullptr != mProcess)
        {
            mProcess->Detach();

            delete mProcess;
        }
    }

    void Software::ImportAddresses(const char* aImp, const char* aExp)
    {
        assert(nullptr != aImp);
        assert(nullptr != aExp);

        KMS::Console::HumanScript lHS;

        lHS.Begin("Tool \"EBPro\"");
        {
            lHS.Line("- Tab \"Project\" - Group \"Library\" - Click \"Address\" ==> \"Address Tag");
            lHS.Step("  Library\" dialog");
            lHS.Indent_Inc();
            lHS.Line("- Click \"Import CSV...\" ==> \"Open\" dialog");
            lHS.Indent_Inc();

            char lLine[LINE_LENGTH];
            sprintf_s(lLine, "Select \"%s\"", aImp);
            lHS.Line(lLine);

            lHS.Step("  and click \"Open\" ==> \"EasyBuilder Pro\" dialog");
            lHS.Step("- Click \"Yes\" ==> \"EasyBuilder Pro\" dialog");
            lHS.Step("- Click \"OK\"");
            lHS.Indent_Dec();
            lHS.Step("- Click \"Export CSV...\" ==> \"Open\" dialog");
            lHS.Indent_Inc();
            
            sprintf_s(lLine, "- Select \"%s\"", aExp);
            lHS.Line(lLine);

            lHS.Step("  and click \"Open\" ==> \"EasyBuilder Pro\" dialog");
            lHS.Step("- Click \"OK\"");
            lHS.Indent_Dec();
            lHS.Step("- Click \"Exit\"");
            lHS.Step("- Click \"OK\"");

            lHS.Wait();
        }
        lHS.End();
    }

    void Software::ImportFunctions(const char* aImp, const char* aExp)
    {
        assert(nullptr != aImp);
        assert(nullptr != aExp);

        KMS::Console::HumanScript lHS;

        lHS.Begin("Tool \"EBPro\"");
        {
            lHS.Line("- Tab \"Project\" - Group \"Library\" - Click \"Macro\" --> \"Macro Manager\"");
            lHS.Step("  dialog");
            lHS.Indent_Inc();
            lHS.Step("- Click \"Library...\" --> \"Macro Function Library\" dialog");
            lHS.Indent_Inc();
            lHS.Step("- Click \"Import...\" --> \"Open\" dialog");
            lHS.Indent_Inc();
            
            char lLine[LINE_LENGTH];
            sprintf_s(lLine, "- Select \"%s\"", aImp);
            lHS.Line(lLine);

            lHS.Step("  and click \"Open\"");
            lHS.Indent_Dec();
            lHS.Line("- Click \"Export...\" --> \"Open\" dialog");

            sprintf_s(lLine, "- Select \"%s\"", aExp);
            lHS.Line(lLine);

            lHS.Step("  and click \"Open\"");
            lHS.Indent_Dec();
            lHS.Step("- Click \"Exit\"");
            lHS.Indent_Dec();
            lHS.Step("- Click \"OK\"");

            lHS.Wait();
        }
        lHS.End();
    }

    void Software::ImportLabels(const char* aImp, const char* aExp)
    {
        assert(nullptr != aImp);
        assert(nullptr != aExp);

        KMS::Console::HumanScript lHS;

        lHS.Begin("Tool \"EBPro\"");
        {
            lHS.Line("- Tab \"Project\" - Group \"Library\" - Click \"Label\" ==> \"Label Tag Library\"");
            lHS.Step("  dialog");
            lHS.Indent_Inc();
            lHS.Step("- Click \"Load Label File...\" ==> \"Open\" dialog");
            lHS.Indent_Inc();

            char lLine[LINE_LENGTH];
            sprintf_s(lLine, "- Select \"%s\"", aImp);
            lHS.Line(lLine);

            lHS.Step("  and click \"Open\"");
            lHS.Indent_Dec();
            lHS.Step("- Click \"Save Label File...\" ==> \"Open\" dialog");

            sprintf_s(lLine, "- Select \"%s\"", aExp);
            lHS.Line(lLine);
            lHS.Step("  and click \"Open\"");
            lHS.Indent_Dec();
            lHS.Step("- Click \"OK\"");

            lHS.Wait();
        }
        lHS.End();
    }

    void Software::Open(const char* aFile)
    {
        if (nullptr != mProcess)
        {
            if (mProcess->IsRunning())
            {
                return;
            }

            delete mProcess;
        }

        mProcess = new Proc::Process(File::Folder::CURRENT, aFile);

        mProcess->SetVerb("open");
        mProcess->Start();
    }

}
