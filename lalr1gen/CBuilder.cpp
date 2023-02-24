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
#include "CBuilder.hpp"
//Local
#include "General.hpp"
//Namespaces
using namespace SJCLib;
//Definitions
#define ACTION_ACCEPT "CLALR1PARSER_ACTION_ACCEPT"
#define ACTION_ERROR "CLALR1PARSER_ACTION_ERROR"
#define GOTO_ERROR "CLALR1PARSER_GOTO_ERROR"

//Global Variables
extern AOutputStream *pOutput;

//Constructor
CBuilder::CBuilder()
{
    this->ppActionTable = NULL;
    this->ppGotoTable = NULL;
}

//Destructor
CBuilder::~CBuilder()
{
    repeat(this->itemSets.GetNoOfElements(), i)
    {
        HeapDeleteArray(this->ppActionTable[i]);
        HeapDeleteArray(this->ppGotoTable[i]);
    }
    HeapDeleteArray(this->ppActionTable);
    HeapDeleteArray(this->ppGotoTable);
}

//Private Functions
uint32 CBuilder::AddItemSet(CItemSet &itemSet)
{
    repeat(this->itemSets.GetNoOfElements(), i)
    {
        if(this->itemSets[i] == itemSet)
            return i;
    }
    return this->itemSets.Push(itemSet);
}

void CBuilder::AddRulesToItemSet(CItemSet &itemSet)
{
    repeat(itemSet.rules.GetNoOfElements(), i)
    {
        if(!itemSet.rules[i].IsAtEnd() && this->pInput->grammar.IsNonTerminal(*itemSet.rules[i]))
        {
            repeat(this->pInput->grammar.productionRules.GetNoOfElements(), j)
            {
                //if(this->pInput->rules[j].lhs == *itemSet.rules[i] && !itemSet.HasRule(this->pInput->rules[j]))
                if(this->pInput->grammar.productionRules[j].lhs == *itemSet.rules[i])
                {
                    if(itemSet.HasRule(this->pInput->grammar.productionRules[j]))
                    {
                        repeat(itemSet.rules.GetNoOfElements(), k)
                        {
                            if(this->pInput->grammar.productionRules[j].Equals(itemSet.rules[k]))
                                goto next;
                        }
                    }
                    itemSet.rules.Push(this->pInput->grammar.productionRules[j]);
                    this->AddRulesToItemSet(itemSet);
                }
                next:;
            }
        }
    }
}

void CBuilder::BuildExtendedGrammar()
{
    //Add the start rule
    /*CRule startRule;

    startRule.lhs = CString(0) + this->pInput->startSymbol + CString(ENDOFINPUT);
    this->extendedGrammarRules.Push(startRule);*/

    repeat(this->itemSets.GetNoOfElements(), i)
    {
        repeat(this->itemSets[i].rules.GetNoOfElements(), j)
        {
            CRule<CString> rule = this->itemSets[i].rules[j];

            if(rule.IsEpsilonRule())
            {
                bool b;

                b = true;
            }

            if(!rule.IsInFront())
                continue;

            if(this->translationsNt.Find(((uint64)i << 32) | this->pInput->nonTerminals[rule.lhs]).IsValid() || rule.lhs == this->pInput->startSymbol)
            {
                CRule<SExtendedGrammarEntity> newRule;
                SExtendedGrammarEntity ent;

                ent.start = i;

                while(!rule.IsAtEnd())
                {
                    if(this->pInput->grammar.IsNonTerminal(*rule))
                        ent.end = this->translationsNt[((uint64)ent.start << 32) | this->pInput->nonTerminals[*rule]];
                    else
                        ent.end = this->translationsT[((uint64)ent.start << 32) | this->pInput->terminals[*rule]];

                    ent.text = *rule;

                    if(this->pInput->grammar.IsNonTerminal(*rule))
                        this->extendedGrammar.nonTerminals.Insert(ent);
                    else
                        this->extendedGrammar.terminals.Insert(ent);

                    newRule.rhs.Push(ent);
                    ent.start = ent.end;
                    *rule++;
                }
                ent.start = i;
                ent.text = rule.lhs;
                ent.end = this->translationsNt[(((uint64)i << 32) | this->pInput->nonTerminals[rule.lhs])];
                newRule.lhs = ent;

                if(!rule.IsEpsilonRule())
                {
                    if(this->pInput->grammar.IsNonTerminal(rule.lhs))
                        this->extendedGrammar.nonTerminals.Insert(ent);
                    else
                        this->extendedGrammar.terminals.Insert(ent);
                }
                if(!this->extendedGrammar.productionRules.IsInArray(newRule))
                    this->extendedGrammar.productionRules.Push(newRule);
            }
        }
    }

#ifdef _DEBUG
    *pOutput << "Extended Grammar:" << endl;
	repeat(this->extendedGrammar.productionRules.GetNoOfElements(), i)
	{
		*pOutput << i << ". " << this->extendedGrammar.productionRules[i].ToString() << endl;
	}
	*pOutput << "-------------------------------------------------------------------" << endl;
#endif
}

