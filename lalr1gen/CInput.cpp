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
//Class Header
#include "CInput.h"
//Local
#include "General.h"
//Definitions
#define EXPECTARROW {if(this->lexer.GetNextToken() != TOKEN_ARROW) ERROR("Expected arrow.");}
#define EXPECTIDENT {if(this->lexer.GetNextToken() != TOKEN_IDENTIFIER) ERROR("Expected identifier.");}
#define EXPECTSEM {if(this->lexer.GetNextToken() != TOKEN_SEMICOLON) ERROR("Expected semicolon.");}

//Constructor
CInput::CInput()
{
    this->createLexer = false;
    this->createParser = false;
    this->nextNonTerminalId = 0;
    this->grammar.productionRules.EnsureSize(100000); //IF ARRAY NEEDS TO RESIZE, PRODUCTION RULE PRECEDENCE WILL FUCK UP
}

//Private Functions
bool CInput::ReadEnum()
{
    CString setName;
    ESourceToken t;
    bool follows;
    uint32 enumValue;
    CMap<CString, uint32> set;

    follows = true;
    enumValue = 0;

    EXPECTIDENT;
    setName = this->lexer.GetTokenValue();
    if(this->lexer.GetNextToken() != TOKEN_OPENBRACE)
        ERROR("Expected open brace.");

    while(true)
    {
        t = this->lexer.GetNextToken();

        switch(t)
        {
            case TOKEN_IDENTIFIER:
                if(follows)
                {
                    if(this->definedConstants.Contains(this->lexer.GetTokenValue()))
                        ERROR("'" + this->lexer.GetTokenValue() + "' is already defined.");
                    set.Insert(this->lexer.GetTokenValue(), enumValue);
                    this->definedConstants.Insert(this->lexer.GetTokenValue());
                }
                else
                {
                    ERROR("Expected a comma.");
                }
                follows = false;
                enumValue++;
                break;
            case TOKEN_COMMA:
                if(follows)
                    if(this->lexer.GetNextToken() != TOKEN_IDENTIFIER)
                        ERROR("Expected identifier.");
                follows = true;
                break;
            case TOKEN_CLOSEBRACE:
                if(this->lexer.GetNextToken() != TOKEN_SEMICOLON)
                    ERROR("Expected semicolon.");
                this->enums.Insert(setName, set);
                return true;
                break;
            default:
                INPUTERROR("Illegal input.");
        }
    }

    ASSERT(0);
    return false;
}

bool CInput::ReadOpPrecedence(uint32 precedence)
{
    while(true)
    {
        switch(this->lexer.GetNextToken())
        {
            case TOKEN_IDENTIFIER:
                if(!this->enums[this->alphabetName].Find(this->lexer.GetTokenValue()).IsValid())
                    INPUTERROR('\'' + this->lexer.GetTokenValue() + "' is not a terminal.");
                this->grammar.opPrecedences[this->lexer.GetTokenValue()] = precedence;
                break;
            case TOKEN_DEFINITION:
                this->grammar.opPrecedences[this->lexer.GetTokenValue()] = precedence;
                break;
            case TOKEN_SEMICOLON:
                return true;
        }
    }

    return false;
}

bool CInput::ReadOpPrecedences()
{
    this->nextPrec = 1;
    for(CMapIterator<CString, uint32> it = this->enums[this->alphabetName].Begin(); it.IsValid(); it++)
    {
        this->grammar.opPrecedences.Insert(it.GetKey(), 0);
    }

    while(true)
    {
        switch(this->lexer.GetNextToken())
        {
            case TOKEN_LEFT:
                if(!this->ReadOpPrecedence(this->nextPrec++))
                    INPUTERROR("Couldn't read precedence statement.");
                break;
            case TOKEN_SEMICOLON:
                return true;
            default:
                INPUTERROR("Invalid input while trying to read operator precedences.");
        }
    }

    return false;
}

bool CInput::ReadRuleRightSide(CString lhs, uint32 precedence)
{
    uint32 index;
    ESourceToken t;
    CRule<CString> rule;
    bool ok;

    ok = false;
    rule.lhs = lhs;

    while(true)
    {
        t = this->lexer.GetNextToken();

        switch(t)
        {
            case TOKEN_END:
            case TOKEN_SEMICOLON:
                index = this->grammar.productionRules.Push(rule);
                this->grammar.rulePrecedences.Insert(&this->grammar.productionRules[index], precedence);
                return true;
            case TOKEN_IDENTIFIER:
            {
                //if(!this->nonTerminals.Find(this->lexer.GetTokenValue()).IsValid() && !this->terminals.Find(this->lexer.GetTokenValue()).IsValid())
                //ERROR('\'' + this->lexer.GetTokenValue() + "' is neither a terminal nor a nonterminal.");
                if(!this->definedConstants.Contains(this->lexer.GetTokenValue()))
                {
                    this->definedConstants.Insert(this->lexer.GetTokenValue());
                    this->nonTerminals.Insert(this->lexer.GetTokenValue(), this->nextNonTerminalId++);
                }

                rule.rhs.Push(this->lexer.GetTokenValue());
            }
                break;
            default:
                INPUTERROR("Illegal input.");
        }
    }

    return false;
}

