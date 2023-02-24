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
#include "Input.hpp"
//Local
#include "Definitions.hpp"
//Definitions
#define EXPECTARROW {if(this->lexer.GetNextToken() != TOKEN_ARROW) ERROR("Expected arrow.");}
#define EXPECTIDENT {if(this->lexer.GetNextToken() != TOKEN_IDENTIFIER) ERROR("Expected identifier.");}
#define EXPECTSEM {if(this->lexer.GetNextToken() != TOKEN_SEMICOLON) ERROR("Expected semicolon.");}

//Private methods
bool Input::ReadEnum()
{
    String setName;
    ESourceToken t;
    bool follows;
    uint32 enumValue;
    BinaryTreeMap<String, uint32> set;

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
                        ERROR(u8"'" + this->lexer.GetTokenValue() + u8"' is already defined.");
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

    NOT_IMPLEMENTED_ERROR; //TODO: implement me
    return false;
}

//Public methods
bool Input::ParseInput(SeekableInputStream &refInput)
{
    bool finished;
    String buffer;

    finished = false;

    char *pText;
    uint64 size = refInput.QueryRemainingBytes();
    pText = (char *)MemAlloc((uint32)size+1);
    refInput.ReadBytes(pText, (uint32)size);
    pText[size] = 0;
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
                String regEx, mapToken;
                ESourceToken token;

                if(this->alphabetName.IsEmpty())
                    ERROR("No alphabet has been defined.");
                if(!this->enums.Contains(this->alphabetName))
                    ERROR("Alphabet has been declared but not defined.");

                regEx = this->lexer.GetTokenValue();
                regEx = regEx.SubString(1, regEx.GetLength() - 2);
                //regEx.Replace("\\t", '\t');

                nextLexerRuleToken:;
                token = this->lexer.GetNextToken();
                switch(token)
                {
                    case TOKEN_ARROW:
                        break;
                    case TOKEN_CASE_INSENSITIVE:
                        NOT_IMPLEMENTED_ERROR; //TODO: implement me
                        goto nextLexerRuleToken;
                        break;
                    default:
                        ERROR("Invalid token");
                        break;
                }

                EXPECTIDENT;

                const BinaryTreeMap<String, uint32> &refAlphabet = this->enums[this->alphabetName];

                mapToken = this->lexer.GetTokenValue();
                if(!refAlphabet.Contains(this->lexer.GetTokenValue()))
                    ERROR(u8"'" + this->lexer.GetTokenValue() + u8"' is not a defined alphabet value.");

                EXPECTSEM;

                this->lexerRules.Insert(regEx, mapToken);
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
                //this->startSymbol = this->lexer.GetTokenValue();
                NOT_IMPLEMENTED_ERROR; //TODO: implement me

                EXPECTSEM;
            }
                break;
            case TOKEN_ENUM:
                if(!this->ReadEnum())
                    ERROR("Couldn't read set.");
                break;
            case TOKEN_OPPREC:
                //if(!this->ReadOpPrecedences())
                NOT_IMPLEMENTED_ERROR; //TODO: implement me
                ERROR("Couldn't read operator precedences");
                break;
            default:
                ERROR("Wrong format");
        }
    }

    MemFree(pText);

    return true;
}