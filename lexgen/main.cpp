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
//Libs
#include <StdXX.hpp>
//Namespaces
using namespace StdXX;
//Definitions
#define APPLICATION_NAME "LexerGenerator"
#define APPLICATION_CREATOR "Amir Czwink"

//Prototypes
bool BuildTables(const FileSystem::Path &refInputFilePath);
bool CheckArguments();
bool EvaluateArguments(const FixedArray<String> &refArgs);
void PrintManual();

//Global Variables
FileSystem::Path g_input;

int32 Main(const String &programName, const FixedArray<String> &args)
{
    if(EvaluateArguments(args) && CheckArguments())
    {
        if(BuildTables(g_input))
        {
            stdOut << "Done." << endl;
            return EXIT_SUCCESS;
        }

        return EXIT_FAILURE;
    }
    else
    {
        PrintManual();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void PrintManual()
{
    stdOut << APPLICATION_NAME << endl << endl
           << "Usage: " << endl
           << "  " << APPLICATION_NAME << " input" << endl << endl
           << "   input      an input file describing the grammar" << endl;
}

bool EvaluateArguments(const FixedArray<String> &refArgs)
{
    if(refArgs.GetNumberOfElements() != 1) //exactly one argument must exist
        return false;

    g_input = refArgs[0];

    return true;
}

bool CheckArguments()
{
    FileSystem::File file(g_input);
    return file.Exists();
}