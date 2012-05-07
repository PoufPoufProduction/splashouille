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

#ifndef SPLASHOUILLE_STYLE_HPP_
#define SPLASHOUILLE_STYLE_HPP_

#include <string>

namespace libconfig
{
class Setting;
}

namespace splashouille
{

class Style
{
public:
    enum Property { __display=0,        __left,             __top,              __width,            __height,
                    __backgroundColor,  __positionX,        __positionY,        __opacity,          __relativeleft,
                    __relativetop,      __last
                  };


    /**
     * Import a style from the configuration setting
     * @param _setting is the configuration setting
     */
    virtual void import(libconfig::Setting & _setting) = 0;

    /**
     * Set the style properties with the default values
     */
    virtual void reset() = 0;

    /**
     * Force the hasChanged test
     */
    virtual void touch() = 0;

    /**
     * Set the style properties by copying another style
     * @param _style is the style to copy
     */
    virtual void copy(const splashouille::Style * _style) = 0;

    /**
     * Set the style properties by addying another style
     * @param _style is the style to add
     */
    virtual void add(const splashouille::Style * _style) = 0;

    /**
     * Set the style properties by mixing another style regarding a ratio
     * @param _style is the style to mix
     * @param _ratio is the ratio of the new style (0: no modification, 1: style=_style)
     */
    virtual void mix(const splashouille::Style * _style, float _ratio) = 0;

    /**
     * Compare a style (only common properties)
     * @param _style is the style to compare
     * @return the bitmap difference
     */
    virtual int compare(const splashouille::Style * _style) const = 0;

    /**
     * Log a style to the standard output
     * @param _rank is the log rank
     */
    virtual void log(int _rank = 0) const = 0;

    /** specific accessors */
    virtual int         hasChanged() const = 0;
    virtual long        getBitmap() const = 0;
    virtual float       getLeft() const = 0;
    virtual float       getTop() const = 0;
    virtual float       getRelativeLeft() const = 0;
    virtual float       getRelativeTop() const = 0;
    virtual float       getWidth() const = 0;
    virtual float       getHeight() const = 0;
    virtual void        getBackgroundColor(int & _red, int & _green, int & _blue) const = 0;
    virtual void        getPosition(float & _x, float & _y) const = 0;
    virtual int         getOpacity() const = 0;
    virtual bool        getDisplay() const = 0;

    virtual int         hasChangedSinceLastTime() = 0;
    virtual void        setLeft(float _left) = 0;
    virtual void        setTop(float _top) = 0;
    virtual void        setRelativeLeft(float _left) = 0;
    virtual void        setRelativeTop(float _top) = 0;
    virtual void        setWidth(float _width) = 0;
    virtual void        setHeight(float _height) = 0;
    virtual void        setBackgroundColor(int _red, int _green, int _blue) = 0;
    virtual void        setPositionX(float _x) = 0;
    virtual void        setPositionY(float _y) = 0;
    virtual void        setPosition(float _x, float _y) = 0;
    virtual void        setOpacity(int _opacity) = 0;
    virtual void        setDisplay(bool _display) = 0;
};

}


#endif

