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
//Local
#include "Input.hpp"
#include "LexerBuilder.hpp"
#include "Definitions.hpp"
//Namespaces
using namespace StdXX;

bool BuildTables(const FileSystem::Path &refInputFilePath)
{
    Input input;

    FileInputStream file(refInputFilePath);

    if(!input.ParseInput(file))
    {
        ERROR("Couldn't parse input file.");
    }

    if(input.LexerShouldBeCreated())
    {
        LexerBuilder lexBuilder(input);

        if(!lexBuilder.Build())
            ERROR("Couldn't create lexer tables.");
    }

    return true;
}