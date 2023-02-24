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
#include "CInput.hpp"
#include "CLexerBuilder.h"
//Namespaces
using namespace SJCLib;

//Global Variables
extern CFileInputStream g_simInput;
extern CFileOutputStream g_simResult;

CString BackresolveTerminal(uint32 terminal, const CMap<CString, uint32> &refTerminals)
{
    foreach(it, refTerminals)
    {
        if(it.GetValue() == terminal)
            return it.GetKey();
    }

    return "";
}

void RunSimulation(CInput &refInput, CLexerBuilder &refLexBuilder, CBuilder &refParserBuilder)
{
    byte *pInput;
    uint8 reductionRuleLengthTable[1000];
    int32 actionTable[10000];
    uint32 reductionRuleLeftSideTable[1000];
    uint32 gotoTable[10000];
    CString line;

    class SimulationLexer : public CLexer<uint32>
    {
    private:
        //Methods
        bool OnNextToken(const uint32 &refParsedToken)
        {
            if(ignoringTokens.Contains(refParsedToken))
            {
                g_simResult << "Ignoring token: " << BackresolveTerminal(refParsedToken, this->pInput->GetTerminalsSet()) << endl;
                return false;
            }

            g_simResult << "Setting new lookAhead: " << BackresolveTerminal(refParsedToken, this->pInput->GetTerminalsSet()) << endl;

            return true;
        }
    public:
        //Members
        CInput *pInput;
        CFiniteSet<uint32> ignoringTokens;
        //Constructor
        SimulationLexer(const uint8 *pTranslateTable, const uint32 *pTransitionTable, const uint32 *pTokenTypeTable) : CLexer<uint32>(pTranslateTable, pTransitionTable, pTokenTypeTable)
        {
        }
    };

    class SimulationParser : public CLALR1Parser<uint32, void*>
    {
    private:
        //Methods
        void OnPutToken(const uint32 &refTerminal, const CString &refString, void* &refOutput)
        {
            refOutput = NULL;
            g_simResult << "Putting token: (" << BackresolveTerminal(refTerminal, this->pInput->GetTerminalsSet()) << ", " << refString << ')' << endl;
        }

        void OnReduce(int32 reductionRule, void* &refOutput)
        {
            refOutput = NULL;
            g_simResult << "Reducing by rule " << reductionRule << ", " << pInput->GetGrammar().productionRules[reductionRule-1].ToString() << endl;
        }
    public:
        //Members
        CInput *pInput;
        //Constructor
        SimulationParser(uint32 nTerminals, uint32 nNonTerminals, const int32 *pActionTable, const uint8 *pReductionRuleLengthTable, const uint32 *pReductionRuleLeftSideTable, const uint32 *pGotoTable) : CLALR1Parser(nTerminals, nNonTerminals, pActionTable, pReductionRuleLengthTable, pReductionRuleLeftSideTable, pGotoTable)
        {
        }
    };

    refParserBuilder.CopyTables(actionTable, reductionRuleLengthTable, reductionRuleLeftSideTable, gotoTable);

    SimulationLexer &refLex = SimulationLexer(refLexBuilder.GetTranslateTable(), refLexBuilder.GetTransitionsTable(), refLexBuilder.GetTokenTypeTable());
    SimulationParser &refParser = SimulationParser(refParserBuilder.GetNumberOfTerminals(), refParserBuilder.GetNumberOfNonTerminals(), actionTable, reductionRuleLengthTable, reductionRuleLeftSideTable, gotoTable);
    CTextReader &refReader = CTextReader(g_simInput);

    refLex.pInput = &refInput;
    refParser.pInput = &refInput;

    while(true)
    {
        line = refReader.ReadASCIILine();

        if(line == "simstart;")
            break;
        else if(line == "simignore")
        {
        }
        else
        {
            refLex.ignoringTokens.Insert(refInput.GetTerminalsSet()[line]);
        }
    }

    pInput = (byte *)malloc(g_simInput.GetRemainingBytes()+1);
    pInput[g_simInput.GetRemainingBytes()] = 0;
    g_simInput.ReadBytes(pInput, g_simInput.GetRemainingBytes());

    refLex.SetInput(pInput);
    if(refParser.Parse(refLex))
    {
        g_simResult << endl << "Input successfully parsed." << endl;
    }
    else
    {
        g_simResult << endl << "Input is not part of the language." << endl;
    }

    free(pInput);
}