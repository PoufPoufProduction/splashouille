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

#ifndef SPLASHOUILLEIMPL_MAP_HPP_
#define SPLASHOUILLEIMPL_MAP_HPP_

#include <splashouille/Defines.hpp>
#include <splashouille/Map.hpp>
#include <splashouille/Crowd.hpp>
#include <splashouilleImpl/Object.hpp>
#include <splashouilleImpl/Style.hpp>

class SDL_Surface;
class SDL_Rect;

namespace splashouille
{
class Style;
class Image;
class Library;
}

namespace splashouilleImpl
{
class Crowd;

class Map : virtual public splashouille::Map, virtual public splashouilleImpl::Object, virtual public splashouille::Crowd::Listener
{
private:
    Map(const std::string & _id, libconfig::Setting & _setting, splashouille::Library * _library);
    Map(const std::string & _id, Map * _map, splashouille::Library * _library);
    Map(const std::string & _id, splashouille::Library * _library);
    ~Map();

    static int                              counter;    // Number of maps

    typedef struct { int position[4]; }     limits;     // left, top, right, bottom
    enum Action { updateTiles = 0, removeTiles = 1 };   // Parsing tiles action

    splashouilleImpl::Style                 lastStyle;  // Style during the last update
    splashouilleImpl::Crowd *               crowd;      // The parent animation crowd
    splashouille::Library *                 library;    // The root library for creating tiles in a dynamic way
    splashouille::Image *                   tileset;    // The tileset object reference
    std::list<splashouille::Object*>        toDelete;   // The tiles to delete list (can not delete objects from onObject callback)
    int                                     size[4];    // The size of the map
    int *                                   map;        // The map array
    Mode                                    mode;       // The map mode (ortho or iso)
    bool                                    first;      // Is it the first update
    limits                                  current;    // The current view limits
    limits                                  last;       // The last update view limits

    /**
     * Compute the limits of the viewed part of the map regarding its position and size
     * @param _style is the current style of the maps
     * @param _limits are the returned computed limits
    */
    void getLimits(const splashouille::Style * _style, limits * _limits);

public:
    /** Accessors */
    bool                                    isMap() const                             { return true; }
    splashouille::Image *                   getTileset() const;

    /**
     * Render the object into the surface canvas
     * @param _surface is the surface to fill
     * @param _offset is the parent offset
     * @return true
     */
    bool render(SDL_Surface * _surface UNUSED, SDL_Rect * _offset UNUSED = 0) { return true; }

    /**
    * Update the object regarding the timestamp and add the area to update if any
    * @param _timestamp is the current timestamp
    * @return number of pixels involved in the modification
    */
    bool update(int _timestamp);

    /**
     * Log the map to the standard output
     * @param _rank is the log rank
     */
    void log(int _rank = 0) const;

    /**
     * Crowd event
     */
    bool outCrowd();
    bool inCrowd(splashouilleImpl::Crowd * _crowd) { crowd = _crowd; return true; }
    bool onObject(splashouille::Object * _object, int _user);

    friend class Library;
};

}


#endif

 
 
 
