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
#include "Input.hpp"

class LexerBuilder
{
private:
    //Members
    const Input &refInput;
    FileOutputStream cppFile;
    //Methods
    void BuildTables(const LinkedList<FormalLanguages::CharacterClass *> &refCharSets);
    void BuildTranslateTable(const LinkedList<FormalLanguages::CharacterClass *> &refCharSets);
public:
    //Constructor
    LexerBuilder(const Input &refInput);
    //Methods
    bool Build();
};