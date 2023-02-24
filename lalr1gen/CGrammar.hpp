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
//Global
#include <deque>
//Local
#include "CRule.hpp"
#include "General.hpp"
//Namespaces
using namespace SJCLib;

//G = (V, Sigma, P, S)
template <typename T>
class CGrammar
{
private:
    //Variables
    T epsilon;

    //Functions
    bool BuildFirstSetsStep3To6(CMap<T, CFiniteSet<T>> &refSets, T &x, uint32 index)
    {
        bool changed;

        changed = false;

        repeat(this->productionRules.GetNoOfElements(), i)
        {
            if(this->productionRules[i].lhs == x && this->productionRules[i].IsEpsilonRule() && !refSets[x].Find(this->epsilon).IsValid())
            {
                refSets[x].Insert(this->epsilon);
                changed = true;
            }
        }

        repeat(this->productionRules.GetNoOfElements(), i)
        {
            if(this->productionRules[i].lhs == x && !this->productionRules[i].IsEpsilonRule() && index < this->productionRules[i].rhs.GetNoOfElements() && this->IsTerminal(this->productionRules[i].rhs[index]) && !refSets[x].Find(this->productionRules[i].rhs[index]).IsValid())
            {
                refSets[x].Insert(this->productionRules[i].rhs[index]);
                changed = true;
            }
        }

        repeat(this->productionRules.GetNoOfElements(), i)
        {
            if(this->productionRules[i].lhs == x && !this->productionRules[i].IsEpsilonRule() && index < this->productionRules[i].rhs.GetNoOfElements() && this->IsNonTerminal(this->productionRules[i].rhs[index]) && this->productionRules[i].rhs[index] != x)
            {
                CFiniteSet<T> tmp1, tmp2;

                tmp1 = refSets[this->productionRules[i].rhs[index]];
                tmp1.Delete(this->epsilon);

                tmp2 = refSets[x];

                tmp2.Union(tmp1);
                if(tmp2 != refSets[x])
                {
                    refSets[x].Union(tmp1);
                    changed = true;
                }
            }
        }

        repeat(this->productionRules.GetNoOfElements(), i)
        {
            if(this->productionRules[i].lhs == x && index < this->productionRules[i].rhs.GetNoOfElements() && refSets[x].Find(this->epsilon).IsValid())
            {
                return changed || this->BuildFirstSetsStep3To6(refSets, x, index+1);
            }
        }
        return changed;
    }

    void BuildFollowSetFirstUnite(CMap<T, CFiniteSet<T>> &refSets, const CMap<T, CFiniteSet<T>> &refFirstSets, T x, CRule<T> rule, uint32 index)
    {
        CFiniteSet<T> tmp;
        T next;

        if(rule.rhs.GetLength() == index)
            return;
        next = rule.rhs[index];

        tmp = refFirstSets[next];
        tmp.Delete(this->epsilon);
        refSets[x].Union(tmp);
        if(refSets[next].Find(this->epsilon).IsValid()) //Epsilon is in FIRST(next)
        {
            this->BuildFollowSetFirstUnite(refSets, refFirstSets, x, rule, index+1);
        }
    }

    void BuildFollowSetsStep2(CMap<T, CFiniteSet<T>> &refSets, const CMap<T, CFiniteSet<T>> &refFirstSets, T &x, uint32 index)
    {
        repeat(this->productionRules.GetLength(), i)
        {
            T current, next;

            //Rule is at end
            if(this->productionRules[i].rhs.GetLength() == index || this->productionRules[i].rhs.GetLength() == index+1)
                continue;

            current = this->productionRules[i].rhs[index];
            next = this->productionRules[i].rhs[index+1];

            if(current == x && this->IsTerminal(next))
            {
                refSets[x].Insert(next);
            }
            else if(current == x && this->IsNonTerminal(next))
            {
                this->BuildFollowSetFirstUnite(refSets, refFirstSets, x, this->productionRules[i], index+1);
            }
        }
    }