void CBuilder::BuildItemSet(CItemSet &itemSet)
{
    CFiniteSet<CString> gives;

    repeat(itemSet.rules.GetNoOfElements(), i)
    {
        if(itemSet.rules[i].IsAtEnd() /*|| itemSet.rules[i].rhs[0] == this->pInput->epsilon*/)
            continue;
        gives.Insert(*itemSet.rules[i]);
    }
    for(CFiniteSetIterator<CString> it = gives.Begin(); it.IsValid(); it++)
    {
        this->Give(itemSet, it.GetValue());
    }
}

void CBuilder::BuildItemSets()
{
    CItemSet i0;

    repeat(this->pInput->grammar.productionRules.GetNoOfElements(), i)
    {
        if(this->pInput->grammar.productionRules[i].lhs == this->pInput->startSymbol)
        {
            i0.rules.Push(this->pInput->grammar.productionRules[i]);
        }
    }
    this->AddRulesToItemSet(i0);
    this->AddItemSet(i0);
    this->BuildItemSet(i0);

#ifdef _DEBUG
    *pOutput << "Itemsets:" << endl;
	repeat(this->itemSets.GetNoOfElements(), i)
	{
		*pOutput << "i" << i << ": " << this->itemSets[i].ToString() << endl;
	}
	*pOutput << "--------------------------------------------------------" << endl;
#endif
}

