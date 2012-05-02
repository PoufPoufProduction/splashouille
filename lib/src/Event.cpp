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

#include <splashouille/Defines.hpp>
#include <splashouilleImpl/Engine.hpp>
#include <splashouilleImpl/Library.hpp>
#include <splashouilleImpl/Crowd.hpp>
#include <splashouilleImpl/Fashion.hpp>
#include <splashouilleImpl/Style.hpp>
#include <splashouilleImpl/Animation.hpp>
#include <splashouilleImpl/Timeline.hpp>
#include <splashouilleImpl/Event.hpp>
#include <libconfig.h++>
#include <iostream>
#include <iomanip>

#include <SDL.h>

using namespace splashouilleImpl;

int Event::garbageNumber    = 0;
int Event::eventCount       = 0;

Event::Event(Timeline * _timeline):
    timeStampInMilliSeconds(0), type(none), object(0), timeline(_timeline), value(0), valueStr(""), option(false)
{
    garbageNumber++;
}

Event::~Event()
{
    garbageNumber--;
}

/** Accessors */
splashouille::Timeline * Event::getTimeline() { return timeline; }

/**
 * Update the event regarding the current timestamp
 */
void Event::clear()
{
    if (object) { dynamic_cast<Fashion*>(object->getFashion())->clear(); }
}

/**
 * Import and prepare the timeline
 * @param _library is the objects library
 * @param _setting is the configuration setting
 * @return true if everything is fine
 */
bool Event::import(splashouille::Library * _library, libconfig::Setting & _setting)
{
    bool            ret = true;     // The return value
    std::string     typeStr;        // The type event as string
    std::string     objectId;

    _setting.lookupValue(_setting.exists(EVENT_TIMESTAMP)?EVENT_TIMESTAMP:EVENT_TIMESTAMP_SHORT, timeStampInMilliSeconds);
    _setting.lookupValue(EVENT_TYPE, typeStr);
    if (_setting.exists(EVENT_ID))  { _setting.lookupValue(EVENT_ID, id); }
    else                            { char m[128]; snprintf(m, 128, "event%05d", eventCount++); id.assign(m); }

    if (!typeStr.compare(EVENT_TYPE_CREATE) )
    {
        type = create;
        object = _library->createObject(_setting);
        objectId = object->getId();
        objectIds.push_back(objectId);
    }
    else if (!typeStr.compare(EVENT_TYPE_COPY) )
    {
        type = copy;

        if (_setting.exists(EVENT_OBJECTID)) { _setting.lookupValue(EVENT_OBJECTID, objectId); }
        else                                 { char m[128]; snprintf(m, 128, "__event%05d", eventCount-1); objectId.assign(m); }
        objectIds.push_back(objectId);

        std::string parentStr;
        _setting.lookupValue(EVENT_PARENT, parentStr);
        object = _library->copyObject(parentStr, objectId);

        if (object)     { dynamic_cast<Object*>(object)->import(_setting); }

        ret = (object!=0);
    }
    else if (!typeStr.compare(EVENT_TYPE_CLOSE) )
    {
        if (_setting.exists(EVENT_OBJECTID))
        {
            type = close;
            libconfig::Setting & setting = _setting[EVENT_OBJECTID];
            if (setting.getType()== libconfig::Setting::TypeString)
            {
                _setting.lookupValue(EVENT_OBJECTID, objectId);
                objectIds.push_back(objectId);
            }
            else
            if (setting.getType()== libconfig::Setting::TypeList)
            {
                for (int i=0; i<setting.getLength(); i++)
                {
                    objectId.assign((const char*)setting[i]);
                    objectIds.push_back(objectId);
                }
            }
        }
    }
    else if (!typeStr.compare(EVENT_TYPE_GOTO) )
    {
        type = moveto;
        _setting.lookupValue(EVENT_VALUE, value);
    }
    else if (!typeStr.compare(EVENT_TYPE_TIMELINE) )
    {
        type = changetimeline;
        option = true;
        _setting.lookupValue(EVENT_VALUE, valueStr);

        if (_setting.exists(EVENT_OBJECTID))
        {
            _setting.lookupValue(EVENT_OBJECTID, objectId);
            objectIds.push_back(objectId);
        }

       _setting.lookupValue(EVENT_OPTION, option);
    }
    else if (!typeStr.compare(EVENT_TYPE_CLEAR) )
    {
        type = clearcrowd;
        if (_setting.exists(EVENT_VALUE)) {  _setting.lookupValue(EVENT_VALUE, valueStr); }
    }
    else if (!typeStr.compare(EVENT_TYPE_FASHION) )
    {
        type = changefashion;
        _setting.lookupValue(EVENT_VALUE, valueStr);
        if (_setting.exists(EVENT_OBJECTID))
        {
            libconfig::Setting & setting = _setting[EVENT_OBJECTID];
            if (setting.getType()== libconfig::Setting::TypeString)
            {
                _setting.lookupValue(EVENT_OBJECTID, objectId);
                objectIds.push_back(objectId);
            }
            else
            if (setting.getType()== libconfig::Setting::TypeList)
            {
                for (int i=0; i<setting.getLength(); i++)
                {
                    objectId.assign((const char*)setting[i]);
                    objectIds.push_back(objectId);
                }
            }
        }
    }
    else if (!typeStr.compare(EVENT_TYPE_STATE) )
    {
        type = state;
        _setting.lookupValue(EVENT_VALUE, value);

        if (_setting.exists(EVENT_OBJECTID))
        {
            _setting.lookupValue(EVENT_OBJECTID, objectId);
            objectIds.push_back(objectId);
        }
    }
    else
    {
        ret = false;
    }

    return ret;
}