    void BuildFollowSetsStep3And4(CMap<T, CFiniteSet<T>> &refSets, const CMap<T, CFiniteSet<T>> &refFirstSets, T &x)
    {
        repeat(this->productionRules.GetLength(), i)
        {
        }
    }
public:
    //Variables
    CFiniteSet<T> nonTerminals;
    CFiniteSet<T> terminals;
    CArray<CRule<T>> productionRules;
    T startSymbol;
    T endOfInput;
    CMap<T, uint32> opPrecedences;
    CMap<CRule<T> *, uint32> rulePrecedences;

    //Functions
    void BuildFirstSets(CMap<T, CFiniteSet<T>> &refSets)
    {
        std::deque<T> queue;

        for(CFiniteSetIterator<T> it = this->nonTerminals.Begin(); it.IsValid(); it++)
        {
            queue.push_back(it.GetValue());
        }

        for(CFiniteSetIterator<T> it = this->nonTerminals.Begin(); it.IsValid(); it++)
        {
            queue.push_back(it.GetValue());
        }

        while(!queue.empty())
        {
            T x;

            x = queue.front();
            queue.pop_front();

            if(BuildFirstSetsStep3To6(refSets, x, 0))
                queue.push_back(x);
        }
    }

    void BuildFollowSets(CMap<T, CFiniteSet<T>> &refSets, const CMap<T, CFiniteSet<T>> &refFirstSets)
    {
        /*std::deque<T> queue;

        //Init all sets with {}
        for(CFiniteSetIterator<T> it = this->nonTerminals.GetIterator(); it.IsValid(); it++)
        {
            refSets[it.GetValue()] = CFiniteSet<T>();
        }

        //Follow(S) = {$}
        refSets[this->startSymbol].Insert(this->endOfInput);

        //Put all nonterminals that are not the startsymbol on the queue
        for(CFiniteSetIterator<T> it = this->nonTerminals.GetIterator(); it.IsValid(); it++)
        {
            if(it.GetValue() != this->startSymbol)
                queue.push_back(it.GetValue());
        }

        while(!queue.empty())
        {
            T x;

            x = queue.front();
            queue.pop_front();

            this->BuildFollowSetsStep2(refSets, refFirstSets, x, 0);
        }

        //Put all nonterminals that are not the startsymbol on the queue
        for(CFiniteSetIterator<T> it = this->nonTerminals.GetIterator(); it.IsValid(); it++)
        {
            if(it.GetValue() != this->startSymbol)
                queue.push_back(it.GetValue());
        }

        //Put all nonterminals that are not the startsymbol on the queue
        for(CFiniteSetIterator<T> it = this->nonTerminals.GetIterator(); it.IsValid(); it++)
        {
            if(it.GetValue() != this->startSymbol)
                queue.push_back(it.GetValue());
        }

        while(!queue.empty())
        {
            T x;

            x = queue.front();
            queue.pop_front();

            this->BuildFollowSetsStep3And4(refSets, refFirstSets, x);
        }*/
        //Init all sets with {}
        for(CFiniteSetIterator<T> it = this->nonTerminals.Begin(); it.IsValid(); it++)
        {
            refSets[it.GetValue()] = CFiniteSet<T>();
        }

        refSets[this->startSymbol].Insert(this->endOfInput);

        repeat(this->productionRules.GetNoOfElements(), i)
        {
            CRule<T> rule = this->productionRules[i];

            repeat(rule.rhs.GetNoOfElements(), j)
            {
                if(this->IsNonTerminal(*rule))
                {
                    if(rule.IsAtLast())
                    {
                        refSets[*rule].Union(refSets[rule.lhs]);
                    }
                    else
                    {
                        CRule<T> tmp;

                        tmp = rule;
                        tmp++;

                        if(this->IsNonTerminal(*tmp))
                        {
                            refSets[*rule].Union(refFirstSets[*tmp]);
                            refSets[*rule].Delete(this->epsilon);

                            if(refFirstSets[*tmp].Find(this->epsilon).IsValid())
                            {
                                refSets[*rule].Union(refSets[rule.lhs]);
                            }
                        }
                        else
                        {
                            refSets[*rule].Insert(*tmp);
                        }
                    }
                }
                rule++;
            }
        }
    }

    bool IsNonTerminal(T input)
    {
        return this->nonTerminals.Find(input).IsValid();
    }

    bool IsTerminal(T input)
    {
        return this->terminals.Find(input).IsValid();
    }
};