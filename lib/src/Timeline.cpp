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

#include <splashouille/Object.hpp>
#include <splashouilleImpl/Engine.hpp>
#include <splashouilleImpl/Animation.hpp>
#include <splashouilleImpl/Library.hpp>
#include <splashouilleImpl/Engine.hpp>
#include <splashouilleImpl/Event.hpp>
#include <splashouilleImpl/Timeline.hpp>
#include <libconfig.h++>
#include <iostream>
#include <iomanip>
#include <SDL.h>

using namespace splashouilleImpl;

int Timeline::garbageNumber = 0;

Timeline::Timeline(Animation * _animation):eventIndex(0), animation(_animation) { garbageNumber++; }
Timeline::~Timeline()
{
    for (unsigned int i=0; i<events.size(); i++) { delete events[i]; }
    garbageNumber--;
}

/** Accessors */
splashouille::Animation * Timeline::getAnimation() { return animation; }

/**
 * Import and prepare the timeline
 * @param _library is the objects library (for creating new objects)
 * @param _setting is the configuration setting
 * @return true if everything is fine
 */
bool Timeline::import(splashouille::Library * _library, libconfig::Setting & _setting)
{
    bool            ret = true;

    try {
        for (int i=0; i<_setting.getLength(); i++) {
            Event * event = new Event(this);
            event->import(_library, _setting[i]);
            events.push_back(event);
        }
    }
    catch(libconfig::SettingTypeException e) { ret = false; }

    if (Engine::debug)
    {
        std::cout<<std::setw(STD_LABEL)<<std::left<<"Timeline::import"
             <<" (size: "<<events.size()<<") (return: "<<(ret?"OK":"KO")<<")"<<std::endl;
    }

    return ret;
}

/**
 * run the events
 * @param _timestamp is the current timestamp
 * @return number of pixels updated
 */
void Timeline::update(int _timestamp)
{
    bool            cont = true;

    int localTimestamp = _timestamp - animation->getInitialTimestamp();
    if (localTimestamp<0) { localTimestamp=0; }

    while ( eventIndex<events.size() &&
            cont &&
            events[eventIndex]->getTimeStampInMilliSeconds()<=localTimestamp)
    {
        cont = events[eventIndex]->run(_timestamp);
        eventIndex++;
    }
}

/**
 * Clear the timeline
 */
void Timeline::clear()
{
    eventIndex = 0;
    for (unsigned int i=0; i<events.size(); i++) { events[i]->clear(); }
}

/**
 * Move the timestamp
 * @param _currentTimeStamp is the currentTimestamp
 * @param _timeStampInMilliSeconds is the new timestamp of the animation
 */
void Timeline::move(int _currentTimeStamp, int _timeStampInMilliSeconds)
{
    // Set the eventIndex to its new position regarding the timestamp
    eventIndex = 0;
    while (eventIndex<events.size() && events[eventIndex]->getTimeStampInMilliSeconds()<_timeStampInMilliSeconds) { eventIndex++; }

    // Reset the fashion of all the event objects if timestamp is going back (useless otherwise)
    if (_currentTimeStamp>_timeStampInMilliSeconds) { for (unsigned int i=0; i<events.size(); i++) { events[i]->clear(); } }

    // Update the animation initial timestamp
   animation->setInitialTimestamp(animation->getInitialTimestamp() + _currentTimeStamp - _timeStampInMilliSeconds);
}

/**
 * Log the timeline to the standard output
 * @param _rank is the log rank
 */
void Timeline::log(int _rank) const
{
    std::string offset; offset.append(4*_rank,' ');
    std::cout<<offset<<"+ Timeline (size: "<<events.size()<<") (id: "<<eventIndex<<")"<<std::endl;
    for (unsigned int i=0; i<events.size(); i++) { events[i]->log(_rank+1); }
}

