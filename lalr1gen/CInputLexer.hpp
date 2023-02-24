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
//Namespaces
using namespace SJCLib;
//Definitions
#define KEYWORD_ALPHABET "ALPHABET"
#define KEYWORD_NONTERMINALS "NONTERMINALS"
#define KEYWORD_TERMINALS "TERMINALS"
#define KEYWORD_STARTSYMBOL "STARTSYMBOL"
#define KEYWORD_ENDOFINPUT "ENDOFINPUT"
#define KEYWORD_ENUM "enum"
#define KEYWORD_OPPREC "OPPREC"
#define KEYWORD_LEFT "LEFT"
#define KEYWORD_CASE_INSENSITIVE "CASE_INSENSITIVE"

enum ESourceToken
{
    TOKEN_ILLEGAL,
    TOKEN_END,
    TOKEN_TABSPACE,
    TOKEN_LINEFEED,
    TOKEN_CARRIAGERETURN,
    TOKEN_SINGLELITERAL,
    TOKEN_DEFINITION,
    TOKEN_COMMA,
    TOKEN_SINGLELINECOMMENT,
    TOKEN_MULTILINECOMMENT,
    TOKEN_OPENBRACE,
    TOKEN_CLOSEBRACE,
    TOKEN_SEMICOLON,
    TOKEN_IDENTIFIER,
    TOKEN_ALPHABET,
    TOKEN_NONTERMINALS,
    TOKEN_TERMINALS,
    TOKEN_STARTSYMBOL,
    TOKEN_ENDOFINPUT,
    TOKEN_ENUM,
    TOKEN_OPPREC,
    TOKEN_LEFT,
    TOKEN_ARROW,
    TOKEN_CASE_INSENSITIVE,
};

//DFA
class CInputLexer
{
    enum EState
    {
        STATE_ILLEGAL,
        STATE_START,
        STATE_END,
        STATE_FINISHED,
        STATE_TABSPACE,
        STATE_LINEFEED,
        STATE_CARRIAGERETURN,
        STATE_HASH,
        STATE_UNCLOSEDSINGLELITERAL,
        STATE_CLOSEDSINGLELITERAL,
        STATE_DEFINITION,
        STATE_COMMA,
        STATE_MINUS,
        STATE_SLASH,
        STATE_SINGLELINECOMMENT,
        STATE_MULTILINECOMMENT,
        STATE_ARROW,
        STATE_SEMICOLON,
        STATE_IDENTIFIER,
        STATE_OPENBRACE,
        STATE_CLOSEBRACE,
    };

    enum ECharClass
    {
        CC_ILLEGAL,
        CC_NULLBYTE,
        CC_TABSPACE,
        CC_LINEFEED,
        CC_CARRIAGERETURN,
        CC_HASH,
        CC_APOSTROPHE,
        CC_STAR,
        CC_PLUS,
        CC_COMMA,
        CC_MINUS,
        CC_SLASH,
        CC_SEMICOLON,
        CC_GREATERTHAN,
        CC_UNDERSCORE,
        CC_LETTER,
        CC_NUMBER,
        CC_SQUAREDBRACKETOPEN,
        CC_BACKSLASH,
        CC_SQUAREDBRACKETCLOSE,
        CC_OPENBRACE,
        CC_CLOSEBRACE,
        CC_INVALID,
    };
private:
    //Variables
    const char *pSource;
    uint32 lineNumber;
    CString tokenValue;
    //Functions
    ECharClass GetCharClass(char c);
    EState GetNextState(EState state, ECharClass cc);
    ESourceToken GetTokenType(EState state);
public:
    //Functions
    uint32 GetLineNumber() const;
    ESourceToken GetNextToken();
    const CString &GetTokenValue();
    void Init(const char *pSource);
};