void CBuilder::BuildReductions()
{
    CMap<SExtendedGrammarEntity, CFiniteSet<SExtendedGrammarEntity>> firstSets, followSets;
    CArray<CFiniteSet<CString>> matchedFollowSets;
    CArray<CRule<SExtendedGrammarEntity>> productionRules;
    CArray<CFiniteSet<CString>> finalSets;
    CArray<uint32> ruleIndices;
    bool *pValid;
    uint32 index;
    bool merged;

    productionRules = this->extendedGrammar.productionRules;
    pValid = new bool[this->itemSets.GetNoOfElements()];
    this->extendedGrammar.BuildFirstSets(firstSets);
    this->extendedGrammar.BuildFollowSets(followSets, firstSets);

#ifdef _DEBUG
    *pOutput << "First sets:" << endl;
	for(CMapIterator<SExtendedGrammarEntity, CFiniteSet<SExtendedGrammarEntity>> it = firstSets.Begin(); it.IsValid(); it++)
	{
		*pOutput << it.GetKey().text << " {";
		for(CFiniteSetIterator<SExtendedGrammarEntity> it2 = it.GetValue().Begin(); it2.IsValid(); it2++)
		{
			*pOutput << it2.GetValue().text << ", ";
		}
		*pOutput << "}" << endl;
	}
	*pOutput << "-----------------------------------------------------------------" << endl << endl;
#endif

    MemZero(pValid, this->itemSets.GetNoOfElements() * sizeof(*pValid));

    repeat(productionRules.GetNoOfElements(), i)
    {
        CMapIterator<SExtendedGrammarEntity, CFiniteSet<SExtendedGrammarEntity>> it;

        index = matchedFollowSets.Push(CFiniteSet<CString>());
        for(CFiniteSetIterator<SExtendedGrammarEntity> it = followSets.Find(productionRules[i].lhs).GetValue().Begin(); it.IsValid(); it++)
        {
            matchedFollowSets[index].Insert(it.GetValue().text);
        }
    }

    repeat(this->itemSets.GetNoOfElements(), i)
    {
        finalSets.Push(CFiniteSet<CString>());
        ruleIndices.Push(0);
    }

#ifdef _DEBUG
    *pOutput << "Extended Grammar with Follow Sets:" << endl;
	repeat(productionRules.GetNoOfElements(), i)
	{
		*pOutput << i << ". " << productionRules[i].ToString()
			<< "\t\tFollow set: " <<  this->PrintSet(matchedFollowSets[i]) << endl;
	}
	*pOutput << "-------------------------------------------------------------------" << endl;
#endif

    repeat(productionRules.GetNoOfElements(), i)
    {
        uint32 end1, end2;

        merged = false;

        if(productionRules[i].lhs.text == this->pInput->startSymbol)
            continue;

        for(uint32 j = i+1; j < productionRules.GetNoOfElements(); j++)
        {
            if(productionRules[i].IsEpsilonRule())
                end1 = productionRules[i].lhs.start;
            else
                end1 = productionRules[i].GetLast().end;
            if(productionRules[j].IsEpsilonRule())
                end2 = productionRules[j].lhs.start;
            else
                end2 = productionRules[j].GetLast().end;
            if(ToNormalRule(productionRules[i]) == ToNormalRule(productionRules[j]) && end1 == end2)
            {
                finalSets[end1].Union(matchedFollowSets[i]);
                finalSets[end1].Union(matchedFollowSets[j]);
                pValid[end1] = true;
                merged = true;
            }
        }
        if(!merged)
        {
            if(productionRules[i].IsEpsilonRule())
                end1 = productionRules[i].lhs.start;
            else
                end1 = productionRules[i].GetLast().end;
            finalSets[end1].Union(matchedFollowSets[i]);
            pValid[end1] = true;
            ruleIndices[end1] = this->pInput->grammar.productionRules.Find(ToNormalRule(productionRules[i]));
        }
    }

#ifdef _DEBUG
    *pOutput << "Final Sets:" << endl;
	repeat(this->itemSets.GetNoOfElements(), i)
	{
		*pOutput << i << ". ";
		if(pValid[i] && this->pInput->grammar.productionRules[ruleIndices[i]].lhs != this->pInput->startSymbol)
			*pOutput << this->pInput->grammar.productionRules[ruleIndices[i]].ToString() << "\t\tFinal: " << finalSets[i].GetNoOfElements();

		*pOutput << endl;
	}
	*pOutput << "-----------------------------------------------------------------" << endl << endl;
#endif

    repeat(this->itemSets.GetNoOfElements(), i)
    {
        if(pValid[i] && this->pInput->grammar.productionRules[ruleIndices[i]].lhs != this->pInput->startSymbol) //is row valid
        {
            for(CFiniteSetIterator<CString> it = finalSets[i].Begin(); it.IsValid(); it++)
            {
                CRule<CString> &rule = this->pInput->grammar.productionRules[ruleIndices[i]];

                if((this->pInput->grammar.rulePrecedences[&rule] != 0) && (this->pInput->grammar.opPrecedences[it.GetValue()] != 0) && (this->pInput->grammar.rulePrecedences[&rule] < this->pInput->grammar.opPrecedences[it.GetValue()]))
                {
                    //stdOut << this->pInput->grammar.rulePrecedences[&rule] << " " << this->pInput->grammar.opPrecedences[it.GetValue()] << endl;
                    //this->ppActionTable[i][this->pInput->terminals[it.GetValue()]] = this->Give(this->itemSets[i], it.GetValue());
                    continue;
                }

                this->ppActionTable[i][this->pInput->terminals[it.GetValue()]] = '-' + CString(ruleIndices[i]+1);
            }
        }
    }

    HeapDeleteArray(pValid);
}

void CBuilder::BuildTranslationTable()
{
    repeat(this->itemSets.GetNoOfElements(), i)
    {
        CFiniteSet<CString> gives;

        repeat(this->itemSets[i].rules.GetNoOfElements(), j)
        {
            if(this->itemSets[i].rules[j].IsAtEnd() /*|| this->itemSets[i].rules[j].rhs[0] == this->pInput->epsilon*/)
                continue;
            gives.Insert(*this->itemSets[i].rules[j]);
        }
        for(CFiniteSetIterator<CString> it = gives.Begin(); it.IsValid(); it++)
        {
            CItemSet newItemSet;
            uint32 end;

            repeat(this->itemSets[i].rules.GetNoOfElements(), j)
            {
                if(this->itemSets[i].rules[j].IsAtEnd())
                    continue;
                if(*this->itemSets[i].rules[j] == it.GetValue())
                {
                    newItemSet.rules[newItemSet.rules.Push(this->itemSets[i].rules[j])]++;
                }
            }
            this->AddRulesToItemSet(newItemSet);
            end = this->AddItemSet(newItemSet);

            if(this->pInput->grammar.IsNonTerminal(it.GetValue()))
            {
                this->translationsNt.Insert(((uint64)i << 32) | (this->pInput->nonTerminals[it.GetValue()]), end);
            }
            else
            {
                this->translationsT.Insert(((uint64)i << 32) | (this->pInput->terminals[it.GetValue()]), end);
            }
        }
    }
}

