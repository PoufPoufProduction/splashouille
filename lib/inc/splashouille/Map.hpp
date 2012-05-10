/*
Copyright 2011 JohannC

This file is part of SPLASHOUILLE.

SPLASHOUILLE is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

SPLASHOUILLE is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
SPLASHOUILLE.  If not, see http://www.gnu.org/licenses/
*/

#ifndef SPLASHOUILLE_MAP_HPP_
#define SPLASHOUILLE_MAP_HPP_

#include <splashouille/Object.hpp>

namespace splashouille
{

/**
 * THE MAP OBJECT\n
 * Virtual object which handles tiles
 */
class Map : virtual public splashouille::Object
{
public:
    enum Mode { ortho = 0, iso };

    /** @return the tileset image */
    virtual splashouille::Image *                   getTileset() const = 0;

};

}


#endif

