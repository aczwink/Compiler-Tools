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
#include "CInputLexer.h"

//Private Functions
CInputLexer::ECharClass CInputLexer::GetCharClass(char c)
{
    switch(c)
    {
        case 0:
            return CC_NULLBYTE;
        case 9: //TAB
        case 32: //Space
            return CC_TABSPACE;
        case 10: //\n
            return CC_LINEFEED;
        case 13: //\r
            return CC_CARRIAGERETURN;
        case 35: //#
            return CC_HASH;
        case 39: //'
            return CC_APOSTROPHE;
        case 42: //*
            return CC_STAR;
        case 43: //+
            return CC_PLUS;
        case 44: //,
            return CC_COMMA;
        case 45: //-
            return CC_MINUS;
        case 47: ///
            return CC_SLASH;
        case 59: //;
            return CC_SEMICOLON;
        case 62: //>
            return CC_GREATERTHAN;
        case 95: //_
            return CC_UNDERSCORE;
        case 48: //0
        case 49: //1
        case 50: //2
        case 51: //3
        case 52: //4
        case 53: //5
        case 54: //6
        case 55: //7
        case 56: //8
        case 57: //9
            return CC_NUMBER;
        case 65: //A
        case 66: //B
        case 67: //C
        case 68: //D
        case 69: //E
        case 70: //F
        case 71: //G
        case 72: //H
        case 73: //I
        case 74: //J
        case 75: //K
        case 76: //L
        case 77: //M
        case 78: //N
        case 79: //O
        case 80: //P
        case 81: //Q
        case 82: //R
        case 83: //S
        case 84: //T
        case 85: //U
        case 86: //V
        case 87: //W
        case 88: //X
        case 89: //Y
        case 90: //Z
        case 97: //a
        case 98: //b
        case 99: //c
        case 100: //d
        case 101: //e
        case 102: //f
        case 103: //g
        case 104: //h
        case 105: //i
        case 106: //j
        case 107: //k
        case 108: //l
        case 109: //m
        case 110: //n
        case 111: //o
        case 112: //p
        case 113: //q
        case 114: //r
        case 115: //s
        case 116: //t
        case 117: //u
        case 118: //v
        case 119: //w
        case 120: //x
        case 121: //y
        case 122: //z
            return CC_LETTER;
        case 91: //[
            return CC_SQUAREDBRACKETOPEN;
        case 92: //Backslash
            return CC_BACKSLASH;
        case 93: //]
            return CC_SQUAREDBRACKETCLOSE;
        case 123: //{
            return CC_OPENBRACE;
        case 125: //}
            return CC_CLOSEBRACE;
        case 33: //!
        case 34: //"
        case 36: //$
        case 37: //%
        case 38: //&
        case 40: //(
        case 41: //)
        case 46: //.
        case 58: //:
        case 60: //<
        case 61: //=
        case 63: //?
        case 94: //^
        case 124: //|
        case -28: //ä
        case -33: //ß
            return CC_INVALID;
    }
    return CC_ILLEGAL;
}