CString CBuilder::FindNonTerminalByIndex(uint32 index)
{
    for(CMapIterator<CString, uint32> it = this->pInput->nonTerminals.Begin(); it.IsValid(); it++)
    {
        if(it.GetValue() == index)
            return it.GetKey();
    }
    return CString();
}

CString CBuilder::FindTerminalByIndex(uint32 index)
{
    for(CMapIterator<CString, uint32> it = this->pInput->terminals.Begin(); it.IsValid(); it++)
    {
        if(it.GetValue() == index)
            return it.GetKey();
    }
    return CString();
}

uint32 CBuilder::Give(CItemSet &itemSet, CString nonTerminalOrTerminal)
{
    uint32 index;
    CItemSet newItemSet;

    index = 0;

    repeat(itemSet.rules.GetNoOfElements(), i)
    {
        if(itemSet.rules[i].IsAtEnd())
            continue;
        if(*itemSet.rules[i] == nonTerminalOrTerminal)
        {
            newItemSet.rules[newItemSet.rules.Push(itemSet.rules[i])]++;
        }
    }
    this->AddRulesToItemSet(newItemSet);
    if(itemSet != newItemSet)
    {
        uint32 nItemSets;

        nItemSets = this->itemSets.GetNoOfElements();
        index = this->AddItemSet(newItemSet);
        if(this->itemSets.GetNoOfElements() != nItemSets)
            this->BuildItemSet(newItemSet);
    }

    return index;
}

void CBuilder::InitActionAndGotoTables()
{
    this->ppActionTable = new CString *[this->itemSets.GetNoOfElements()];
    this->ppGotoTable = new CString *[this->itemSets.GetNoOfElements()];

    //Step 1
    repeat(this->itemSets.GetNoOfElements(), i)
    {
        this->ppActionTable[i] = new CString[this->pInput->terminals.GetNoOfElements()];
        this->ppGotoTable[i] = new CString[this->pInput->nonTerminals.GetNoOfElements()];

        repeat(this->pInput->terminals.GetNoOfElements(), j)
        {
            this->ppActionTable[i][j] = ACTION_ERROR;

            if(/*this->itemSets[i].rules.GetLength() == 1 &&*/ this->itemSets[i].rules[0].lhs == this->pInput->startSymbol && this->itemSets[i].rules[0].IsAtEnd() && this->pInput->terminals[this->pInput->endOfInput] == j)
                this->ppActionTable[i][j] = ACTION_ACCEPT;
        }

        repeat(this->pInput->nonTerminals.GetNoOfElements(), j)
        {
            this->ppGotoTable[i][j] = GOTO_ERROR;
        }
    }

    //Step 2
    for(CMapIterator<uint64, uint32> it = this->translationsNt.Begin(); it.IsValid(); it++)
    {
        this->ppGotoTable[it.GetKey() >> 32][it.GetKey() & 0xFFFFFFFF] = CString(it.GetValue());
    }

    //Step 3
    for(CMapIterator<uint64, uint32> it = this->translationsT.Begin(); it.IsValid(); it++)
    {
        this->ppActionTable[it.GetKey() >> 32][it.GetKey() & 0xFFFFFFFF] = CString(it.GetValue());
    }
}

CString CBuilder::PrintSet(CFiniteSet<CString> &refSet)
{
    CString text;

    text = "{";

    for(CFiniteSetIterator<CString> it = refSet.Begin(); it.IsValid(); it++)
    {
        text += ToString(it.GetValue()) + ", ";
    }
    text += "}";

    return text;
}

