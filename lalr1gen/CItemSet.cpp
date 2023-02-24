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
#include "CItemSet.h"

//Public Functions
bool CItemSet::HasRule(CRule<CString> &rule) const
{
    repeat(this->rules.GetNoOfElements(), i)
    {
        if(this->rules[i].lhs == rule.lhs && this->rules[i].rhs.GetNoOfElements() == rule.rhs.GetNoOfElements())
        {
            repeat(rule.rhs.GetNoOfElements(), j)
            {
                if(this->rules[i].rhs[j] != rule.rhs[j])
                {
                    goto next;
                }
            }
            return true;
        }
        next:;
    }
    return false;
}

CString CItemSet::ToString() const
{
    CString text;

    repeat(this->rules.GetNoOfElements(), i)
    {
        text += this->rules[i].ToString() + "\r\n";
    }

    return text;
}

//Non-Class Operators
bool operator==(const CItemSet &lhs, const CItemSet &rhs)
{
    if(lhs.rules.GetNoOfElements() != rhs.rules.GetNoOfElements())
        return false;

    repeat(lhs.rules.GetNoOfElements(), i)
    {
        if(!((CItemSet &)lhs).rules[i].Equals(rhs.rules[i]))
            return false;
    }

    return true;
}

bool operator!=(const CItemSet &lhs, const CItemSet &rhs)
{
    return !(lhs == rhs);
}