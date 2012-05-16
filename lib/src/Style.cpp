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
#include <splashouilleImpl/Style.hpp>

#include <libconfig.h++>

#include <iostream>
#include <iomanip>

#ifdef __GNUC__
#define UNUSED __attribute__((unused))
#else
#define UNUSED
#endif

using namespace splashouilleImpl;

Style::Style()                                  { reset(); }
Style::Style(libconfig::Setting & _setting)     { reset(); import(_setting); }
Style::Style(splashouille::Style * _style)      { copy(_style); }
Style::~Style() { }

/**
 * Import a style from the configuration setting
 * @param _setting is the configuration setting
 */
void Style::import(libconfig::Setting & _setting)
{
    int value;

    if (_setting.lookupValue(STYLE_LEFT, value))                        { left = value; bitmap|=(1<<__left); }
    if (_setting.lookupValue(STYLE_TOP, value))                         { top = value;  bitmap|=(1<<__top); }
    if (_setting.lookupValue(STYLE_RELATIVE_LEFT, value))               { relativeLeft = value; bitmap|=(1<<__relativeleft); }
    if (_setting.lookupValue(STYLE_RELATIVE_TOP, value))                { relativeTop = value;  bitmap|=(1<<__relativetop); }
    if (_setting.lookupValue(STYLE_WIDTH, value))                       { width= value; bitmap|=(1<<__width); }
    if (_setting.lookupValue(STYLE_HEIGHT, value))                      { height=value; bitmap|=(1<<__height); }
    if (_setting.lookupValue(STYLE_OPACITY, opacity))                   { bitmap|=(1<<__opacity); }
    if (_setting.lookupValue(STYLE_USER, user))                         { bitmap|=(1<<__user); }
    if (_setting.lookupValue(STYLE_DISPLAY, value))                     { display=(value); bitmap|=(1<<__display); }

    if (_setting.exists(STYLE_BACKGROUNDCOLOR))
    {
        for (int i=0; i<3; i++) backgroundColor[i] = (int)_setting[STYLE_BACKGROUNDCOLOR][i];
        bitmap|=(1<<__backgroundColor);
    }

    if (_setting.exists(STYLE_POSITION))
    {
        for (int i=0; i<2; i++) position[i] = (int)_setting[STYLE_POSITION][i];
        bitmap|=(1<<__positionX) | (1<<__positionY);
    }
    else
    {
        if (_setting.lookupValue(STYLE_POSITION_X, value))                { position[0] = value; bitmap|=(1<<__positionX); }
        if (_setting.lookupValue(STYLE_POSITION_Y, value))                { position[1] = value; bitmap|=(1<<__positionY); }
    }

}

/**
 * Set the style properties with the default values
 */
void Style::reset()
{
    bitmap                      = 0;
    top = left = width = height = 0;
    relativeLeft = relativeTop  = 0;
    display                     = true;
    opacity                     = 255;
    user                        = 0;
    backgroundColor[0] = backgroundColor[1] = backgroundColor[2] = 0;
    position[0] = position[1]   = 0;
    touch();
}

/**
 * Force the hasChanged test
 */
void Style::touch()
{
    changed                     = (1<<__last)-1;
}

/**
 * Set the style properties by copying another style
 * @param _style is the style to copy
 */
void Style::copy(const splashouille::Style * _style)
{
    if (_style!=this)
    {
        reset();

        left                = _style->getLeft();
        top                 = _style->getTop();
        relativeLeft        = _style->getRelativeLeft();
        relativeTop         = _style->getRelativeTop();
        width               = _style->getWidth();
        height              = _style->getHeight();
        display             = _style->getDisplay();
        opacity             = _style->getOpacity();
        user                = _style->getUser();

        _style->getBackgroundColor(backgroundColor[0], backgroundColor[1], backgroundColor[2]);
        _style->getPosition(position[0], position[1]);

        bitmap  = _style->getBitmap();
        changed = _style->getBitmap();
    }
}

/**
 * Set the style properties by addying another style
 * @param _style is the style to add
 */
void Style::add(const splashouille::Style * _style)
{
    if (_style!=this)
    {
        if (_style->getBitmap()&(1<<__left))        { setLeft           ( _style->getLeft()); }
        if (_style->getBitmap()&(1<<__top))         { setTop            ( _style->getTop()); }
        if (_style->getBitmap()&(1<<__relativeleft)){ setRelativeLeft   ( _style->getRelativeLeft()); }
        if (_style->getBitmap()&(1<<__relativetop)) { setRelativeTop    ( _style->getRelativeTop()); }
        if (_style->getBitmap()&(1<<__width))       { setWidth          ( _style->getWidth()); }
        if (_style->getBitmap()&(1<<__height))      { setHeight         ( _style->getHeight()); }
        if (_style->getBitmap()&(1<<__display))     { setDisplay        ( _style->getDisplay()); }
        if (_style->getBitmap()&(1<<__opacity))     { setOpacity        ( _style->getOpacity()); }
        if (_style->getBitmap()&(1<<__user))        { setUser           ( _style->getUser()); }

        if (_style->getBitmap()&(1<<__backgroundColor))
        {
            int red, green, blue;
            _style->getBackgroundColor(red, green, blue);
            setBackgroundColor(red, green, blue);
        }

        float p[2]; _style->getPosition(p[0], p[1]);
        if (_style->getBitmap()&(1<<__positionX))   {   setPositionX    (p[0]); }
        if (_style->getBitmap()&(1<<__positionY))   {   setPositionY    (p[1]); }

        bitmap  |= _style->getBitmap();
    }
}

