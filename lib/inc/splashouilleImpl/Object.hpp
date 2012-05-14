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

#ifndef SPLASHOUILLEIMPL_OBJECT_HPP_
#define SPLASHOUILLEIMPL_OBJECT_HPP_

#include <splashouille/Object.hpp>
#include <string>
#include <map>

namespace splashouilleImpl
{
class Fashion;
class Crowd;

/**
 * The object implementation class.
 */
class Object : virtual public splashouille::Object
{
private:
    /**
     * Return true if the value begin with 'mouse'
     * @param _value is the value to test
     * @return true if _value begin with mouse
     */
    bool beginWithMouse(const std::string & _value);

protected:
    inline int                                  min(int nX, int nY) { return nX > nY ? nY : nX; }
    typedef std::map<std::string, Fashion*>     FashionMap;

    Object(const std::string & _id);
    ~Object();

public:
    static int                              garbageNumber;          // Number of allocated objects

    FashionMap                              fashions;               // The fashions of the object
    Fashion *                               fashion;                // The current fashion
    SDL_Surface *                           surface;                // The render surface
    SDL_Rect *                              source;                 // The source image
    SDL_Rect *                              position;               // The position
    SDL_Rect *                              updateArea;             // The update area
    std::string                             id;                     // The object id
    std::string                             type;                   // The object type
    std::string                             fashionId;              // The current fashion id
    std::string                             tag;                    // The object tag
    int                                     initialTimestamp;       // The initial timestamp of the object
    int                                     zIndex;                 // The object z-index;
    int                                     state;                  // The callback state parameter
    bool                                    mouseOver;              // Is the mouse over the object

    splashouille::Object::Listener *        listener;               // The object listener
    splashouille::Object::AssociatedData *  associatedData;         // The associated data

public:

    /**
     * Handle the mouseEvent
     * @param _timestampInMilliSeconds is the current timestamp
     * @param _x is the mouse position on x-axis
     * @param _y is the mouse position on y-axis
     * @param _checkOver false when object is over
     * @param _state is the button action (if any)
     * @return true for forwarding the event to the below objects
     */
    bool mouseEvent(int _timestampInMilliSeconds, int _x, int _y, bool _checkOver, int _state = 0);

    /**
     * get the object id
     * @return the object id as string
     */
    const std::string & getId() const { return id; }

    /**
     * get the object type
     * @return the type as string
     */
    const std::string & getType() const { return type; }

    /**
     * get the object tag
     * @return the tag as string
     */
    const std::string & getTag() const { return tag; }

    /**
     * set the object tag (to call before inserting in the crowd)
     */
    void setTag(const std::string & _tag) { tag = _tag; }

    /**
     * get the z-index
     * @return the z-index as integer
     */
    int getZIndex() const { return zIndex; }

    /**
     * set the z-index (to call before inserting in the crowd)
     */
    void setZIndex(int _zIndex) { zIndex = _zIndex; }

    /**
     * get the state free integer value
     * @return the state free integer value
     */
    int getState() const { return state; }

    /**
     * set the state free integer value
     * @param _state
     */
    void setState(int _state) { state = _state; }

    /**
     * set the object listener
     * @param _listener is the new object listener
     */
    void setListener(splashouille::Object::Listener * _listener) { listener = _listener; }

    /**
     * Get the object listener
     * @return the object listener
     */
    splashouille::Object::Listener * getListener() const { return listener; }

    /**
     * Get the fashion
     * @return the fashion
     */
    splashouille::Fashion *  getFashion();

    /**
     * Get the current fashion id
     * @return the fashion id as string
     */
    const std::string & getFashionId() { return fashionId; }

    /**
     * Get the style
     * @return the style
     */
    splashouille::Style * getStyle();

    /**
     * Change the current fashion
     * @param _fashionId is the fashion Id as String
     * @return true if the fashion is found
     */
    bool changeFashion(const std::string & _fashionId);

    /**
     * Clone the fashion of another object
     * @param _object is the fashion source
     */
    void cloneFashion(Object * _object);

    /**
     * Import an object from configuration file
     * @param _setting is the configuration setting
     * @return true if succeed
     */
    bool import(libconfig::Setting & _setting);

    /**
     * Get the timestamp of the crowd insertion
     * @return the timestamp in milliseconds
     */
    int getInitialTimestamp() const { return initialTimestamp; }

    /**
     * Set the timestamp of the crowd insertion
     * @param _timestampInMilliSeconds is the initial timestamp
     */
    void setInitialTimestamp(int _timestampInMilliSeconds) { initialTimestamp = _timestampInMilliSeconds; }

    /**
     * Update the object regarding the timestamp and add the area to update if any
     * @param _timestamp is the current timestamp
     * @return true if the object has changed
     */
    virtual bool update(int _timestamp);

    /**
     * Render the object into the surface canvas
     * @param _surface is the surface to fill
     * @param _offset is the parent offset
     * @return true
     */
    virtual bool render(SDL_Surface * _surface UNUSED, SDL_Rect * _offset UNUSED = 0) { return true; }

    /**
     * Get the position of the object
     * @param x,y,w,h are the coordinates of the object
     * @return true if any
     */
    virtual bool getPosition(int & _x, int & _y, int & _w, int & _h);

    /**
     * Get the position of the object
     * @return the position
     */
    virtual const SDL_Rect * getPosition() { return position; }

    /**
     * Get the updateArea of the object
     * @return the position
     */
    virtual const SDL_Rect * getUpdateRect() { return updateArea; }

    /**
     * set the associated data
     * @param _data is the associated data
     */
    void setAssociatedData(splashouille::Object::AssociatedData * _data) { associatedData = _data; }

    /**
     * @return the associated data (0 if none)
     */
    splashouille::Object::AssociatedData * getAssociatedData() { return associatedData; }

    /**
     * clear the position
     */
    void clearPosition();

    /**
     * crowd events special callbacks
     */
    virtual bool inCrowd(splashouilleImpl::Crowd * _crowd UNUSED) { return true; }
    virtual bool outCrowd() { return true; }

    /**
     * Types accessors
     */
    virtual bool isSolid() const        { return false; }
    virtual bool isImage() const        { return false; }
    virtual bool isAnimation() const    { return false; }
    virtual bool isSound() const        { return false; }
    virtual bool isMap() const          { return false; }
    virtual bool isEngine() const       { return false; }

    /**
     * Specific methods
     */
    virtual void setTileIndex(int _tileIndex UNUSED) {}

};

}

#endif
 
 
