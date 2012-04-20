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

#ifndef SPLASHOUILLEIMPL_SOLID_HPP_
#define SPLASHOUILLEIMPL_SOLID_HPP_

#include <splashouille/Defines.hpp>
#include <splashouille/Solid.hpp>
#include <splashouilleImpl/Object.hpp>

class SDL_Surface;
class SDL_Rect;

namespace splashouille
{
class Style;
}

namespace splashouilleImpl
{
class Library;

class Solid : virtual public splashouille::Solid, virtual public splashouilleImpl::Object
{
private:
    Solid(const std::string & _id, libconfig::Setting & _setting);
    Solid(const std::string & _id, Solid * _solid);
    Solid(const std::string & _id);
    ~Solid();

    int     red, green, blue;               // Save the background color in case of change

public:
    /** Accessors */
    bool                                    isSolid() const                             { return true; }

    /**
     * Render the object into the surface canvas
     * @param _surface is the surface to fill
     * @param _offset is the parent offset
     * @return true
     */
    bool render(SDL_Surface * _surface, SDL_Rect * _offset = 0);

    /**
     * Log the solid to the standard output
     * @param _rank is the log rank
     */
    void log(int _rank = 0) const;

    /**
     * Delete the surface when out of crowd
     * @return true
     */
    bool outCrowd();

    friend class Library;
};

}


#endif

 
 
 
