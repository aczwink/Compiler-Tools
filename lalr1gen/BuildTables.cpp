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
#include "CBuilder.hpp"
#include "CInput.h"
#include "CLexerBuilder.h"
#include "General.h"
//Namespaces
using namespace SJCLib;

//Prototypes
void RunSimulation(CInput &refInput, CLexerBuilder &refLexBuilder, CBuilder &refBuilder);

bool BuildTables(CString inputFileName, bool runSimulation)
{
    char *pBuffer;
    CFileInputStream file;
    CInput input;
    CLexerBuilder lexbuilder;
    CBuilder builder;

    if(!file.Open(inputFileName))
        ERROR("Input file couldn't be opened.");
    pBuffer = (char *)malloc(file.GetSize()+1);
    file.ReadBytes(pBuffer, file.GetSize());
    pBuffer[file.GetSize()] = '\0';

    if(!input.ParseInput(pBuffer))
    {
        free(pBuffer);
        ERROR("Couldn't parse input file.");
    }
    free(pBuffer);

    if(input.LexerShouldBeCreated())
    {
        if(!lexbuilder.Build(input))
            ERROR("Couldn't create lexer tables.");
    }

    if(input.ParserShouldBeCreated())
    {
        if(!builder.Build(input))
            ERROR("Couldn't create parser tables.");
    }

    if(runSimulation)
    {
        stdOut << "Starting simulation." << endl;
        RunSimulation(input, lexbuilder, builder);
    }

    return true;
}