CInputLexer::EState CInputLexer::GetNextState(EState state, ECharClass cc)
{
    switch(state)
    {
        case STATE_START:
            switch(cc)
            {
                case CC_NULLBYTE:
                    return STATE_END;
                case CC_TABSPACE:
                    return STATE_TABSPACE;
                case CC_LINEFEED:
                    return STATE_LINEFEED;
                case CC_CARRIAGERETURN:
                    return STATE_CARRIAGERETURN;
                case CC_HASH:
                    return STATE_HASH;
                case CC_APOSTROPHE:
                    return STATE_UNCLOSEDSINGLELITERAL;
                case CC_COMMA:
                    return STATE_COMMA;
                case CC_MINUS:
                    return STATE_MINUS;
                case CC_SLASH:
                    return STATE_SLASH;
                case CC_SEMICOLON:
                    return STATE_SEMICOLON;
                case CC_LETTER:
                    return STATE_IDENTIFIER;
                case CC_OPENBRACE:
                    return STATE_OPENBRACE;
                case CC_CLOSEBRACE:
                    return STATE_CLOSEBRACE;
            }
            break;
        case STATE_END:
            return STATE_FINISHED;
        case STATE_TABSPACE:
            switch(cc)
            {
                case CC_TABSPACE:
                    return STATE_TABSPACE;
                case CC_CARRIAGERETURN:
                case CC_HASH:
                case CC_MINUS:
                case CC_SLASH:
                case CC_LETTER:
                case CC_SEMICOLON:
                case CC_OPENBRACE:
                case CC_CLOSEBRACE:
                    return STATE_FINISHED;
            }
            break;
        case STATE_LINEFEED:
            switch(cc)
            {
                case CC_TABSPACE:
                case CC_CARRIAGERETURN:
                case CC_HASH:
                case CC_APOSTROPHE:
                case CC_SLASH:
                case CC_SEMICOLON:
                case CC_LETTER:
                case CC_OPENBRACE:
                case CC_CLOSEBRACE:
                    return STATE_FINISHED;
            }
            break;
        case STATE_CARRIAGERETURN:
            switch(cc)
            {
                case CC_LINEFEED:
                    return STATE_FINISHED;
            }
            break;
        case STATE_HASH:
            switch(cc)
            {
                case CC_LETTER:
                    return STATE_DEFINITION;
            }
            break;
        case STATE_UNCLOSEDSINGLELITERAL:
            switch(cc)
            {
                case CC_APOSTROPHE:
                    return STATE_CLOSEDSINGLELITERAL;
                case CC_TABSPACE:
                case CC_HASH:
                case CC_STAR:
                case CC_PLUS:
                case CC_COMMA:
                case CC_MINUS:
                case CC_SLASH:
                case CC_SEMICOLON:
                case CC_GREATERTHAN:
                case CC_UNDERSCORE:
                case CC_LETTER:
                case CC_NUMBER:
                case CC_SQUAREDBRACKETOPEN:
                case CC_BACKSLASH:
                case CC_SQUAREDBRACKETCLOSE:
                case CC_OPENBRACE:
                case CC_CLOSEBRACE:
                case CC_INVALID:
                    return STATE_UNCLOSEDSINGLELITERAL;
            }
            break;
        case STATE_CLOSEDSINGLELITERAL:
            switch(cc)
            {
                case CC_TABSPACE:
                    return STATE_FINISHED;
            }
            break;
        case STATE_DEFINITION:
            switch(cc)
            {
                case CC_TABSPACE:
                case CC_SEMICOLON:
                    return STATE_FINISHED;
                case CC_LETTER:
                    return STATE_DEFINITION;
            }
            break;
        case STATE_COMMA:
            switch(cc)
            {
                case CC_CARRIAGERETURN:
                    return STATE_FINISHED;
            }
            break;
        case STATE_MINUS:
            switch(cc)
            {
                case CC_GREATERTHAN:
                    return STATE_ARROW;
            }
            break;
        case STATE_SLASH:
            switch(cc)
            {
                case CC_SLASH:
                    return STATE_SINGLELINECOMMENT;
                case CC_STAR:
                    return STATE_MULTILINECOMMENT;
            }
            break;
        case STATE_SINGLELINECOMMENT:
            return STATE_FINISHED;
        case STATE_MULTILINECOMMENT:
            return STATE_FINISHED;
        case STATE_ARROW:
            switch(cc)
            {
                case CC_TABSPACE:
                    return STATE_FINISHED;
            }
            break;
        case STATE_SEMICOLON:
            switch(cc)
            {
                case CC_NULLBYTE:
                case CC_TABSPACE:
                case CC_CARRIAGERETURN:
                    return STATE_FINISHED;
            }
            break;
        case STATE_IDENTIFIER:
            switch(cc)
            {
                case CC_NULLBYTE:
                case CC_TABSPACE:
                case CC_CARRIAGERETURN:
                case CC_COMMA:
                case CC_SEMICOLON:
                    return STATE_FINISHED;
                case CC_UNDERSCORE:
                case CC_LETTER:
                case CC_NUMBER:
                    return STATE_IDENTIFIER;
            }
            break;
        case STATE_OPENBRACE:
            switch(cc)
            {
                case CC_CARRIAGERETURN:
                    return STATE_FINISHED;
            }
            break;
        case STATE_CLOSEBRACE:
            switch(cc)
            {
                case CC_SEMICOLON:
                    return STATE_FINISHED;
            }
            break;
    }

    return STATE_ILLEGAL;
}

