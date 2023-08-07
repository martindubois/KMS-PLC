
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2023 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/EBPro_Software.cpp

#include "Component.h"

// ===== Local ==============================================================
#include "../Common/EBPro/Software.h"

using namespace KMS;

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

static void Instruction_Begin();
static void Instruction_End  ();

namespace EBPro
{

    // Public
    // //////////////////////////////////////////////////////////////////////

    Software::Software() : mProcess(NULL) {}

    Software::~Software()
    {
        if (NULL != mProcess)
        {
            mProcess->Detach();

            delete mProcess;
        }
    }

    void Software::ImportAddresses(const char* aImp, const char* aExp)
    {
        assert(NULL != aImp);
        assert(NULL != aExp);

        Instruction_Begin();

        //                                   1           2          3          4          5          6          7
        //                          1234567890 12345678 9012345678 90123456 7890123456 78901234 567890 123456789012
        gConsole.OutputStream() << "    - Tab \"Project\" - Group \"Library\" - Click \"Address\" ==> \"Address Tag\n";
        //                          1234567890123 4567890123456789012345678901234567890123456789012345678901234567
        gConsole.OutputStream() << "      Library\" dialog                                                     [ ]\n";
        //                          1234567890123456 78901234567890 123456 78901 234567890123456789012345678901234567
        gConsole.OutputStream() << "        - Click \"Import CSV...\" ==> \"Open\" dialog                         [ ]\n";
        gConsole.OutputStream() << "            - Select \"" << aImp << "\"\n";
        //                          123456789012345678901234 56789 012345 6789012345678901 23456789012345678901234567
        gConsole.OutputStream() << "              and click \"Open\" ==> \"EasyBuilder Pro\" dialog               [ ]\n";
        //                          12345678901234567890 1234 567890 1234567890123456 7890123456789012345678901234567
        gConsole.OutputStream() << "            - Click \"Yes\" ==> \"EasyBuilder Pro\" dialog                    [ ]\n";
        //                          12345678901234567890 123 456789012345678901234567890123456789012345678901234567
        gConsole.OutputStream() << "            - Click \"OK\"                                                  [ ]\n";
        //                          1234567890123456 78901234567890 123456 78901 234567890123456789012345678901234567
        gConsole.OutputStream() << "        - Click \"Export CSV...\" ==> \"Open\" dialog                         [ ]\n";
        gConsole.OutputStream() << "            - Select \"" << aExp << "\"\n";
        //                          123456789012345678901234 56789 012345 6789012345678901 23456789012345678901234567
        gConsole.OutputStream() << "              and click \"Open\" ==> \"EasyBuilder Pro\" dialog               [ ]\n";
        //                          12345678901234567890 123 456789012345678901234567890123456789012345678901234567
        gConsole.OutputStream() << "            - Click \"OK\"                                                  [ ]\n";
        //                          1234567890123456 78901 23456789012345678901234567890123456789012345678901234567
        gConsole.OutputStream() << "        - Click \"Exit\"                                                    [ ]\n";

        Instruction_End();
    }

    void Software::ImportFunctions(const char* aImp, const char* aExp)
    {
        assert(NULL != aImp);
        assert(NULL != aExp);

        Instruction_Begin();

        //                                   1           2          3          4          5           6         7
        //                          1234567890 12345678 9012345678 90123456 7890123456 789012 345678 90123456789012 3
        gConsole.OutputStream() << "    - Tab \"Project\" - Group \"Library\" - Click \"Macro\" --> \"Macro Manager\"\n";
        //                          12345678901234567890123456789012345678901234567890123456789012345678901234567
        gConsole.OutputStream() << "      dialog                                                              [ ]\n";
        //                          1234567890123456 78901234567 890 12 34567890123456789012345 6789012345678901234567
        gConsole.OutputStream() << "        - Click \"Library...\" --> \"Macro Function Library\" dialog           [ ]\n";
        //                          12345678901234567890 1234567890 12 345 67890 123456789012345 6789012345678901234567
        gConsole.OutputStream() << "            - Click \"Import...\" --> \"Open\" dialog                           [ ]\n";
        gConsole.OutputStream() << "                - Select \"" << aImp << "\"\n";
        //                          1234567890123456789012345678 90123 45678901234567890123456789012345678901234567
        gConsole.OutputStream() << "                  and click \"Open\"                                        [ ]\n";
        //                          12345678901234567890 1234567890 123456 78901 234567890123456789012345678901234567
        gConsole.OutputStream() << "            - Click \"Export...\" --> \"Open\" dialog                         [ ]\n";
        gConsole.OutputStream() << "                - Select \"" << aExp << "\"\n";
        //                          1234567890123456789012345678 90123 45678901234567890123456789012345678901234567
        gConsole.OutputStream() << "                  and click \"Open\"                                        [ ]\n";
        //                          12345678901234567890 12345 6789012345678901234567890123456789012345678901234567
        gConsole.OutputStream() << "            - Click \"Exit\"                                                [ ]\n";

        Instruction_End();
    }

    void Software::ImportLabels(const char* aImp, const char* aExp)
    {
        assert(NULL != aImp);
        assert(NULL != aExp);

        Instruction_Begin();

        //                                   1           2          3          4          5           6         7
        //                          1234567890 12345678 9012345678 90123456 7890123456 789012 345678 901234567890123456 7
        gConsole.OutputStream() << "    - Tab \"Project\" - Group \"Library\" - Click \"Label\" ==> \"Label Tag Library\"\n";
        //                          12345678901234567890123456789012345678901234567890123456789012345678901234567
        gConsole.OutputStream() << "      dialog                                                              [ ]\n";
        //                          1234567890123456 7890123456789012345 678901 23456 7890123456789012345678901234567
        gConsole.OutputStream() << "        - Click \"Load Label File...\" ==> \"Open\" dialog                    [ ]\n";
        gConsole.OutputStream() << "            - Select \"" << aImp << "\"\n";
        //                          123456789012345678901234 56789 012345678901234567890123456789012345678901234567
        gConsole.OutputStream() << "              and click \"Open\"                                            [ ]\n";
        //                          1234567890123456 7890123456789012345 678901 23456 7890123456789012345678901234567
        gConsole.OutputStream() << "        - Click \"Save Label File...\" ==> \"Open\" dialog                    [ ]\n";
        gConsole.OutputStream() << "            - Select \"" << aExp << "\"\n";
        //                          123456789012345678901234 56789 012345678901234567890123456789012345678901234567
        gConsole.OutputStream() << "              and click \"Open\"                                            [ ]\n";

        Instruction_End();
    }

    void Software::Open(const char* aFile)
    {
        if (NULL != mProcess)
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

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

void Instruction_Begin()
{
    ::Console::Instruction_Begin();

    gConsole.OutputStream() << "    Tool \"EBPro\"\n";
}

void Instruction_End()
{
    //                                   1           2         3         4         5         6         7
    //                          1234567890123456 789 0123456789012345678901234567890123456789012345678901234567
    gConsole.OutputStream() << "        - Click \"OK\"                                                      [ ]\n";

    gConsole.OutputStream() << "Presse ENTER to continue";

    ::Console::Instruction_End();

    char lLine[LINE_LENGTH];
    fgets(lLine, sizeof(lLine), stdin);
}
