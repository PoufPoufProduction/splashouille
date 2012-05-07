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

#ifndef SPLASHOUILLE_EVENT_HPP_
#define SPLASHOUILLE_EVENT_HPP_

namespace libconfig
{
class Setting;
}

namespace splashouille
{
class Timeline;

/**
 * THE EVENT CLASS\n
 * Timeline executes the events regarding their timestamp. Event make modification on
 * animation objects depending on their type.
 */
class Event
{
public:
    /**
     * THE EVENT TYPE
@verbatim
none                not really used
insert              insert a new object into the current animation crowd
copy                copy an object from an older one and insert it into the current animation crowd
pause               pause the current animation
moveto              update the current animation timestamp
changetimeline      change an animation timeline (current animation by default)
clearcrowd          clear the current animation crowd
changefashion       change the fashion of an object
state               change the state variable of an object
@endverbatim
     */
    enum Type       { none = 0, insert, copy, close, pause, moveto, changetimeline, clearcrowd, changefashion, state };
public:
    /** @return the event type */
    virtual Type getType() = 0;

    /** @return the timestamp event in milliseconds  */
    virtual int                             getTimeStampInMilliSeconds() = 0;

    /** @return the event timeline (used for getting back the timeline and then the animation and its crowd)*/
    virtual splashouille::Timeline *        getTimeline() = 0;

    /** @return the id of the event (not really used) */
    virtual const std::string &             getId() = 0;

    /**
     * Log the event to the standard output
     * @param _rank is the log rank
     */
    virtual void log(int _rank = 0) const = 0;

};

}

#endif
