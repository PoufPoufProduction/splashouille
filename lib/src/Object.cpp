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
#include <splashouilleImpl/Object.hpp>
#include <splashouilleImpl/Fashion.hpp>
#include <splashouilleImpl/Style.hpp>
#include <splashouilleImpl/Image.hpp>

#include <libconfig.h++>
#include <SDL.h>

#include <iostream>
#include <iomanip>


using namespace splashouilleImpl;

int Object::garbageNumber = 0;

Object::Object(const std::string & _id):
    surface(0), id(_id), tag("default"), initialTimestamp(0), zIndex(0), state(0), nbUpdates(0), mouseOver(false),
    listener(0), associatedData(0)
{
    source      = new SDL_Rect();
    position    = new SDL_Rect();
    updateArea  = new SDL_Rect();
    fashion     = new Fashion();
    fashionId   = "default";
    fashions.insert(std::pair<std::string, Fashion*>(fashionId, fashion));

    source->x = source->y = source->w = source->h = 0;
    clearPosition();

    garbageNumber++;
}

Object::~Object()
{
    delete source;
    delete position;
    delete updateArea;

    for (FashionMap::iterator vIt=fashions.begin(); vIt!=fashions.end(); vIt++) { delete vIt->second; }

    garbageNumber--;
}

/**
 * Import an object from configuration file
 * @param _setting is the configuration setting
 * @return true if succeed
 */
bool Object::import(libconfig::Setting & _setting)
{
    // HANDLE THE FASHION(S)
    if (_setting.exists(FASHIONS))
    {
        Fashion * fashionSav = fashion;
        fashions.clear();
        try {
            for (int i=_setting[FASHIONS].getLength()-1; i>=0; i--)
            {
                libconfig::Setting & setting = _setting[FASHIONS][i];
                fashion = new Fashion();
                fashion->import(setting[FASHION]);

                setting.lookupValue(FASHION_ID, fashionId);
                fashions.insert(std::pair<std::string, Fashion*>(fashionId, fashion));
            }
        }
        catch(libconfig::SettingTypeException e) { }

        // Copy the old current style into the new fashion
        fashion->getStyle()->copy(fashionSav->getStyle());
        delete fashionSav;
    }
    else
    if (_setting.exists(FASHION))               { fashion->import(_setting[FASHION]); }

    // HANDLE THE STYLE
    if (_setting.exists(STYLE))                 { fashion->getStyle()->import(_setting[STYLE]); }

    // HANDLE THE SPECIAL VALUE
    if (_setting.exists(MOUSE))
    {
        std::string tmp;
        _setting.lookupValue(MOUSE, tmp);
        Engine::setMouse(this);
        if (!tmp.compare(MOUSE_TOPLEFT))        { Engine::mouseOffset[0] = 0;  Engine::mouseOffset[1] = 0; } else
        if (!tmp.compare(MOUSE_TOP))            { Engine::mouseOffset[0] = 5;  Engine::mouseOffset[1] = 0; } else
        if (!tmp.compare(MOUSE_TOPRIGHT))       { Engine::mouseOffset[0] = 10; Engine::mouseOffset[1] = 0; } else
        if (!tmp.compare(MOUSE_LEFT))           { Engine::mouseOffset[0] = 0;  Engine::mouseOffset[1] = 5; } else
        if (!tmp.compare(MOUSE_CENTER))         { Engine::mouseOffset[0] = 5;  Engine::mouseOffset[1] = 5; } else
        if (!tmp.compare(MOUSE_RIGHT))          { Engine::mouseOffset[0] = 10; Engine::mouseOffset[1] = 5; } else
        if (!tmp.compare(MOUSE_BOTTOMLEFT))     { Engine::mouseOffset[0] = 0;  Engine::mouseOffset[1] = 10;} else
        if (!tmp.compare(MOUSE_BOTTOM))         { Engine::mouseOffset[0] = 5;  Engine::mouseOffset[1] = 10;} else
        if (!tmp.compare(MOUSE_BOTTOMRIGHT))    { Engine::mouseOffset[0] = 10; Engine::mouseOffset[1] = 10;}
    }

    // HANDLE THE ZINDEX
    if (_setting.exists(DEFINITION_ZINDEX))     { _setting.lookupValue(DEFINITION_ZINDEX, zIndex); }

    // HANDLE THE TAG
    if (_setting.exists(DEFINITION_TAG))        { _setting.lookupValue(DEFINITION_TAG, tag); }

    // HANDLE THE CALLBACK VALUE
    if (_setting.exists(DEFINITION_STATE))      { _setting.lookupValue(DEFINITION_STATE, state); }

    // IMAGE SPECIAL TILE INDEX
    if (isImage() && _setting.exists(DEFINITION_TILE))
    {
        int index = 0;
        if (_setting[DEFINITION_TILE].getType() == libconfig::Setting::TypeString)
        {
            std::string strTmp;
            _setting.lookupValue(DEFINITION_TILE, strTmp);
            index = strTmp[0];
        }
        else
        {
            _setting.lookupValue(DEFINITION_TILE, index);
        }
        index = index%TileSetSizeMax;
        setTileIndex(index);
    }

    return true;
}

/**
 * Change the current fashion
 * @param _fashionId is the fashion Id as String
 * @param _force is true than change fashion even if it is the same
 * @return true if the fashion is found
 */
