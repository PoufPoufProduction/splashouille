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

#ifndef SPLASHOUILLEIMPL_ANIMATION_HPP_
#define SPLASHOUILLEIMPL_ANIMATION_HPP_

#include <splashouille/Defines.hpp>
#include <splashouille/Animation.hpp>
#include <splashouilleImpl/Object.hpp>

#include <SDL.h>
#include <map>
#include <string>

namespace splashouille
{
class Style;
class Library;
}

namespace splashouilleImpl
{
class Timeline;
class Crowd;
class Library;

const static int    nbUpdateRectsMax = 256;

/**
 * The animation class implementation
 * Actually, there are two kind of animations: static and not static
 * The static one doesn't create any surface, it is just a group of objects.
 * The not static handles its own surface
 */
class Animation : virtual public splashouille::Animation, virtual public splashouilleImpl::Object
{
protected:
    typedef std::map<std::string,Timeline*> TimelineMap;

    int                                     numberOfPixels;                 // The number of pixels involved in update
    int                                     nbUpdateRects;                  // The update rects number
    SDL_Rect                                updateRects[nbUpdateRectsMax];  // The update rects
    TimelineMap                             timelines;                      // The animation timelines
    Timeline *                              timeline;                       // Save the current timeline pointer for perf
    Crowd *                                 crowd;                          // The animation active objects
    splashouille::Library *                 library;                        // A copy of the library pointer
    bool                                    donotmove;                      // The animation is static
    Animation *                             parent;                         // If the animation is static : the parent animation
    bool                                    hasChanged;                     // Has the animation style changed ?


    Animation(const std::string & _id, libconfig::Setting & _setting, splashouille::Library * _library);
    Animation(const std::string & _id, Animation * _animation, splashouille::Library * _library);
    Animation(const std::string & _id, splashouille::Library * _library);
    ~Animation();

    /**
     * initialize the timeline(s)
     */
    void initTimelines();

public:
    /** Accessors */
    splashouille::Timeline *                getTimeline();
    splashouille::Crowd *                   getCrowd();
    splashouille::Library *                 getLibrary()            { return library; }
    bool                                    isStatic() const        { return donotmove; }
    void                                    isStatic(bool _value)   { donotmove = _value; }
    bool                                    isAnimation() const     { return true; }
    void                                    setParent(Animation*_a) { if (this!=_a) { parent = _a; } }

    void                                    clear()                 { numberOfPixels  = 0; nbUpdateRects   = 0; }

    /**
     * Handle the mouseEvent
     * @param _timestampInMilliSeconds is the current timestamp
     * @param _x is the mouse position on x-axis
     * @param _y is the mouse position on y-axis
     * @param _state is the button action (if any)
     */
    bool mouseEvent(int _timestampInMilliSeconds, int _x, int _y, int _state = 0);

    /**
     * Update the object regarding the timestamp and add the area to update if any
     * @param _timestamp is the current timestamp
     * @return true if modified
     */
    bool update(int _timestamp);

    /**
     * Add an update rect into the animation
     * @param _updateRect is the updateRect to insert
     */
    void addUpdateRect(const SDL_Rect * _updateRect);

    /**
     * Import an animation from configuration file
     * @param _setting is the configuration setting
     * @param _library is the library
     * @return true if succeed
     */
    bool import(libconfig::Setting & _setting);

    /**
     * Render the object into the surface canvas
     * @param _surface is the surface to fill
     * @param _offset is the parent offset
     * @return true
     */
    bool render(SDL_Surface * _surface, SDL_Rect * _offset = 0);

    /**
     * Log the animation to the standard output
     * @param _rank is the log rank
     */
    virtual void log(int _rank = 0) const;

    /**
     * Change the current timeline
     * @param _timelineId is the timeline Id as String
     * @return true if the timeline is found
     */
    bool changeTimeline(const std::string & _timelineId, bool _updateInitialTimeStamp = true);

    /**
     * Forward the callback
     * @return true
     */
    bool outCrowd();

    /**
     * Erase the updated parts (if any)
     */
    virtual void fillWithBackground();

    friend class Library;
};
}

#endif

