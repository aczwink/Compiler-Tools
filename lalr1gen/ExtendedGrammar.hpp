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
//Namespaces
using namespace SJCLib;

template <typename T>
class CRule;

struct SExtendedGrammarEntity
{
    uint32 start;
    CString text;
    uint32 end;

    SExtendedGrammarEntity()
    {
        start = 0;
        end = 0;
    }
};

CString ToString(const SExtendedGrammarEntity &ent);
CRule<CString> ToNormalRule(CRule<SExtendedGrammarEntity> rule);
bool operator==(const SExtendedGrammarEntity &lhs, const SExtendedGrammarEntity &rhs);
bool operator!=(const SExtendedGrammarEntity &lhs, const SExtendedGrammarEntity &rhs);
bool operator<(const SExtendedGrammarEntity &lhs, const SExtendedGrammarEntity &rhs);
bool operator>(const SExtendedGrammarEntity &lhs, const SExtendedGrammarEntity &rhs);