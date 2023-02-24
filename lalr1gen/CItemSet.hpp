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
#include "CRule.hpp"

class CItemSet
{
public:
    //Variables
    CArray<CRule<CString>> rules;
    //Functions
    bool HasRule(CRule<CString> &rule) const;
    CString ToString() const;
};

bool operator==(const CItemSet &lhs, const CItemSet &rhs);
bool operator!=(const CItemSet &lhs, const CItemSet &rhs);