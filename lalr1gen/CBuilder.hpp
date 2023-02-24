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
#include "CInput.hpp"
#include "CItemSet.hpp"
#include "ExtendedGrammar.hpp"

class CBuilder
{
private:
    //Variables
    CInput *pInput;
    CArray<CItemSet> itemSets;
    CMap<uint64, uint32> translationsNt;
    CMap<uint64, uint32> translationsT;
    CGrammar<SExtendedGrammarEntity> extendedGrammar;
    CString **ppActionTable;
    CString **ppGotoTable;
    //Functions
    uint32 AddItemSet(CItemSet &itemSet);
    void AddRulesToItemSet(CItemSet &itemSet);
    void BuildExtendedGrammar();
    void BuildFirstSets();
    void BuildFollowSets();
    void BuildItemSet(CItemSet &itemSet);
    void BuildItemSets();
    void BuildReductions();
    void BuildTranslationTable();
    CString FindNonTerminalByIndex(uint32 index);
    CString FindTerminalByIndex(uint32 index);
    uint32 Give(CItemSet &itemSet, CString nonTerminalOrTerminal);
    void InitActionAndGotoTables();
    CString PrintSet(CFiniteSet<CString> &refSet);
    void Unite(CString nonTerminal1, CString nonTerminal2);
    void Unite2(CString nonTerminal1, CString nonTerminal2);
    void Unite3(CString nonTerminal1, CString nonTerminal2);
public:
    //Constructor
    CBuilder();
    //Destructor
    ~CBuilder();
    //Functions
    bool Build(CInput &input);
    void CopyTables(int32 *pActionTable, uint8 *pReductionRuleLengthTable, uint32 *pReductionRuleLeftSideTable, uint32 *pGotoTable);

    //Inline
    inline uint32 GetNumberOfNonTerminals() const
    {
        return this->pInput->nonTerminals.GetNoOfElements();
    }

    inline uint32 GetNumberOfTerminals() const
    {
        return this->pInput->terminals.GetNoOfElements();
    }
};