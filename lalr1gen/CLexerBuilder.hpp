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
#include "CInput.h"

class CLexerBuilder
{
private:
    //Members
    uint8 translate[256];
    uint32 *pTransitionTable;
    uint32 *pTokenTypeTable;
    //Functions
    const CString &FindAlphabetValue(uint32 stateIndex, const CMap<CString, uint32> &refAlphabet) const;
public:
    //Constructor
    CLexerBuilder();
    //Destructor
    ~CLexerBuilder();
    //Functions
    bool Build(const CInput &refInput);

    //Inline
    const uint32 *GetTokenTypeTable() const
    {
        return this->pTokenTypeTable;
    }

    const uint32 *GetTransitionsTable() const
    {
        return this->pTransitionTable;
    }

    const uint8 ((&GetTranslateTable())[256])
    {
        return translate;
    }
};