/**
 * Set the style properties by mixing another style regarding a ratio
 * @param _style is the style to mix
 * @param _ratio is the ratio of the new style (0: no modification, 1: style=_style)
 */
void Style::mix(const splashouille::Style * _style, float _ratio)
{
    if (_style!=this)
    {
        int both = bitmap&_style->getBitmap();
        if ( both&(1<<__left) )         setLeft         ( mix(left, _style->getLeft(), _ratio) );
        if ( both&(1<<__top) )          setTop          ( mix(top, _style->getTop(), _ratio) );
        if ( both&(1<<__relativeleft) ) setRelativeLeft ( mix(relativeLeft, _style->getRelativeLeft(), _ratio) );
        if ( both&(1<<__relativetop) )  setRelativeTop  ( mix(relativeTop, _style->getRelativeTop(), _ratio) );
        if ( both&(1<<__width) )        setWidth        ( mix(width, _style->getWidth(), _ratio) );
        if ( both&(1<<__height) )       setHeight       ( mix(height, _style->getHeight(),_ratio) );
        if ( both&(1<<__opacity) )      setOpacity      ( mix(opacity, _style->getOpacity(), _ratio) );
        if ( both&(1<<__user) )         setUser         ( mix(user, _style->getUser(), _ratio) );

        if ( both&(1<<__backgroundColor) )
        {
            int rgb[3];
            _style->getBackgroundColor(rgb[0],rgb[1],rgb[2]);
            for (int i=0; i<3; i++) { rgb[i] = mix(backgroundColor[i], rgb[i], _ratio); }
            setBackgroundColor(rgb[0], rgb[1], rgb[2]);
        }

        float p[2]; _style->getPosition(p[0], p[1]);
        if ( both&(1<<__positionX) )    setPositionX    ( mix(position[0], p[0], _ratio) );
        if ( both&(1<<__positionY) )    setPositionY    ( mix(position[1], p[1], _ratio) );

        bitmap  |= _style->getBitmap();
    }
}

/**
 * Compare a style (only common properties)
 * @param _style is the style to compare
 * @return the bitmap difference
 */
int Style::compare(const splashouille::Style * _style) const
{
    int ret = 0;
    int both = bitmap&_style->getBitmap();

    if ( both&(1<<__left) )         ret |= (d(left, _style->getLeft())<<__left);
    if ( both&(1<<__top) )          ret |= (d(top, _style->getTop())<<__top);
    if ( both&(1<<__relativeleft) ) ret |= (d(relativeLeft, _style->getRelativeLeft())<<__relativeleft);
    if ( both&(1<<__relativetop) )  ret |= (d(relativeTop, _style->getRelativeTop())<<__relativetop);
    if ( both&(1<<__width) )        ret |= (d(width, _style->getWidth())<<__width);
    if ( both&(1<<__height) )       ret |= (d(height, _style->getHeight())<<__height);
    if ( both&(1<<__opacity) )      ret |= (d(opacity, _style->getOpacity())<<__opacity);
    if ( both&(1<<__user) )         ret |= (d(user, _style->getUser())<<__user);
    if ( both&(1<<__display) )      ret |= (d(display, _style->getDisplay())<<__display);

    return ret;
}


/**
 * Log a style to the standard output
 * @param _rank is the log rank
 */
void Style::log(int _rank) const
{
    std::string offset; offset.append(4*_rank,' ');
    std::cout<<offset<<"+ Style "<<" (bitmap: "<<bitmap<<")"<<std::endl;
    if (bitmap&(1<<__left))         { std::cout<<offset<<".left     : "<<left<<std::endl; }
    if (bitmap&(1<<__top))          { std::cout<<offset<<".top      : "<<top<<std::endl; }
    if (bitmap&(1<<__relativeleft)) { std::cout<<offset<<".relleft  : "<<relativeLeft<<std::endl; }
    if (bitmap&(1<<__relativetop))  { std::cout<<offset<<".reltop   : "<<relativeTop<<std::endl; }
    if (bitmap&(1<<__width))        { std::cout<<offset<<".width    : "<<width<<std::endl; }
    if (bitmap&(1<<__height))       { std::cout<<offset<<".heigh    : "<<height<<std::endl; }
    if (bitmap&(1<<__display))      { std::cout<<offset<<".display  : "<<display<<std::endl; }
    if (bitmap&(1<<__opacity))      { std::cout<<offset<<".opacity  : "<<opacity<<std::endl; }
    if (bitmap&(1<<__user))         { std::cout<<offset<<".user     : "<<user<<std::endl; }
    if (bitmap&(1<<__backgroundColor))
    {
        std::cout<<offset<<".bg       : ("<<backgroundColor[0]<<","<<backgroundColor[1]<<","<<backgroundColor[2]<<")"<<std::endl;
    }
    if ((bitmap&(1<<__positionX)) || (bitmap&(1<<__positionY)))
    {
        std::cout<<offset<<".position : ("<<position[0]<<","<<position[1]<<")"<<std::endl;
    }
}

void Style::getBackgroundColor(int & _red, int & _green, int & _blue) const
{ _red = backgroundColor[0]; _green = backgroundColor[1]; _blue = backgroundColor[2]; }

void Style::getPosition(float & _x, float & _y) const
{ _x = position[0]; _y = position[1]; }