bool Object::changeFashion(const std::string & _fashionId, bool _force)
{
    bool ret = false;

    for (FashionMap::iterator vIt=fashions.begin(); vIt!=fashions.end(); vIt++)
    {
        if (!vIt->first.compare(_fashionId) && (_force || vIt->second!=fashion))
        {
            ret = true;
            initialTimestamp = -1;
            splashouille::Style * style = fashion->getStyle();
            fashionId=_fashionId;
            fashion = vIt->second;
            fashion->clear(style);
        }
    }


    if (Engine::debug && ret)
    {
        std::cout<<std::setw(STD_LABEL)<<std::left<<"Object::changeFashion"<<" (id: "<<getId()<<
                 ") (fashion: "<<fashionId<<")"<<std::endl;
    }

    return ret;
}

/**
 * Clone the fashion of another object
 * @param _object is the fashion source
 */
void Object::cloneFashion(Object * _object)
{
    for (FashionMap::iterator vIt=fashions.begin(); vIt!=fashions.end(); vIt++) { delete vIt->second; }
    fashions.clear(); fashion = 0;

    for (FashionMap::iterator vIt=_object->fashions.begin(); vIt!=_object->fashions.end(); vIt++)
    {
        Fashion * fashionTmp = new Fashion();
        fashionTmp->clone(vIt->second);
        fashions.insert(std::pair<std::string, Fashion*>(vIt->first, fashionTmp));
        if (vIt->second == _object->fashion) { fashion = fashionTmp; fashionId = vIt->first; }
    }
}

/**
 * Get the fashion
 * @return the fashion
 */
splashouille::Fashion * Object::getFashion()    { return fashion; }

/**
 * Get the style
 * @return the style
 */
splashouille::Style * Object::getStyle()        { return fashion->getStyle(); }

/**
 * Get the position of the object
 * @param x,y,w,h are the coordinates of the object
 * @return true if any
 */
bool Object::getPosition(int & _x, int & _y, int & _w, int & _h)
{
    _x = position->x;
    _y = position->y;
    _w = position->w;
    _h = position->h;

    return true;
}


/**
 * Return true if the value begin with 'mouse'
 * @param _value is the value to test
 * @return true if _value begin with mouse
 */
bool Object::beginWithMouse(const std::string & _value)
{
    return (_value.size()>=5 && _value[0]=='m' && _value[1]=='o' && _value[2]=='u' && _value[3]=='s' && _value[4]=='e');
}

/**
 * Handle the mouseEvent
 * @param _timestampInMilliSeconds is the current timestamp
 * @param _x is the mouse position on x-axis
 * @param _y is the mouse position on y-axis
 * @param _checkOver false when object is over
 * @param _state is the button action (if any)
 * @return true for forwarding the event to the below objects
 */
bool Object::mouseEvent(int _timestampInMilliSeconds, int _x, int _y, bool _checkOver, int _state)
{
    bool ret = true;

    if (_x>=position->x && _x<=position->x+position->w && _y>=position->y && _y<=position->y+position->h && _checkOver)
    {
        if (getStyle()->getDisplay())
        {
            ret = false;

            if (beginWithMouse(getFashionId()))
            {
                // CHANGE THE FASHION IF NECESSARY
                std::string newFashionId = _state?"mouseclick":"mouseover";
                if (newFashionId.compare(fashionId))
                {
                    bool release = (!fashionId.compare("mouseclick"));
                    changeFashion(newFashionId);
                    if (listener)
                    {
                        if (release || !newFashionId.compare("mouseclick"))
                            { ret = listener->onMouseClick(_timestampInMilliSeconds, this, _x, _y, _state, release); }
                        else
                            { ret = listener->onMouseOver(_timestampInMilliSeconds, this, _x, _y); }
                    }
                }

                // THE TRANSPARENCY FORWARDS THE EVENT
                ret |= (getStyle()->getOpacity()!=255);
            }
            mouseOver = true;
        }
    }
    else
    if (mouseOver || !_checkOver )
    {
        mouseOver = false;
        if (beginWithMouse(getFashionId())) { changeFashion("mouseout"); }
        if (listener) { listener->onMouseOut(_timestampInMilliSeconds, this); }
    }
    return ret;
}


/**
 * clear the position
 */
void Object::clearPosition()
{
    position->x = position->y = position->w = position->h = 0;
}


/**
 * Update the object regarding the timestamp and add the area to update if any
 * @param _timestamp is the current timestamp
 * @return true if has changed
 */
bool Object::update( int _timestamp)
{
    // Handle the initial timestamp
    if (initialTimestamp<0) { initialTimestamp=_timestamp; }

    int localTimestamp = _timestamp - initialTimestamp;
    if (localTimestamp<0) { localTimestamp=0; }

    // Get the current style
    int ret = 0;
    splashouille::Style * style = fashion->getStyle(localTimestamp);

    if ((ret = style->hasChangedSinceLastTime()))
    {
        splashouille::Engine::copy(updateArea, position);

        position->x = style->getLeft() + style->getRelativeLeft();
        position->y = style->getTop() + style->getRelativeTop();
        position->w = style->getWidth();
        position->h = style->getHeight();

        float p[2];
        style->getPosition(p[0], p[1]);
        source->x = p[0];
        source->y = p[1];
        source->w = position->w;
        source->h = position->h;

        splashouille::Engine::add(updateArea, position);

    }

    nbUpdates++;
    return (ret);
}


