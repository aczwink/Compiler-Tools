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
//Local
#include "CGrammar.hpp"
#include "CInputLexer.hpp"
#include "CRule.hpp"
//Namespaces
using namespace SJCLib;

class CInput
{
    friend class CLexerBuilder;
    friend class CBuilder;
private:
    //Variables
    bool createLexer;
    bool createParser;
    CString alphabetName;
    CFiniteSet<CString> definedConstants;
    CMap<CString, CMap<CString, uint32>> enums;
    CArray<CString> lexerRegExes; //in order
    CMap<CString, CString> lexerRules; //map regex to token value
    CFiniteSet<CString> caseInsensitiveLexerRules;
    CGrammar<CString> grammar;
    CInputLexer lexer;
    CString nonTerminalSetName;
    CString terminalSetName;
    CString startSymbol;
    CString endOfInput;
    CMap<CString, uint32> nonTerminals;
    uint32 nextNonTerminalId;
    CMap<CString, uint32> terminals;
    uint32 nextPrec;
    //Functions
    bool ReadEnum();
    bool ReadOpPrecedence(uint32 precedence);
    bool ReadOpPrecedences();
    bool ReadRuleRightSide(CString lhs, uint32 precedence);
    bool ReadRules();
public:
    //Constructor
    CInput();
    //Functions
    bool ParseInput(const char *pText);
    //Inline
    inline const CGrammar<CString> &GetGrammar() const
    {
        return this->grammar;
    }

    inline const CMap<CString, uint32> &GetTerminalsSet() const
    {
        return this->enums[this->alphabetName];
    }

    inline bool LexerShouldBeCreated() const
    {
        return this->createLexer;
    }

    inline bool ParserShouldBeCreated() const
    {
        return this->createParser;
    }
};