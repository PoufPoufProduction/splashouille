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
#include <splashouilleImpl/Fashion.hpp>
#include <splashouilleImpl/Style.hpp>

#include <libconfig.h++>

#include <iostream>
#include <iomanip>

using namespace splashouilleImpl;

/**
 * Transition constructor
 * f(t) = a*t3+b*t2+c*t with f(0) = 0, f(1) = 1 and f'(0) = speedIn, f'(1) = speedOut
 * @param _timestampIn is the beginning timestamp of the animation
 * @param _timestampOut is the ending timestamp of the animation
 * @param _speedIn is the speed at the begin of the animation
 * @param _speedOut is the speed at the end of the animation
 * @param _period is the period of the transition
 */
Fashion::Transition::Transition(int _timestampIn, int _timestampOut, double _speedIn, double _speedOut, int _period) :
    timestampIn(_timestampIn), timestampOut(_timestampOut), speedIn(_speedIn), speedOut(_speedOut),
    period(_period), nbTimes(0), done(false)
{
    style = new Style();

    a = speedIn + speedOut - 2.0;
    b = 3.0 - speedIn*2 - speedOut;
    c = speedIn;
}

/**
 * Another constructor
 */
Fashion::Transition::Transition(Transition * _t):
    timestampIn(_t->timestampIn), timestampOut(_t->timestampOut), speedIn(_t->speedIn), speedOut(_t->speedOut), period(_t->period), nbTimes(_t->nbTimes), done(false)
{
    style = new Style(_t->style);

    a = speedIn + speedOut - 2.0;
    b = 3.0 - speedIn*2 - speedOut;
    c = speedIn;
}

/**
 * Transition destructor
 * Delete the local copy of style
 */
Fashion::Transition::~Transition()
{
    delete style;
}

/**
 * Compute the ratio of the next style regarding the timestamp
 * @param _timestamp is the current timestamp
 */
float Fashion::Transition::ratio(int _timestamp)
{
    float ret;

    _timestamp-=nbTimes*period;

    if (_timestamp < timestampIn)   { ret = 0; }    else
    if (_timestamp > timestampOut)  { ret = 1; }    else
    {
        float t = (float) (_timestamp-timestampIn)/(timestampOut-timestampIn);
        ret = a*t*t*t + b*t*t + c*t;
    }

    return ret;
}

Fashion::Fashion(splashouille::Fashion * _fashion): timestamp(-1)
{
    style = new Style(_fashion->getStyle());
    current = new Style(_fashion->getStyle());
    last = new Style();
    clone(_fashion);
}

Fashion::Fashion(): timestamp(-1)
{
    style = new Style();
    current = new Style();
    last = new Style();
}

/**
 * Fashion destructor: delete the local copy of the initial style
 */
Fashion::~Fashion()
{
    delete style;
    delete current;
    delete last;

    for (std::list<Transition*>::iterator it=transitions.begin(); it!=transitions.end(); it++) { delete (*it); }
}

/**
 * Remove the old transitions by updating the initial style
 * @param _timestamp is the current timestamp
 * @param _active is set to true if a transaction is currently active
 */
void Fashion::removeOldTransitions(int _timestamp, bool & _active)
{
    _active = false;
    for (std::list<Transition*>::iterator it=transitions.begin(); it!=transitions.end(); it++)
    {
        Transition * transition = *it;

        if (transition->timestampOut+transition->nbTimes*transition->period<=_timestamp && !transition->done)
        {
            style->add(transition->style);
            if (transition->period) { transition->nbTimes++; } else { transition->done = true; }
        }
        else
        if (transition->timestampIn+transition->nbTimes*transition->period<_timestamp &&
            transition->timestampOut+transition->nbTimes*transition->period>_timestamp )
        {
            _active = true;
        }
    }
}

/**
 * Import a fashion
 * @param _fashion is the parent fashion
 */
void Fashion::clone(splashouille::Fashion * _fashion)
{
    style->copy(_fashion->getStyle());
    current->copy(_fashion->getStyle());

    splashouilleImpl::Fashion * fashion = static_cast<splashouilleImpl::Fashion *>(_fashion);
    for (std::list<Transition*>::const_iterator it=fashion->getTransitions().begin(); it!=fashion->getTransitions().end(); it++)
    {
        addTransition(new Transition(*it));
    }
}

/**
 * Import a fashion
 * @param _setting is the fashion configuration (as list of transitions)
 */
void Fashion::import(libconfig::Setting & _setting)
{
    try { for (int i=0; i<_setting.getLength(); i++) { importTransition(_setting[i]); } }
    catch(libconfig::SettingTypeException e) { }
}

/**
 * Import a transition from the configuration file
 * @param _setting is the transition setting
 * @return the new allocated style of the object at the end of the animation
 */
