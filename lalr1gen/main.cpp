/*
 * Copyright (c) 2023 Amir Czwink (amir130@hotmail.de)
 *
 * This file is part of Compiler-Tools.
 *
 * Compiler-Tools is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Compiler-Tools is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Compiler-Tools.  If not, see <http://www.gnu.org/licenses/>.
 */
//Local
#include "resource.h"
//Namespaces
using namespace SJCLib;

//Prototypes
bool BuildTables(CString input, bool runSimulation);
bool CheckArguments();
bool EvaluateArguments(CArray<CString> &args);
void PrintManual();

//Global Variables
CString g_Input;
CString g_logFileName, g_simFileName;
AOutputStream *pOutput;
CFileInputStream g_simInput;
CFileOutputStream g_OutputFile, g_simResult;

int main(int argc, char **argv)
{
    CArray<CString> args;

    SetConsoleTitle(APPLICATION_TITLE);
    for(uint32 i = 1; i < (uint32)argc; i++)
    {
        args.Push(argv[i]);
    }

    if(EvaluateArguments(args))
    {
        if(CheckArguments())
        {
            if(BuildTables(g_Input, !g_simFileName.IsEmpty()))
            {
                stdOut << "Done." << endl;
                return EXIT_SUCCESS;
            }
        }

        return EXIT_FAILURE;
    }
    else
    {
        PrintManual();
        return EXIT_FAILURE;
    }

    PrintManual();
    return EXIT_SUCCESS;
}

void PrintManual()
{
    stdOut << APPLICATION_TITLE << endl << endl
           << "Usage: " << endl
           << "  " << APPLICATION_NAME << " input [options]" << endl << endl
           << "   input      an input file describing the grammar" << endl
           << "   options" << endl
           << "      -log file    put several information into file" << endl
           << "      -sim file    run simulation on file" << endl;
}

bool EvaluateArguments(CArray<CString> &args)
{
    uint8 i;

    if(args.GetNoOfElements() >= 1)
    {
        //at least the input file must exist
        g_Input = args[0];
        pOutput = &stdOut;

        for(i = 1; i < args.GetNoOfElements(); i++)
        {
            if(args[i] == "-log")
            {
                g_logFileName = args[++i];
            }
            else if(args[i] == "-sim")
            {
                g_simFileName = args[++i];
            }
            else
            {
                return false;
            }
        }

        return true;
    }

    return false;
}

bool CheckArguments()
{
    if(!g_logFileName.IsEmpty())
    {
        if(g_OutputFile.Open(g_logFileName))
        {
            pOutput = &g_OutputFile;
        }
        else
        {
            stdErr << "Log file couldn't be opened." << endl;
            return false;
        }
    }

    if(!g_simFileName.IsEmpty())
    {
        if(!g_simInput.Open(g_simFileName))
        {
            stdErr << "Couldn't open simulation input." << endl;
            return false;
        }

        if(!g_simResult.Open(L"__simresult.log"))
        {
            stdErr << "Couldn't open simulation result file." << endl;
            return false;
        }
    }

    return true;
}