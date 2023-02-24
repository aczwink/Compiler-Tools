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
#include "LexerBuilder.hpp"
//Namespaces
using namespace StdXX::FormalLanguages;

//Constructor
LexerBuilder::LexerBuilder(const Input &refInput) : refInput(refInput), cppFile(FileSystem::Path("CLexer.cpp"))
{
}

//Private methods
void LexerBuilder::BuildTables(const LinkedList<CharacterClass *> &refCharSets)
{
    this->BuildTranslateTable(refCharSets);
}

void LexerBuilder::BuildTranslateTable(const LinkedList<CharacterClass *> &refCharSets)
{
    TextWriter textWriter(this->cppFile, TextCodecType::UTF8);

    textWriter << "uint32 CLexer::Translate(uint32 codePoint)"
                  << endl << "{";

    textWriter << endl << "}"
                  << endl;
}

//Public methods
bool LexerBuilder::Build()
{
    NFA *pNFA;
    DFA *pDFA;
    CRegExParser regExParser;
    Alphabet alphabet;

    for(const auto &refKV : this->refInput.GetLexerRules())
    {
        if(!regExParser.Preprocess(refKV.key))
        {
            stdErr << "Malformed lexer-rule: \"" << refKV.key << '"' << endl;
            return false;
        }
    }

    regExParser.BuildDisjointCharSets();

    regExParser.PushNFA(new NFA(regExParser.GetDisjointCharSets().GetNumberOfElements()));
    for(const auto &refKV : this->refInput.GetLexerRules())
    {
        pNFA = regExParser.Parse(refKV.key);
        regExParser.PushNFA(pNFA);
        regExParser.Union();
    }

    pNFA = regExParser.PopNFA();
    pDFA = pNFA->ToDFA(alphabet);
    pDFA->Reduce();

    this->BuildTables(regExParser.GetDisjointCharSets());

    return true;
}