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

#ifndef SPLASHOUILLEIMPL_CROWD_HPP_
#define SPLASHOUILLEIMPL_CROWD_HPP_

#include <splashouille/Crowd.hpp>
#include <map>
#include <list>

class SDL_Rect;

namespace splashouilleImpl
{
class Animation;
class Object;

/**
 * The crowd implementation
 */
class Crowd : public splashouille::Crowd
{
public:
    static int                                          garbageNumber;      // The number of allocated crowd
private:
    std::map<std::string, Object *>                     library;            // All the objects in the crowd
    std::map<std::string, std::list<Object*>*>          crowd;              // The crowd objects (map by tags, list by zindex)
    Animation *                                         animation;          // The parent animation

    typedef std::list<Object*>::iterator                objectIterator;
    typedef std::pair<objectIterator, objectIterator>   objectParser;

    /**
     * Return a pointer to the tag-related list
     * @param _tag is the tagged list
     * @return the list
     */
    std::list<Object*>* getTagObjects(const std::string & _tag);

    /**
     * Remove an object from the crowd (do not delete the instance)
     * @param _it is the map iterator
     * @return the instance pointer (0 if deleted failed)
     */
    splashouille::Object * dropObject(std::map<std::string, Object *>::iterator _it);

    /** Constructor and destructors */
    Crowd(Animation * _animation);
    ~Crowd();

public:

    /**
     * Insert an active object into the crowd
     * @param _timestamp is the insertion timestamp
     * @param _object is the object
     * @return true
     */
    bool insertObject(int _timestamp, splashouille::Object * _object);

    /**
     * Get an object from its id
     * @param _id is the object to get
     * @return the object if found (0 else)
     */
    splashouille::Object * getObject(const std::string & _id);

    /**
     * Remove an object from the crowd (do not delete the instance)
     * @param _id is the object to delete
     * @return the instance pointer (0 if deleted failed)
     */
    splashouille::Object * dropObject(const std::string & _id);

    /**
     * Remove an object from the crowd (do not delete the instance)
     * @param _obj is the object to delete as pointer
     * @return the instance pointer (0 if deleted failed)
     */
    splashouille::Object * dropObject(splashouille::Object * _obj);

    /**
     * Set the zIndex of an object
     * @param _obj is the object to change
     * @param _zIndex is the new zIndex
     * @return true if succeed
     */
    bool setZIndex(splashouille::Object * _obj, int _zIndex);

    /**
     * Set the zIndex of an object
     * @param _id is the object to change
     * @param _zIndex is the new zIndex
     * @return true if succeed
    */
    bool setZIndex(const std::string & _id, int _zIndex);

    /**
     * Get the crowd size
     * @return the crowd size as int
     */
    int getSize();

    /**
     * Get the parent animation
     * @return the parent animation
     */
    splashouille::Animation * getAnimation() { return animation; }

    /**
     * Log the crowd to the standard output
     * @param _rank is the log rank
     */
    void log(int _rank = 0) const;

    /**
     * Handle the mouseEvent
     * @param _x is the mouse position on x-axis
     * @param _y is the mouse position on y-axis
     * @param _button is the button pressed or released (if any)
     * @param _state is the button action (if any)
     */
    bool mouseEvent(int _timestampInMilliSeconds, int _x, int _y, int _button = 0, int _state = 0);

    /**
     * Parse the crowd
     * @param _listener is the callback listener
     */
    void forEach(Listener * _listener, const std::string & _tag = "") const;

    /**
     * Update the current crowd
     * @param _timestamp is the current timestamp
     * @return
     */
    void update(int _timestamp);

    /**
     * Render the current crowd
     * @param _surface is the surface to render
     * @param _offset is the parent offset
     */
    void render(SDL_Surface * _surface, SDL_Rect * _offset = 0);

    /**
     * Clear the crowd
     */
    void clear();

    /**
     * Forward the callback
     * @return true
     */
    bool outCrowd();

    friend class Animation;
};

}

#endif

