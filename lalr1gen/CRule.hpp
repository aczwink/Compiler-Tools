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
#include "ExtendedGrammar.hpp"
//Namespaces
using namespace SJCLib;

CString ToString(CString str);

template <typename T>
class CRule
{
private:
    //Variables
    uint32 pointer;
public:
    //Variables
    T lhs;
    CArray<T> rhs;

    //Constructor
    CRule()
    {
        this->pointer = 0;
    }

    CRule(const CRule &other)
    {
        this->lhs = other.lhs;
        this->pointer = other.pointer;
        this->rhs = other.rhs;
    }

    //Operators
    CRule operator++(int)
    {
        CRule tmp;

        tmp = *this;
        this->pointer++;

        return tmp;
    }

    T operator*()
    {
        if(this->rhs.IsEmpty())
            return T();
        return this->rhs[this->pointer];
    }

    //Functions
    bool Equals(const CRule<T> &other) const
    {
        if(this->lhs == other.lhs && this->pointer == other.pointer && this->rhs.GetNoOfElements() == other.rhs.GetNoOfElements())
        {
            repeat(this->rhs.GetNoOfElements(), i)
            {
                if(this->rhs[i] != other.rhs[i])
                    return false;
            }
            return true;
        }
        return false;
    }

    bool IsEpsilonRule()
    {
        return !this->rhs.GetNoOfElements();
    }

    bool IsInFront()
    {
        return !this->pointer;
    }

    bool IsAtEnd()
    {
        return this->pointer == this->rhs.GetNoOfElements();
    }

    bool IsAtLast()
    {
        return this->pointer == this->rhs.GetNoOfElements()-1;
    }

    T GetLast()
    {
        if(this->rhs.IsEmpty())
            return T();
        return this->rhs[this->rhs.GetNoOfElements()-1];
    }

    CString ToString() const
    {
        CString text;

        text = ::ToString(this->lhs) + " -> ";

        repeat(this->rhs.GetNoOfElements(), i)
        {
            if(this->pointer == i)
                text += "* ";
            text += ::ToString(this->rhs[i]) + ' ';
        }
        if(this->pointer == this->rhs.GetNoOfElements())
            text += '*';

        return text;
    }
};

template <typename T>
bool operator==(const CRule<T> &lhs, const CRule<T> &rhs)
{
    return lhs.Equals(rhs);
}