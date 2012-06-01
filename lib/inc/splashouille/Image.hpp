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

#ifndef SPLASHOUILLE_IMAGE_HPP_
#define SPLASHOUILLE_IMAGE_HPP_

#include <splashouille/Object.hpp>

namespace splashouille
{

/**
 * THE IMAGE OBJECT\n
 * Load a bitmap from file and display it regarding its style
 * Thanks to the position style attribute, image handled sprite animation
 * Also a tileset can be defined in order to map position with tile index
 * Last point, original bitmaps are all stored in a static garbage in order to
 * minimize the memory used. 2 Images with the same filename use the same
 * bitmap (but the style may be different of course)
 */
class Image : virtual public splashouille::Object
{
public:
    /**
     * THE DISPLAY MODE
@verbatim
crop            crop the bitmap for fitting the image size
scale           scale the bitmap for fitting the image size
@endverbatim
     */
    enum Display { crop, scale };

    /** @return the image filename as string */
    virtual const std::string & getFilename() const = 0;

    /** @return the real width of the image */
    virtual int         getWidth() const = 0;

    /** @return the real height of the image */
    virtual int         getHeight() const = 0;

    /** @return true if an alpha color is defined (and set the _r,_g,_b parameters in the same time */
    virtual bool        getAlphaColor(int & _r, int & _g, int & _b) const = 0;

    /** @return the image display mode */
    virtual Display     getDisplay() const = 0;

    /** @return the tileindex in case of the image reference is associated with a tileset */
    virtual int         getTileIndex() const = 0;

    /**
     * create the image surface from its filename
     * @param _filename is the image filename
     * @return true is the image is new (ie the filename has never been used) 
     */
    virtual bool        setFilename(const std::string & _filename) = 0;

    /** Set the alpha color of the bitmap */
    virtual void        setAlphaColor(int _r=255, int _g=0, int _b=255) = 0;

    /** Set the image size */
    virtual void        setSize(int _width, int _height) = 0;

    /** Set the image display mode (not used for the moment) */
    virtual void        setDisplay(Display _display) = 0;

    /** Set the tileindex in case of the image reference is associated with a tileset */
    virtual void        setTileIndex(int _tileIndex) = 0;

};

}


#endif

 
 
 
