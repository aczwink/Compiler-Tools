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
//Main Header
#include "ExtendedGrammar.h"
//Local
#include "CRule.h"

CString ToString(const SExtendedGrammarEntity &ent)
{
    return CString(ent.start) + ent.text + CString(ent.end);
}

CRule<CString> ToNormalRule(CRule<SExtendedGrammarEntity> rule)
{
    CRule<CString> tmp;

    tmp.lhs = rule.lhs.text;

    repeat(rule.rhs.GetNoOfElements(), i)
    {
        tmp.rhs.Push(rule.rhs[i].text);
    }

    return tmp;
}

bool operator==(const SExtendedGrammarEntity &lhs, const SExtendedGrammarEntity &rhs)
{
    return lhs.end == rhs.end && lhs.start == rhs.start && lhs.text == rhs.text;
}

bool operator!=(const SExtendedGrammarEntity &lhs, const SExtendedGrammarEntity &rhs)
{
    return !(lhs == rhs);
}

bool operator<(const SExtendedGrammarEntity &lhs, const SExtendedGrammarEntity &rhs)
{
    return ToString(lhs) < ToString(rhs);
}

bool operator>(const SExtendedGrammarEntity &lhs, const SExtendedGrammarEntity &rhs)
{
    return ToString(lhs) > ToString(rhs);
}