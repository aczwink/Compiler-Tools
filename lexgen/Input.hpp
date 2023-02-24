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
#pragma once
//Libs
#include <StdXX.hpp>
//Local
#include "InputLexer.hpp"
//Namespaces
using namespace StdXX;

class Input
{
private:
    //Members
    InputLexer lexer;
    String alphabetName;
    BinaryTreeMap<String, BinaryTreeMap<String, uint32>> enums;
    BinaryTreeSet<String> definedConstants;
    BinaryTreeMap<String, String> lexerRules; //map regex to token value
    //Methods
    bool ReadEnum();
public:
    //Methods
    bool ParseInput(SeekableInputStream &refInput);

    //Inline
    inline const BinaryTreeMap<String, String> &GetLexerRules() const
    {
        return this->lexerRules;
    }

    inline bool LexerShouldBeCreated() const
    {
        return !this->lexerRules.IsEmpty();
    }
};