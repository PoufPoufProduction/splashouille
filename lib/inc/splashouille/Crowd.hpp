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

#ifndef SPLASHOUILLE_CROWD_HPP_
#define SPLASHOUILLE_CROWD_HPP_

#include <splashouille/Defines.hpp>
#include <splashouille/Object.hpp>
#include <string>

class SDL_Surface;

namespace splashouille
{
class Animation;

/**
 * THE ANIMATION CROWD
 * Inserting an object into the crowd makes it active, that means it will be render in the animation.
 */
class Crowd
{
public:

    /**
     * THE CROWD LISTENER
     * Some crowd event callbacks
     */
    class Listener
    {
        public:
        /**
         * Callback on a object
         * @param _object is an object of the crowd
         * @return false for ending the current process
         */
        virtual bool onObject(splashouille::Object * _object UNUSED, int _user UNUSED ) { return true; }
    };

    /**
     * Insert an active object into the crowd
     * @param _timestamp is the insertion timestamp
     * @param _object is the object
     * @return true
     */
    virtual bool insertObject(int _timestamp, splashouille::Object * _object) = 0;

    /**
     * Get an object from its id
     * @param _id is the object to get
     * @return the object if found (0 else)
     */
    virtual splashouille::Object * getObject(const std::string & _id) = 0;

    /**
     * Remove an object from the crowd (DO NOT DELETE THE INSTANCE)
     * @param _id is the object to delete as key
     * @return the instance pointer (0 if deleted failed)
     */
    virtual splashouille::Object * dropObject(const std::string & _id) = 0;

    /**
     * Remove an object from the crowd (DO NOT DELETE THE INSTANCE)
     * @param _obj is the object to delete as pointer
     * @return the instance pointer (0 if deleted failed)
     */
    virtual splashouille::Object * dropObject(splashouille::Object * _obj) = 0;

    /**
     * Set the zIndex of an object
     * @param _obj is the object to change
     * @param _zIndex is the new zIndex
     * @return true if succeed
     */
    virtual bool setZIndex(splashouille::Object * _obj, int _zIndex) = 0;

    /**
     * Set the zIndex of an object
     * @param _id is the object to change
     * @param _zIndex is the new zIndex
     * @return true if succeed
    */
    virtual bool setZIndex(const std::string & _id, int _zIndex) = 0;

    /**
     * Get the crowd size
     * @return the crowd size as int
     */
    virtual int getSize() = 0;

    /**
     * Get the parent animation
     * @return the parent animation
     */
    virtual splashouille::Animation * getAnimation() = 0;

    /**
     * Log the crowd to the standard output
     * @param _rank is the log rank
     */
    virtual void log(int _rank = 0) const = 0;

    /**
     * Parse the crowd
     * @param _listener is the callback listener
     * @param _tag is the requested tag object (all objects if empty)
     * @param _ascendant is true for an z-index ascendant browsing (from farest to closest)
     * @param _user is a user parameter forwarded to the callback
     */
    virtual void forEach(Listener * _listener, const std::string & _tag = "", bool _ascendant = true, int _user = 0 ) const = 0;

    /**
     * Clear the crowd: remove objects from the crowd
     * @param _tag is the requested tag object (all objects if empty)
     */
    virtual void clear(const std::string & _tag = "") = 0;

};

}

#endif