//First(1) = First(1) U First(2)
/*void CBuilder::Unite(CString nonTerminal1, CString nonTerminal2)
{
	CMapIterator<CString, CFiniteSet<CString>> it;
	CFiniteSet<CString> &refNonTerminal1Set = this->firstSets.Find(nonTerminal1).GetValue();

	it = this->firstSets.Find(nonTerminal2);
	for(CFiniteSetIterator<CString> itInner = it.GetValue().GetIterator(); itInner.IsValid(); itInner++)
	{
		if(itInner.GetValue() != this->pInput->epsilon)
			refNonTerminal1Set.Insert(itInner.GetValue());
	}
}

//Follow(1) = Follow(1) U Follow(2)
void CBuilder::Unite2(CString nonTerminal1, CString nonTerminal2)
{
	CMapIterator<CString, CFiniteSet<CString>> it;
	CFiniteSet<CString> &refFollowSet1 = this->followSets.Find(nonTerminal1).GetValue();

	it = this->followSets.Find(nonTerminal2);
	for(CFiniteSetIterator<CString> itInner = it.GetValue().GetIterator(); itInner.IsValid(); itInner++)
	{
		refFollowSet1.Insert(itInner.GetValue());
	}
}

//Follow(1) = Follow(1) U First(2)
void CBuilder::Unite3(CString nonTerminal1, CString nonTerminal2)
{
	CMapIterator<CString, CFiniteSet<CString>> it;
	CFiniteSet<CString> &refFollowSet1 = this->followSets.Find(nonTerminal1).GetValue();

	it = this->firstSets.Find(nonTerminal2);
	for(CFiniteSetIterator<CString> itInner = it.GetValue().GetIterator(); itInner.IsValid(); itInner++)
	{
		if(itInner.GetValue() != this->pInput->epsilon)
			refFollowSet1.Insert(itInner.GetValue());
	}
}*/

//Public Functions
bool CBuilder::Build(CInput &input)
{
    CFileOutputStream file;
    this->pInput = &input;
    SExtendedGrammarEntity ent;
    uint32 longestActionEntry, longestGotoEntry;

    ent.end = 0;
    ent.start = 0;
    ent.text = input.startSymbol;
    this->extendedGrammar.startSymbol = ent;
    ent.text = input.endOfInput;
    this->extendedGrammar.endOfInput = ent;

    if(!file.Open(L"lalrtables.h"))
        return false;

#ifdef _DEBUG
    *pOutput << "Input Rules:" << endl;
	repeat(this->pInput->grammar.productionRules.GetNoOfElements(), i)
	{
		*pOutput << this->pInput->grammar.productionRules[i].ToString() << endl;
	}
	*pOutput << "---------------------------------------------------------------" << endl;
#endif

    this->BuildItemSets();
    this->BuildTranslationTable();
    this->BuildExtendedGrammar();
    this->InitActionAndGotoTables();
    this->BuildReductions();

    longestActionEntry = (uint32)strlen(ACTION_ACCEPT);
    for(CMapIterator<CString, uint32> it = this->pInput->terminals.Begin(); it.IsValid(); it++)
    {
        if(it.GetKey().GetLength() > longestActionEntry)
            longestActionEntry = it.GetKey().GetLength();
    }

    longestGotoEntry = (uint32)strlen(GOTO_ERROR);
    for(CMapIterator<CString, uint32> it = this->pInput->nonTerminals.Begin(); it.IsValid(); it++)
    {
        if(it.GetKey().GetLength() > longestGotoEntry)
            longestGotoEntry = it.GetKey().GetLength();
    }

    //Dump Definitions
    file << "//Definitions" << endl
         << "#define NUMBER_OF_ALPHABET_ELEMENTS " << this->pInput->terminals.GetNoOfElements() << endl
         << "#define NUMBER_OF_NONTERMINALS " << this->pInput->nonTerminals.GetNoOfElements() << endl
         << endl;

    //Action Table
    file << "static const int32 actionTable[] =" << endl << '{' << endl;
    file << "//\t\t";
    repeat(this->pInput->terminals.GetNoOfElements(), i)
    {
        CString term;

        term = this->FindTerminalByIndex(i);
        repeat(longestActionEntry - term.GetLength(), k)
        {
            file << ' ';
        }
        file << term << ", ";
    }
    file << endl;
    repeat(this->itemSets.GetNoOfElements(), i)
    {
        file << "/*" << i << "*/\t";
        repeat(this->pInput->terminals.GetNoOfElements(), j)
        {
            repeat(longestActionEntry - this->ppActionTable[i][j].GetLength(), k)
            {
                file << ' ';
            }
            file << this->ppActionTable[i][j] << ", ";
        }
        file << endl;
    }
    file << "};" << endl << endl;

    //Goto Table
    file << "static const uint32 gotoTable[] =" << endl << '{' << endl;
    file << "//\t\t";
    repeat(this->pInput->nonTerminals.GetNoOfElements(), i)
    {
        CString nt;

        nt = this->FindNonTerminalByIndex(i);
        repeat(longestGotoEntry - nt.GetLength(), k)
        {
            file << ' ';
        }
        file << nt << ", ";
    }
    file << endl;
    repeat(this->itemSets.GetNoOfElements(), i)
    {
        file << "/*" << i << "*/\t";
        repeat(this->pInput->nonTerminals.GetNoOfElements(), j)
        {
            repeat(longestGotoEntry-this->ppGotoTable[i][j].GetLength(), k)
            {
                file << ' ';
            }
            file << this->ppGotoTable[i][j] << ", ";
        }
        file << endl;
    }
    file << "};" << endl << endl;

    //Reduction Rule Length Table
    file << "static const uint8 reductionRuleLengthTable[] =" << endl << '{' << endl << "\t0xFF/*illegal*/, ";
    repeat(this->pInput->grammar.productionRules.GetNoOfElements(), i)
    {
        file << this->pInput->grammar.productionRules[i].rhs.GetNoOfElements() << ", ";
    }
    file << endl << "};" << endl << endl;

    //Reduction Rule Left Side Table
    file << "static const uint32 reductionRuleLeftSideTable[] =" << endl << '{' << endl << "\tUINT32_MAX/*illegal*/, ";
    repeat(this->pInput->grammar.productionRules.GetNoOfElements(), i)
    {
        file << this->pInput->nonTerminals[this->pInput->grammar.productionRules[i].lhs] << ", ";
    }
    file << endl << "};" << endl << endl;

    //Print rules
    file << "/*" << endl;
    repeat(this->pInput->grammar.productionRules.GetNoOfElements(), i)
    {
        file << i+1 << ". " << this->pInput->grammar.productionRules[i].ToString() << endl;
    }
    file << "*/";

    return true;
}

