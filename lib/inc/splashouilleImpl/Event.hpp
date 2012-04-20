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

#ifndef SPLASHOUILLEIMPL_EVENT_HPP_
#define SPLASHOUILLEIMPL_EVENT_HPP_

#include <splashouille/Event.hpp>
#include <string>
#include <vector>

class SDL_Rect;

namespace splashouille
{
class Object;
class Library;
class Crowd;
}

namespace splashouilleImpl
{

class Timeline;

/**
 * The event implementation class.
 * Constructor and destructor are only reachable by the splashouilleImpl::Timeline class (friend class)
 */
class Event : public splashouille::Event
{
public:
    static int                  garbageNumber;                  // Number of allocated events
    static int                  eventCount;
private:
    int                         timeStampInMilliSeconds;        // Trigger of the event
    Type                        type;                           // Type of the event
    splashouille::Object *      object;                         // An optional object pointer
    std::vector<std::string>    objectIds;                      // An optional object ids vector
    std::string                 id;                             // The event id
    Timeline *                  timeline;                       // The parent timeline
    unsigned int                value;                          // A generic integer value
    std::string                 valueStr;                       // A generic string value
    bool                        option;                         // A generic boolean value

    Event(Timeline * _timeline);
    ~Event();

    /**
     * Run an event
     * @param _timestamp is the current timestamp
     * @return false if the timeline has been changed
     */
    bool run(int _timestamp);

    /**
     * Import and prepare the timeline
     * @param _library is the objects library
     * @param _setting is the configuration setting
     * @return true if everything is fine
     */
    bool import(splashouille::Library * _library, libconfig::Setting & _setting);

    /**
     * Clear the event
     */
    void clear();

public:

    /**
     * Get the type of the event
     * @param _type as string
     */
    Type getType() { return type; }

    /** Accessors */
    int                             getTimeStampInMilliSeconds()    { return timeStampInMilliSeconds; }
    splashouille::Timeline *        getTimeline();
    const std::string &             getId()                         { return id; }

    /**
     * Log the event to the standard output
     * @param _rank is the log rank
     */
    void log(int _rank = 0) const;

    friend class Timeline;

};

}

#endif
 
