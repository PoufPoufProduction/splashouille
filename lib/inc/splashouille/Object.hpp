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

#ifndef SPLASHOUILLE_OBJECT_HPP_
#define SPLASHOUILLE_OBJECT_HPP_

#include <string>
#include <splashouille/Defines.hpp>

class SDL_Surface;
class SDL_Rect;


namespace libconfig
{
class Setting;
}

namespace splashouille
{
class Fashion;
class Style;
class Crowd;

/**
 * The generic object interface
 */
class Object
{
public:
    /**
     * The class object listener
     */
    class Listener
    {
    public:
        virtual void onShow         (int _timestampInMilliSeconds UNUSED, splashouille::Object * _object UNUSED) {}
        virtual void onMouseOut     (int _timestampInMilliSeconds UNUSED, splashouille::Object * _object UNUSED) {}
        virtual bool onMouseOver    (int _timestampInMilliSeconds UNUSED, splashouille::Object * _object UNUSED) { return true; }
        virtual bool onMouseClick   (int _timestampInMilliSeconds UNUSED, splashouille::Object * _object UNUSED, int _state UNUSED)
                                    { return true; }

        virtual void onHide         (splashouille::Object * _object UNUSED) {}
    };

    /**
     * The class associated data
     */
    class AssociatedData
    {
    public:
    };

public:
    /**
     * get the object id
     * @return the object id as string
     */
    virtual const std::string & getId() const = 0;

    /**
     * get the object type
     * @return the type as string
     */
    virtual const std::string & getType() const = 0;

    /**
     * get the object tag
     * @return the tag as string
     */
    virtual const std::string & getTag() const = 0;

    /**
     * get the z-index
     * @return the z-index as integer
     */
    virtual int getZIndex() const = 0;

    /**
     * Get the fashion
     * @return the fashion
     */
    virtual splashouille::Fashion * getFashion() = 0;

    /**
     * Get the style
     * @return the style
     */
    virtual splashouille::Style * getStyle() = 0;

    /**
     * Change the current fashion
     * @param _fashionId is the fashion Id as String
     * @return true if the fashion is found
     */
    virtual bool changeFashion(const std::string & _fashionId) = 0;

    /**
     * Get the position of the object
     * @param x,y,w,h are the coordinates of the object
     * @return true if any
     */
    virtual bool getPosition(int & _x, int & _y, int & _w, int & _h) = 0;

    /**
     * Get the position of the object
     * @return the position
     */
    virtual const SDL_Rect * getPosition() = 0;

    /**
     * Log the object to the standard output
     * @param _rank is the log rank
     */
    virtual void log(int _rank = 0) const = 0;

    /**
     * Get the timestamp of the crowd insertion
     * @return the timestamp in milliseconds
     */
    virtual int getInitialTimestamp() const = 0;

    /**
     * get the state free integer value
     * @return the user free integer value
     */
    virtual int getState() const = 0;

    /**
     * set the state free integer value
     * @param _state is the new object state
     */
    virtual void setState(int _state) = 0;

    /**
     * set the object listener
     * @param _listener is the new object listener
     */
    virtual void setListener(splashouille::Object::Listener * _listener) = 0;

    /**
     * set the associated data
     * @param _data is the associated data
     */
    virtual void setAssociatedData(splashouille::Object::AssociatedData * _data) = 0;

    /**
     * @return the associated data (0 if none)
     */
    virtual splashouille::Object::AssociatedData * getAssociatedData() = 0;

    /**
     * Types accessors
     */
    virtual bool isSolid() const = 0;
    virtual bool isImage() const = 0;
    virtual bool isAnimation() const = 0;
    virtual bool isSound() const = 0;
    virtual bool isEngine() const = 0;
};

}


#endif

