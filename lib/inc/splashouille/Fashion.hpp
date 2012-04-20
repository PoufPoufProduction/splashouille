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

#ifndef SPLASHOUILLE_FASHION_HPP_
#define SPLASHOUILLE_FASHION_HPP_

#include <string>

namespace libconfig
{
class Setting;
}

namespace splashouille
{

class Style;

/**
 * The class which makes the style update.
 */
class Fashion
{
public:
    /**
     * Copy a fashion
     * @param _fashion is the parent fashion
     */
    void clone(splashouille::Fashion * _fashion);

    /**
     * Import a fashion
     * @param _setting is the fashion configuration (as list of transitions)
     */
    virtual void import(libconfig::Setting & _setting) = 0;

    /**
     * Import a transition from the configuration file
     * @param _setting is the transition setting
     * @return the new allocated style of the object at the end of the animation
     */
    virtual splashouille::Style * importTransition(libconfig::Setting & _setting) = 0;

    /**
     * Add an animation to the current style
     * @param _timestampIn is the beginning timestamp of the animation
     * @param _timestampOut is the ending timestamp of the animation
     * @param _speedIn is the speed at the begin of the animation
     * @param _speedOut is the speed at the end of the animation
     * @param _period is the period of the transition
     * @return the new allocated style of the object at the end of the animation
     */
    virtual splashouille::Style * addTransition(int _timestampIn, int _timestampOut, double _speedIn = 1, double _speedOut = 1, int _period = 0) = 0;

    /**
     * Get the current style
     * @return the current style but const (because automaticaly generated)
     */
    virtual const splashouille::Style * getCurrent() = 0;

    /**
     * Get the initial style
     * @return the initial style
     */
    virtual splashouille::Style * getStyle() = 0;

    /**
     * Log a fashion to the standard output
     * @param _rank is the log rank
     */
    virtual void log(int _rank = 0) const = 0;


};

}


#endif

