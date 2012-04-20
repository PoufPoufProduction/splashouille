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

#ifndef SPLASHOUILLEIMPL_TIMELINE_HPP_
#define SPLASHOUILLEIMPL_TIMELINE_HPP_

#include <splashouille/Timeline.hpp>
#include <vector>

class SDL_Rect;

namespace splashouille
{
class Crowd;
class Library;
}

namespace splashouilleImpl
{
class Event;
class Animation;

class Timeline : public splashouille::Timeline
{
public:
    static int                                  garbageNumber;  // The number of allocated timeline
private:
    std::vector<splashouilleImpl::Event *>      events;         // The timeline event
    unsigned int                                eventIndex;     // The event index (events are sorted !!!)
    Animation *                                 animation;      // The parent animation

    inline int min(int nX, int nY) { return nX > nY ? nY : nX; }

    Timeline(Animation * _animation);
    ~Timeline();

public:

    /**
     * Import and prepare the timeline
     * @param _library is the objects library (for creating new objects)
     * @param _setting is the configuration setting
     * @return true if everything is fine
     */
    bool import(splashouille::Library * _library, libconfig::Setting & _setting);

    /**
     * run the events
     * @param _timestamp is the current timestamp
     */
    void update(int _timestamp);

    /** Accessors */
    splashouille::Animation *    getAnimation();

    /**
     * Log the timeline to the standard output
     * @param _rank is the log rank
     */
    void log(int _rank = 0) const;

    /**
     * Move the timestamp
     * @param _currentTimeStamp is the current timestamp
     * @param _updateInitialTimestamp
     */
    void move(int _currentTimeStamp, int _timeStampInMilliSeconds);

    /**
     * Clear the timeline
     */
    void clear();

    friend class Animation;

};

}

#endif