void CBuilder::CopyTables(int32 *pActionTable, uint8 *pReductionRuleLengthTable, uint32 *pReductionRuleLeftSideTable, uint32 *pGotoTable)
{
    uint32 i, rowIndex;

    rowIndex = 0;
    repeat(this->itemSets.GetNoOfElements(), i)
    {
        repeat(this->pInput->terminals.GetNoOfElements(), j)
        {
            if(this->ppActionTable[i][j] == ACTION_ERROR)
                pActionTable[rowIndex + j] = CLALR1PARSER_ACTION_ERROR;
            else if(this->ppActionTable[i][j] == ACTION_ACCEPT)
                pActionTable[rowIndex + j] = CLALR1PARSER_ACTION_ACCEPT;
            else
                pActionTable[rowIndex + j] = (int32)this->ppActionTable[i][j].ToInt64();
        }
        rowIndex += this->pInput->terminals.GetNoOfElements();
    }

    pReductionRuleLeftSideTable[0] = -1;
    pReductionRuleLengthTable[0] = -1;
    for(i = 0; i < this->pInput->grammar.productionRules.GetNoOfElements(); i++)
    {
        pReductionRuleLengthTable[i+1] = this->pInput->grammar.productionRules[i].rhs.GetNoOfElements();
        pReductionRuleLeftSideTable[i+1] = this->pInput->nonTerminals[this->pInput->grammar.productionRules[i].lhs];
    }

    rowIndex = 0;
    repeat(this->itemSets.GetNoOfElements(), i)
    {
        repeat(this->pInput->nonTerminals.GetNoOfElements(), j)
        {
            if(this->ppGotoTable[i][j] == GOTO_ERROR)
                pGotoTable[rowIndex + j] = CLALR1PARSER_GOTO_ERROR;
            else
                pGotoTable[rowIndex + j] = (uint32)this->ppGotoTable[i][j].ToUInt64();
        }
        rowIndex += this->pInput->nonTerminals.GetNoOfElements();
    }
}