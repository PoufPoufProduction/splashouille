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

#ifndef SPLASHOUILLE_ANIMATION_HPP_
#define SPLASHOUILLE_ANIMATION_HPP_

#include <splashouille/Object.hpp>

namespace splashouille
{
class Timeline;
class Crowd;
class Library;

/**
 * THE ANIMATION OBJECT\n
 * The animation handles objects and their evolution thanks to its timeline and crowd
 * There are 2 kinds of animation : static and dynamic. Static animations are much more
 * like a group of objects handled by the parent animation. Dynamic animations handle
 * their own objects thanks to a bitmap but their perfs are not so good than the static
 * ones.
 */
class Animation : virtual public splashouille::Object
{
public:
    /** The background usage for not static animation (or engine) : none: no background,
     * color : use the style->backgroundcolor, copy: use the original surface (only for engine)
     */
    enum backgroundEnum { none = 0, color, copy };

    /** The Animation type. group : animation is just a group of objects which are dealt by
     *  the parent animation, dynamic : animation is handling its own surface and the child objects
     *  final : animation is dynamic but const is not refresh after the first update
     */
    enum animationType { group = 0, dynamic, final };

public:
    /**
     * Each animation is defined by one or more timelines. Only one is active.
     * @return the active animation timeline
     */
    virtual splashouille::Timeline * getTimeline() = 0;

    /**
     * The crowd is the library where every active objects are stored.
     * @return the animation crowd
     */
    virtual splashouille::Crowd * getCrowd() = 0;

    /**
     * A root library pointer is stored in each animation when they are instanced.
     * @return the root library
     */
    virtual splashouille::Library * getLibrary() = 0;

    /** @return the animation type */
    virtual splashouille::Animation::animationType getAnimationType() const = 0;

    /**
     * Change the current timeline
     * @param _timelineId is the timeline Id as String
     * @param _updateInitialTimeStamp for reseting the new timeline timestamp to 0
     * @return true if the timeline is found
     */
    virtual bool changeTimeline(const std::string & _timelineId, bool _updateInitialTimeStamp = true) = 0;

    /**
     * Import an animation from configuration file
     * @param _setting is the configuration setting
     * @return true if succeed
     */
    virtual bool import(libconfig::Setting & _setting) = 0;

};

}


#endif
