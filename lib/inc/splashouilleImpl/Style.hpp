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

#ifndef SPLASHOUILLEIMPL_STYLE_HPP_
#define SPLASHOUILLEIMPL_STYLE_HPP_

#include <splashouille/Style.hpp>

namespace splashouilleImpl
{

const static int    percent = 96;   // 16-dividable hundred for percentage size computation

/**
 * The style regroups every properties of the object
 * Depending on the object type, every properties are not mandatory
 */
class Style : public splashouille::Style
{
private:
    long        bitmap;                             // What properties are currently used
    int         changed;                            // Has the style been changed
    float       left;                               // The x-axis left value in pixels
    float       top;                                // The y-axis top value in pixels
    float       relativeLeft;                       // The additional x-axis left value in pixels
    float       relativeTop;                        // The additional y-axis top value in pixels
    float       width;                              // The width value in pixels
    float       height;                             // The height value in pixels
    int         backgroundColor[3];                 // The RGB background color
    float       position[2];                        // The position offset
    bool        display;                            // The display state
    int         opacity;                            // The opacity;

    /**
     * Mix two values regarding their ratio
     */
    float       mix(float _value1, float _value2, float _ratio) const { return _value2*_ratio + _value1*(1-_ratio); }

    /**
     * true if the integer part of the float are different
     */
    int         d(float _value1, float _value2) const { return (static_cast<int>(_value1)!=static_cast<int>(_value2)); }

public:
    Style();
    Style(libconfig::Setting & _setting);
    Style(splashouille::Style * _style);
    ~Style();

    /**
     * Import a style from the configuration setting
     * @param _setting is the configuration setting
     */
    void        import(libconfig::Setting & _setting);

    /**
     * Set the style properties with the default values
     */
    void        reset();

    /**
     * Force the hasChanged test
     */
    void        touch();

    /**
     * Set the style properties by copying another style
     * @param _style is the style to copy
     */ 
    void        copy(const splashouille::Style * _style);

    /**
     * Set the style properties by addying another style
     * @param _style is the style to add
     */
    void        add(const splashouille::Style * _style);

    /**
     * Set the style properties by mixing another style regarding a ratio
     * @param _style is the style to mix
     * @param _ratio is the ratio of the new style (0: no modification, 1: style=_style)
     */
    void        mix(const splashouille::Style * _style, float _ratio);

    /**
     * Compare a style (only common properties)
     * @param _style is the style to compare
     * @return the bitmap difference
     */
    int         compare(const splashouille::Style * _style) const;

    /**
     * Log a style to the standard output
     * @param _rank is the log rank
     */
    void        log(int _rank = 0) const;

    /** specific accessors */
    int         hasChanged() const              { return changed; }
    bool        getDisplay() const              { return display; }
    long        getBitmap() const               { return bitmap; }
    float       getLeft() const                 { return left; }
    float       getTop() const                  { return top; }
    float       getRelativeLeft() const         { return relativeLeft; }
    float       getRelativeTop() const          { return relativeTop; }
    float       getWidth() const                { return width; }
    float       getHeight() const               { return height; }
    void        getBackgroundColor(int & _red, int & _green, int & _blue) const;
    void        getPosition(float & _x, float & _y) const;
    int         getOpacity() const              { return opacity; }

    int         hasChangedSinceLastTime()       { int ret=changed; changed=0; return ret; }
    void        setLeft(float _left)            { changed|=(d(left,_left)<<__left); left = _left; bitmap|=(1<<__left); }
    void        setTop(float _top)              { changed|=(d(top,_top)<<__top); top = _top; bitmap|=(1<<__top); }
    void        setWidth(float _width)          { changed|=(d(width,_width)<<__width); width = _width; bitmap|=(1<<__width);}
    void        setHeight(float _height)        { changed|=(d(height,_height)<<__height); height = _height; bitmap|=(1<<__height);}
    void        setRelativeLeft(float _left)
                    { changed|=(d(relativeLeft,_left)<<__relativeleft);relativeLeft=_left;bitmap|=(1<<__relativeleft); }
    void        setRelativeTop(float _top)
                    { changed|=(d(relativeTop,_top)<<__relativetop);relativeTop=_top; bitmap|=(1<<__relativetop); }
    void        setBackgroundColor(int _red, int _green, int _blue)
                    { _red      = (_red<0)?0:((_red>255)?255:_red);
                      _green    = (_green<0)?0:((_green>255)?255:_green);
                      _blue     = (_blue<0)?0:((_blue>255)?255:_blue);
                      changed|=(((backgroundColor[0]!=_red)|(backgroundColor[1]!=_green)|(backgroundColor[2]!=_blue))<<__backgroundColor);
                      backgroundColor[0] = _red; backgroundColor[1] = _green; backgroundColor[2] = _blue;
                      bitmap|=(1<<__backgroundColor); }
    void        setPosition(float _x, float _y)
                    { changed|= ((d(position[0],_x)|d(position[1],_y))<<__position);
                      position[0]=_x; position[1]=_y;
                      bitmap|=(1<<__position); }
    void        setOpacity(int _opacity)
                    { _opacity = (_opacity<0)?0:((_opacity>255)?255:_opacity);
                      changed|=(d(opacity,_opacity)<<__opacity); opacity = _opacity; bitmap|=(1<<__opacity); }
    void        setDisplay(bool _display)
                    { changed|=((_display!=display)<<__display); display=_display; bitmap|=(1<<__display); }

};

}


#endif

 
 
 
 
