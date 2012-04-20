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

#ifndef SPLASHOUILLE_TIMELINE_HPP_
#define SPLASHOUILLE_TIMELINE_HPP_

#include <string>

namespace libconfig
{
class Setting;
}

namespace splashouille
{
class Library;
class Animation;

/**
 * The Timeline interface.
 * Timeline purpose is to handle the events during the animation process.
 * @see splashouille::Event class
 */
class Timeline
{
public:
    /**
     * Import and prepare the timeline
     * @param _library is the objects library (for creating new objects)
     * @param _setting is the configuration setting
     * @return true if everything is fine
     */
    virtual bool import(splashouille::Library * _library, libconfig::Setting & _setting) = 0;

    /** Accessors */
    virtual splashouille::Animation *    getAnimation() = 0;

    /**
     * Log the timeline to the standard output
     * @param _rank is the log rank
     */
    virtual void log(int _rank = 0) const = 0;
};

}

#endif