bool CInput::ReadRules()
{
    ESourceToken t;
    CString buffer;

    while(true)
    {
        t = this->lexer.GetNextToken();

        switch(t)
        {
            case TOKEN_END:
                return true;
            case TOKEN_DEFINITION:
            {
                CString precId;

                precId = this->lexer.GetTokenValue();
                if(!this->grammar.opPrecedences.Find(precId).IsValid())
                    INPUTERROR('\'' + precId + "' was not defined in operator precedences.");
                if(this->lexer.GetNextToken() != TOKEN_IDENTIFIER)
                    INPUTERROR("Expected identifier.");
                buffer = this->lexer.GetTokenValue();
                if(this->lexer.GetNextToken() != TOKEN_ARROW)
                    ERROR("Expected arrow.");
                if(!this->ReadRuleRightSide(buffer, 0))
                    ERROR("Couldn't read rule.");
                this->grammar.rulePrecedences[&this->grammar.productionRules[this->grammar.productionRules.GetNoOfElements()-1]] = this->grammar.opPrecedences[precId];
            }
                break;
            default:
                ERROR("Illegal input.");
        }
    }

    return false;
}

//Public Functions
bool CInput::ParseInput(const char *pText)
{
    bool finished;
    CString buffer;

    finished = false;

    this->lexer.Init(pText);

    while(!finished)
    {
        switch(this->lexer.GetNextToken())
        {
            case TOKEN_END:
                finished = true;
                break;
            case TOKEN_SINGLELITERAL:
            {
                CString regEx, mapToken;
                ESourceToken token;
                CMap<CString, uint32> *pAlphabet;

                if(this->alphabetName.IsEmpty())
                    ERROR("No alphabet has been defined.");
                if(!this->enums.Contains(this->alphabetName))
                    ERROR("Alphabet has been declared but not defined.");

                pAlphabet = &this->enums[this->alphabetName];

                regEx = this->lexer.GetTokenValue();
                regEx = regEx.SubString(1, regEx.GetLength()-2);
                //regEx.Replace("\\t", '\t');

                nextLexerRuleToken:;
                token = this->lexer.GetNextToken();
                switch(token)
                {
                    case TOKEN_ARROW:
                        break;
                    case TOKEN_CASE_INSENSITIVE:
                        this->caseInsensitiveLexerRules.Insert(regEx);
                        goto nextLexerRuleToken;
                        break;
                    default:
                        ERROR("Invalid token");
                        break;
                }

                EXPECTIDENT;

                mapToken = this->lexer.GetTokenValue();
                if(!pAlphabet->Contains(this->lexer.GetTokenValue()))
                    ERROR("'" + this->lexer.GetTokenValue() + "' is not a defined alphabet value.");

                EXPECTSEM;

                this->lexerRegExes.Push(regEx);
                this->lexerRules.Insert(regEx, mapToken);
            }
                break;
            case TOKEN_IDENTIFIER: //Read a parser rule
            {
                uint32 rulePrecedence;

                buffer = this->lexer.GetTokenValue();

                if(!this->definedConstants.Contains(buffer))
                {
                    this->definedConstants.Insert(buffer);
                    this->nonTerminals.Insert(buffer, this->nextNonTerminalId++);
                }

                switch(this->lexer.GetNextToken())
                {
                    case TOKEN_ARROW:
                    {
                        rulePrecedence = 0;
                    }
                        break;
                    case TOKEN_DEFINITION:
                    {
                        rulePrecedence = this->grammar.opPrecedences[this->lexer.GetTokenValue()];

                        EXPECTARROW;
                    }
                        break;
                    default:
                        ERROR("Expected arrow or operator precedence");
                }

                if(!this->ReadRuleRightSide(buffer, rulePrecedence))
                    ERROR("Couldn't read rule.");
            }
                break;
            case TOKEN_ALPHABET:
            EXPECTIDENT;
                this->alphabetName = this->lexer.GetTokenValue();
                EXPECTSEM;
                break;
            case TOKEN_STARTSYMBOL:
            {
                EXPECTIDENT;
                this->startSymbol = this->lexer.GetTokenValue();

                EXPECTSEM;
            }
                break;
            case TOKEN_ENUM:
                if(!this->ReadEnum())
                    ERROR("Couldn't read set.");
                break;
            case TOKEN_OPPREC:
                if(!this->ReadOpPrecedences())
                    ERROR("Couldn't read operator precedences");
                break;
            default:
                ERROR("Wrong format");
        }
    }

    //i am to lazy to do this better
    this->terminals = this->enums[this->alphabetName];
    foreach(it, this->terminals)
    {
        if(it.GetValue() == 1)
        {
            this->endOfInput = it.GetKey();

            goto terminalsOk;
        }
    }
    ERROR("Your alphabet does not contain the required 'end of input' token with value 1");
    terminalsOk:;
    foreach(it, this->terminals)
    {
        this->grammar.terminals.Insert(it.GetKey());
    }
    foreach(it, this->nonTerminals)
    {
        this->grammar.nonTerminals.Insert(it.GetKey());
    }
    //end of lazyness

    //Put rule precedences
    foreach(it, this->grammar.productionRules)
    {
        if((*it).IsEpsilonRule() || this->grammar.rulePrecedences[&*it]) //skip epsilon rules or rules which already have precedence
            continue;
        for(int32 j = (*it).rhs.GetNoOfElements()-1; j >= 0; j--)
        {
            if(this->grammar.IsTerminal((*it).rhs[j]))
                this->grammar.rulePrecedences[&*it] = this->grammar.opPrecedences[(*it).rhs[j]];
        }
    }

    //Check what we need to create
    if(!this->lexerRules.IsEmpty())
        this->createLexer = true;

    if(!this->grammar.productionRules.IsEmpty())
    {
        foreach(it, this->nonTerminals)
        {
            repeat(this->grammar.productionRules.GetNoOfElements(), i)
            {
                if(it.GetKey() == this->grammar.productionRules[i].lhs)
                    goto nextNonTerminal;
            }
            ERROR("'" + it.GetKey() + "' has no production rule");
            nextNonTerminal:;
        }

        if(this->startSymbol.IsEmpty())
        {
            ERROR("No start symbol defined");
        }

        if(!this->nonTerminals.Contains(this->startSymbol))
        {
            ERROR("The start symbol has no production rule.");
        }

        this->createParser = true;
    }

    /*if(this->lexer.GetNextToken() != TOKEN_NONTERMINALS)
        ERROR((CString)"Expected '" + KEYWORD_NONTERMINALS + "' keyword.");
    if(this->lexer.GetNextToken() != TOKEN_IDENTIFIER)
        ERROR("Expected identifier.");
    this->nonTerminalSetName = this->lexer.GetTokenValue();
    if(this->lexer.GetNextToken() != TOKEN_SEMICOLON)
        ERROR("Expected semicolon.");

    if(this->lexer.GetNextToken() != TOKEN_TERMINALS)
        ERROR((CString)"Expected '" + KEYWORD_TERMINALS + "' keyword.");
    if(this->lexer.GetNextToken() != TOKEN_IDENTIFIER)
        ERROR("Expected identifier.");
    this->terminalSetName = this->lexer.GetTokenValue();
    if(this->lexer.GetNextToken() != TOKEN_SEMICOLON)
        ERROR("Expected semicolon.");

    if(this->lexer.GetNextToken() != TOKEN_STARTSYMBOL)
        ERROR((CString)"Expected '" + KEYWORD_STARTSYMBOL + "' keyword.");
    if(this->lexer.GetNextToken() != TOKEN_IDENTIFIER)
        ERROR("Expected identifier.");
    this->startSymbol = this->lexer.GetTokenValue();
    if(this->lexer.GetNextToken() != TOKEN_SEMICOLON)
        ERROR("Expected semicolon.");

    if(this->lexer.GetNextToken() != TOKEN_ENDOFINPUT)
        ERROR((CString)"Expected '" + KEYWORD_ENDOFINPUT + "' keyword.");
    if(this->lexer.GetNextToken() != TOKEN_IDENTIFIER)
        ERROR("Expected identifier.");
    this->endOfInput = this->lexer.GetTokenValue();
    if(this->lexer.GetNextToken() != TOKEN_SEMICOLON)
        ERROR("Expected semicolon.");

    if(!this->ReadEnum())
        ERROR("Couldn't read set.");
    if(!this->ReadEnum())
        ERROR("Couldn't read set.");
    if(!this->ReadOpPrecedences())
        ERROR("Couldn't read operator precedences");

    repeat(this->grammar.productionRules.GetNoOfElements(), i)
    {
        this->grammar.rulePrecedences.Insert(&this->grammar.productionRules[i], 0);
    }

    if(!this->ReadRules())
        ERROR("Couldn't read rules.");

    for(CMapIterator<CString, uint32> it = this->terminals.Begin(); it.IsValid(); it++)
    {
        this->grammar.terminals.Insert(it.GetKey());
    }
    for(CMapIterator<CString, uint32> it = this->nonTerminals.Begin(); it.IsValid(); it++)
    {
        this->grammar.nonTerminals.Insert(it.GetKey());
    }

    repeat(this->grammar.productionRules.GetNoOfElements(), i)
    {
        if(this->grammar.productionRules[i].IsEpsilonRule() || this->grammar.rulePrecedences[&this->grammar.productionRules[i]] != 0)
            continue;
        for(int32 j = this->grammar.productionRules[i].rhs.GetNoOfElements()-1; j >= 0; j--)
        {
            if(this->grammar.IsTerminal(this->grammar.productionRules[i].rhs[j]))
            {
                this->grammar.rulePrecedences[&this->grammar.productionRules[i]] = this->grammar.opPrecedences[this->grammar.productionRules[i].rhs[j]];
            }
        }
    }*/

    return true;
}