/**
 * Run an event
 * @param _timestamp is the current timestamp
 * @return false if the timeline has been changed
 */
bool Event::run(int _timestamp)
{
    bool                        ret = true;
    splashouille::Animation *   animation = timeline?timeline->getAnimation():0;
    splashouille::Crowd *       crowd = animation?animation->getCrowd():0;

    if (timeline && animation && crowd)
    {

        // Handle the event
        switch(type)
        {
        case create:
        case copy:              crowd->insertObject(_timestamp, object); break;
        case close:             for (unsigned int i=0;i<objectIds.size();i++) { crowd->dropObject(objectIds[i]); } break;
        case moveto:            timeline->move(_timestamp, value); ret = false; break;
        case changetimeline:    if (objectIds.size())
                                {
                                    splashouille::Animation * a = animation->getLibrary()->getAnimationById(objectIds[0]);
                                    if (a) { a->changeTimeline(valueStr, option); }
                                }
                                else
                                {
                                    animation->changeTimeline(valueStr, option);
                                    ret = false;
                                }
                                break;
        case clearcrowd:        crowd->clear(valueStr); break;
        case changefashion:     if (objectIds.size())
                                {
                                    for (unsigned int i=0;i<objectIds.size();i++)
                                    {
                                        splashouille::Object * o = animation->getLibrary()->getObjectById(objectIds[i]);
                                        if (o) { o->changeFashion(valueStr); }
                                    }
                                }
                                else
                                {
                                    animation->changeFashion(valueStr);
                                }
                                break;
        case state:             if (objectIds.size())
                                {
                                    for (unsigned int i=0;i<objectIds.size();i++)
                                    {
                                        splashouille::Object * o = animation->getLibrary()->getObjectById(objectIds[i]);
                                        if (o) { o->setState(value); }
                                    }
                                }
                                else
                                {
                                    animation->setState(value);
                                }
                                break;
        default:
            break;
        }
    }

    if (Engine::debug)
    {
        std::cout<<std::setw(STD_LABEL)<<std::left<<"Event::run"<<" (timestamp: "<<_timestamp<<") (type: "<<type<<")"<<std::endl;
    }

    return ret;
}


/**
 * Log the animation to the standard output
 * @param _rank is the log rank
 */
void Event::log(int _rank) const
{
    std::string offset; offset.append(4*_rank,' ');
    std::cout<<offset<<"+ Event (timestamp:"<<timeStampInMilliSeconds<<") (type:"<<type<<") (id: "<<id<<")";
    if (objectIds.size())
    {
        std::cout<<" (objectIds:";
        for (unsigned int i=0; i<objectIds.size(); i++) std::cout<<" "<<objectIds[i];
        std::cout<<")";
    }
    if (value) { std::cout<<" (value: "<<value<<")"; }
    std::cout<<std::endl;
}