ESourceToken CInputLexer::GetTokenType(EState state)
{
    switch(state)
    {
        case STATE_END:
            return TOKEN_END;
        case STATE_TABSPACE:
            return TOKEN_TABSPACE;
        case STATE_LINEFEED:
            return TOKEN_LINEFEED;
        case STATE_CARRIAGERETURN:
            return TOKEN_CARRIAGERETURN;
        case STATE_CLOSEDSINGLELITERAL:
            return TOKEN_SINGLELITERAL;
        case STATE_DEFINITION:
            return TOKEN_DEFINITION;
        case STATE_COMMA:
            return TOKEN_COMMA;
        case STATE_SINGLELINECOMMENT:
            return TOKEN_SINGLELINECOMMENT;
        case STATE_MULTILINECOMMENT:
            return TOKEN_MULTILINECOMMENT;
        case STATE_ARROW:
            return TOKEN_ARROW;
        case STATE_SEMICOLON:
            return TOKEN_SEMICOLON;
        case STATE_IDENTIFIER:
            return TOKEN_IDENTIFIER;
        case STATE_OPENBRACE:
            return TOKEN_OPENBRACE;
        case STATE_CLOSEBRACE:
            return TOKEN_CLOSEBRACE;
    }
    return TOKEN_ILLEGAL;
}

//Public Functions
uint32 CInputLexer::GetLineNumber() const
{
    return this->lineNumber;
}

ESourceToken CInputLexer::GetNextToken()
{
    EState state, acceptedState;
    ECharClass cc;
    const char *pStart, *pAcceptedPos;
    ESourceToken token;

    start:;
    state = STATE_START;
    pStart = this->pSource;

    while(state != STATE_FINISHED)
    {
        if(this->GetTokenType(state))
        {
            acceptedState = state;
            pAcceptedPos = this->pSource;

            if(state == STATE_END)
                break;
        }
        cc = this->GetCharClass(*this->pSource);
        state = this->GetNextState(state, cc);
        this->pSource++;
    }

    this->pSource = pAcceptedPos;
    this->tokenValue.Assign(pStart, uint32(this->pSource - pStart));
    token = this->GetTokenType(acceptedState);

    switch(token)
    {
        case TOKEN_END:
            this->pSource--; //Again point to end
            return TOKEN_END;
        case TOKEN_LINEFEED:
            this->lineNumber++;
        case TOKEN_TABSPACE:
        case TOKEN_CARRIAGERETURN:
            //ignore
            goto start;
        case TOKEN_SINGLELINECOMMENT:
        {
            char c;

            while(true)
            {
                c = *this->pSource++;

                if(c == 0)
                {
                    this->pSource--;
                    break;
                }
                if(c == '\n')
                {
                    this->lineNumber++;
                    break;
                }
            }
            goto start; //ignore
        }
            break;
        case TOKEN_MULTILINECOMMENT:
        {
            char c;

            while(true)
            {
                c = *this->pSource++;

                if(c == '\0')
                {
                    this->pSource--;
                    return TOKEN_ILLEGAL;
                }
                if(c == '*' && *this->pSource == '/')
                {
                    this->pSource++;
                    goto start; //ignore
                }
            }
        }
            break;
        case TOKEN_IDENTIFIER:
            if(this->tokenValue == KEYWORD_ALPHABET)
                return TOKEN_ALPHABET;
            if(this->tokenValue == KEYWORD_NONTERMINALS)
                return TOKEN_NONTERMINALS;
            if(this->tokenValue == KEYWORD_TERMINALS)
                return TOKEN_TERMINALS;
            if(this->tokenValue == KEYWORD_STARTSYMBOL)
                return TOKEN_STARTSYMBOL;
            if(this->tokenValue == KEYWORD_ENDOFINPUT)
                return TOKEN_ENDOFINPUT;
            if(this->tokenValue == KEYWORD_ENUM)
                return TOKEN_ENUM;
            if(this->tokenValue == KEYWORD_OPPREC)
                return TOKEN_OPPREC;
            if(this->tokenValue == KEYWORD_LEFT)
                return TOKEN_LEFT;
            if(this->tokenValue == KEYWORD_CASE_INSENSITIVE)
                return TOKEN_CASE_INSENSITIVE;
            return TOKEN_IDENTIFIER;
        default:
            return token;
    }

    return TOKEN_ILLEGAL;
}

const CString &CInputLexer::GetTokenValue()
{
    return this->tokenValue;
}

void CInputLexer::Init(const char *pSource)
{
    this->pSource = pSource;
    this->lineNumber = 0;
}