splashouille::Style * Fashion::importTransition(libconfig::Setting & _setting)
{
    splashouille::Style * ret = 0;

    if (_setting.exists(FASHION_TIMESTAMP))
    {
        // Get the transition timestamp
        int time[2];
        if (_setting[FASHION_TIMESTAMP].getType() == libconfig::Setting::TypeArray)
        {
            time[0] = _setting[FASHION_TIMESTAMP][0];
            time[1] = _setting[FASHION_TIMESTAMP][1];
        }
        else
        {
            _setting.lookupValue(FASHION_TIMESTAMP, time[0]);
            time[1] = time[0];
        }

        // Get the speed value (libconfig is really tricky with the value type)
        float   speed[2] = { 1, 1 };
        int     speedTmp;
        if (_setting.exists(FASHION_SPEEDIN))
        {
            if (_setting[FASHION_SPEEDIN].getType() == libconfig::Setting::TypeFloat)
                    { _setting.lookupValue(FASHION_SPEEDIN, speed[0]); }
            else    { _setting.lookupValue(FASHION_SPEEDIN, speedTmp); speed[0] = speedTmp; }
        }

        if (_setting.exists(FASHION_SPEEDOUT))
        {
            if (_setting[FASHION_SPEEDOUT].getType() == libconfig::Setting::TypeFloat)
                    { _setting.lookupValue(FASHION_SPEEDOUT, speed[1]); }
            else    { _setting.lookupValue(FASHION_SPEEDOUT, speedTmp); speed[1] = speedTmp; }
        }

        // Get the period (if any)
        int period = 0;
        _setting.lookupValue(FASHION_PERIOD, period);

        if ((ret = addTransition(time[0], time[1], speed[0], speed[1], period))) { ret->import(_setting); }

    }

    return ret;
}

/**
 * Add an animation to the current style
 * @param _transition is the transition to add
 * @return the style of the object at the end of the animation
 */
splashouille::Style * Fashion::addTransition(Transition * _transition)
{
    transitions.push_back(_transition);
    return _transition->style;
}

/**
 * Add an animation to the current style
 * @param _timestampIn is the beginning timestamp of the animation
 * @param _timestampOut is the ending timestamp of the animation
 * @param _speedIn is the speed at the begin of the animation
 * @param _speedOut is the speed at the end of the animation
 * @param _period is the period of the transition
 * @return the style of the object at the end of the animation
 */
splashouille::Style * Fashion::addTransition(int _timestampIn, int _timestampOut, double _speedIn, double _speedOut, int _period)
{
    return addTransition(new Transition(_timestampIn, _timestampOut, _speedIn, _speedOut, _period));
}

/**
 * Get the current style computed from the initial style and the transitions list
 * @param _timestamp is the current timestamp
 * @return the current style but const (because automaticaly generated)
 */
splashouille::Style * Fashion::getStyle(int _timestamp)
{
    if (timestamp!=_timestamp || !timestamp)
    {
        bool active;
        removeOldTransitions(_timestamp, active);

        if (style->hasChangedSinceLastTime() || active)
        {
            last->copy(style);
            if (active)
            {
                for (std::list<Transition*>::iterator it=transitions.begin(); it!=transitions.end(); it++)
                {
                    Transition * transition = *it;
                    if ( (transition->timestampIn+transition->nbTimes*transition->period<_timestamp) &&
                         (transition->timestampOut+transition->nbTimes*transition->period>_timestamp) &&
                         (!transition->done) )
                    {
                        last->mix(transition->style, transition->ratio(_timestamp));
                    }
                }
            }
            current->add(last);
        }
        timestamp = _timestamp;
    }

    return current;
}

/**
 * Clear the transition regarding the new timestamp
 * @param _style is the new initial style
 */
void Fashion::clear(splashouille::Style * _style)
{
    for (std::list<Transition*>::const_iterator it=transitions.begin(); it!=transitions.end(); it++)
    {
        (*it)->done = false;
        (*it)->nbTimes = 0;
    }
    if (_style)
    {
        style->copy(_style);
        current->copy(_style);
    }
    else
    {
        style->touch();
        current->reset();
    }
}


/**
 * Log a fashion to the standard output
 * @param _rank is the log rank
 */
void Fashion::log(int _rank) const
{
    std::string offset; offset.append(4*_rank,' ');
    std::cout<<offset<<"+ Fashion"<<" (size: "<<transitions.size()<<")"<<std::endl;
    style->log(_rank);

    for (std::list<Transition*>::const_iterator it=transitions.begin(); it!=transitions.end(); it++)
    {
        Transition * transition = *it;
        std::cout<<offset<<"+ Transition ["<<transition->timestampIn<<","<<transition->timestampOut<<"] ["<<transition->speedIn<<","<<
                    transition->speedOut<<"]"<<(transition->done?" [Done]":"")<<std::endl;
        transition->style->log(_rank+1);
    }
}



