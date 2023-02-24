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
#include "CLexerBuilder.h"
//Special SJCLib file
#include <SJCLib\__CRegExBuilder.h>
//Namespaces
using namespace SJCLibIntern;

//Constructor
CLexerBuilder::CLexerBuilder()
{
    this->pTransitionTable = NULL;
    this->pTokenTypeTable = NULL;
}

//Destructor
CLexerBuilder::~CLexerBuilder()
{
    memfreesafe(pTransitionTable);
    memfreesafe(pTokenTypeTable);
}

//Private Functions
const CString &CLexerBuilder::FindAlphabetValue(uint32 stateIndex, const CMap<CString, uint32> &refAlphabet) const
{
    foreach(it, refAlphabet)
    {
        if(it.GetValue() == stateIndex)
        {
            return it.GetKey();
        }
    }
    ASSERT(0);

    return refAlphabet.Begin().GetKey();
}

//Public Functions
bool CLexerBuilder::Build(const CInput &refInput)
{
    uint32 value;
    CRegExBuilder regExBuilder;
    CFileOutputStream file;
    const CMap<CString, uint32> &refAlphabet = refInput.enums[refInput.alphabetName];
    const CMap<CString, CString> &refRules = refInput.lexerRules;

    if(!file.Open(L"lexertables.h"))
    {
        stdErr << "Couldn't open 'lexertables.h'";
        return false;
    }

    /*foreach(it, refRules)
    {
        CRegExBuilder currentRegExBuilder;

        currentRegExBuilder.CreateNFA(it.GetKey(), refAlphabet[it.GetValue()]);
        regExBuilder.Combine(currentRegExBuilder);
    }*/
    repeat(refInput.lexerRegExes.GetNoOfElements(), i)
    {
        CRegExBuilder currentRegExBuilder;

        currentRegExBuilder.CreateNFA(refInput.lexerRegExes[i], refAlphabet[refRules[refInput.lexerRegExes[i]]], !refInput.caseInsensitiveLexerRules.Contains(refInput.lexerRegExes[i]));
        regExBuilder.Combine(currentRegExBuilder);
    }

    regExBuilder.SaveNFAGraph("1");
    regExBuilder.ConvertNFAToDFA();
    regExBuilder.SaveDFAGraph("2");
    regExBuilder.ReduceDFA();
    regExBuilder.SaveDFAGraph("3");

    pTransitionTable = (uint32 *)malloc(regExBuilder.GetNumberOfAlphabetElements() * regExBuilder.GetNumberOfDFAStates() * sizeof(*pTransitionTable));
    pTokenTypeTable = (uint32 *)malloc(regExBuilder.GetNumberOfDFAStates() * sizeof(*pTokenTypeTable));
    regExBuilder.BuildTables(translate, pTransitionTable, pTokenTypeTable);
    regExBuilder.SaveDFAGraph("4");

    //Write translate table
    file << "static const uint8 translateTable[] =" << endl << "{" << endl;
    repeat(16, i)
    {
        file << "\t";
        repeat(16, j)
        {
            file << translate[i*16+j] << ", ";
        }
        file << endl;
    }
    file << "};" << endl << endl;

    //Write transition table
    file << "static const uint32 transitionsTable[] =" << endl << "{" << endl;

    repeat(regExBuilder.GetNumberOfDFAStates(), i)
    {
        file << "\t";
        repeat(regExBuilder.GetNumberOfAlphabetElements(), j)
        {
            value = pTransitionTable[i * regExBuilder.GetNumberOfAlphabetElements() + j];

            if(value == CLEXER_STATE_FINISHED)
            {
                file << "CLEXER_STATE_FINISHED, ";
            }
            else if(value == CLEXER_STATE_END)
            {
                file << "CLEXER_STATE_END, ";
            }
            else
            {
                file << value << ", ";
            }
        }
        file << endl;
    }
    file << "};" << endl << endl;

    //Write token type table
    file << "static const " << refInput.alphabetName << " tokenTypeTable[] =" << endl << "{" << endl;
    repeat(regExBuilder.GetNumberOfDFAStates(), i)
    {
        file << "\t" << this->FindAlphabetValue(pTokenTypeTable[i], refAlphabet) << "," << endl;
    }
    file << "};";

    return true;
}