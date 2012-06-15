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

#include <splashouilleImpl/Engine.hpp>
#include <splashouilleImpl/Image.hpp>
#include <splashouilleImpl/Fashion.hpp>
#include <splashouille/Style.hpp>
#include <libconfig.h++>
#include <iostream>

#include <SDL.h>
#ifdef SDL_IMAGE
    #include <SDL_image.h>
#endif

using namespace splashouilleImpl;

std::map<std::string, Image::Surface*>  Image::surfaces;
std::list<Image::Tileset*>              Image::tilesets;

/**
 * The Tileset destructor
 */
Image::Tileset::~Tileset()
{
    for (int i=0; i<TileSetSizeMax; i++)
    {
        Tile * tile = tiles[i];
        while (tile) { Tile * tileTmp = tile->next; delete tile; tile = tileTmp; };
    }
}

/**
 * The Tile constructor from libconfig::Setting
 * @param _setting is the position tile setting
 */
Image::Tileset::Tile::Tile(libconfig::Setting & _setting)
{
    position[0] = position[1] = size[0] = size[1] = offset[0] = offset[1] = 0; next = 0;
    delayInMilliseconds = 0;

    if (_setting.getType() == libconfig::Setting::TypeArray)
    {
        int length = _setting.getLength();
        if (length>=2)  { position[0] = _setting[0];    position[1] = _setting[1]; }
        if (length>=4)  {     size[0] = _setting[2];        size[1] = _setting[3]; }
        if (length>=6)  {   offset[0] = _setting[4];      offset[1] = _setting[5]; }
    }
}

/**
 * create the image surface from its filename
 * @param _filename is the image filename
 * @return true is the image is new (ie the filename has never been used) 
 */
bool Image::setFilename(const std::string & _filename)
{
    filename    = _filename;
    surface     = 0;
    original    = 0;

    std::map<std::string, Image::Surface*>::iterator it = surfaces.find(filename);
    if (it!=surfaces.end())
    {
        original = it->second->surface;
        it->second->nbUsages++;
    }
    else
    {
    #ifdef SDL_IMAGE
        SDL_Surface*    loadedImage = IMG_Load(filename.c_str());
    #else
        SDL_Surface*    loadedImage = SDL_LoadBMP(filename.c_str());
    #endif
        if (loadedImage)
        {
            original = SDL_DisplayFormat(loadedImage);
            SDL_FreeSurface(loadedImage);
            surfaces.insert(std::pair<std::string, Image::Surface*>(filename, new Surface(original)));
        }
    }
    surface=original;

    return (surface);
}

/**
 * @return the real width of the image
 */
int Image::getWidth() const { return (original)?original->w:0; }

/**
 * @return the real height of the image
 */
int Image::getHeight() const { return (original)?original->h:0; }

/**
 * Update the color used as transparent color
 * @param _r,_g,_b are the alpha color components
 */
void Image::setAlphaColor(int _r, int _g, int _b)
{
    alphaColor[0] = _r; alphaColor[1] = _g; alphaColor[2] = _b; alpha = true;

    if (surface)
    {
        Uint32 colorKey = SDL_MapRGB( surface->format, alphaColor[0], alphaColor[1], alphaColor[2]);
        SDL_SetColorKey(surface, SDL_RLEACCEL | SDL_SRCCOLORKEY, colorKey);
    }
}

/**
 * Set the image width
 * @param _width is the new image width
 * @param _height is the new image height
 */
void Image::setSize(int _width UNUSED, int _height UNUSED)
{
}

/**
 * @return true if an alpha color is defined (and set the _r,_g,_b parameters in the same time
 */
bool Image::getAlphaColor(int & _r, int & _g, int & _b) const
{
    if (alpha) { _r = alphaColor[0]; _g = alphaColor[1]; _b = alphaColor[2]; }
    return alpha;
}

Image::Image(const std::string & _id, libconfig::Setting & _setting):
    splashouilleImpl::Object(_id), display(crop), original(0), tileset(0), tileIndex(-1)
{
    type = TYPE_IMAGE;

    // FASHION AND STYLE IMPORT
    Object::import(_setting);

    // GET THE FILENAME
    if (_setting.exists(DEFINITION_FILENAME))
    {
        if (_setting[DEFINITION_FILENAME].getType() == libconfig::Setting::TypeGroup)
        {
            _setting[DEFINITION_FILENAME].lookupValue(Engine::locale, filename);
        }
        else
        {
            _setting.lookupValue(DEFINITION_FILENAME, filename);
        }
        setFilename(filename);
    }

    // GET THE TILESET IF ANY
    if (_setting.exists(DEFINITION_TILESET) && _setting[DEFINITION_TILESET].getType() == libconfig::Setting::TypeList)
    {
        tileset = new Tileset();
        tilesets.push_back(tileset);

        try {
            for (int i=0; i<_setting[DEFINITION_TILESET].getLength(); i++)
            {
                libconfig::Setting & setting = _setting[DEFINITION_TILESET][i];

                // GET THE TILE INDEX
                int index = 0;
                if (setting.exists(DEFINITION_TILESET_INDEX))
                {
                    if (setting[DEFINITION_TILESET_INDEX].getType() == libconfig::Setting::TypeString)
                    {
                        std::string strTmp;
                        setting.lookupValue(DEFINITION_TILESET_INDEX, strTmp);
                        index = strTmp[0];
                    }
                    else
                    {
                        setting.lookupValue(DEFINITION_TILESET_INDEX, index);
                    }
                    index = index%TileSetSizeMax;
                }

                // CREATE THE TILE
                if (!tileset->tiles[index])
                {
                    if (setting.exists(DEFINITION_TILESET_POSITION))
                    {
                        tileset->tiles[index] = new Tileset::Tile(setting[DEFINITION_TILESET_POSITION]);
                    }
                    else
                    if (setting.exists(DEFINITION_TILESET_ANIM) &&
                        setting[DEFINITION_TILESET_ANIM].getType() == libconfig::Setting::TypeList)
                    {
                        Tileset::Tile * oldTile = 0;
                        for (int i=0; i<setting[DEFINITION_TILESET_ANIM].getLength(); i++)
                        {
                            libconfig::Setting & tileSetting = setting[DEFINITION_TILESET_ANIM][i];
                            if (tileSetting.exists(DEFINITION_TILESET_POSITION))
                            {
                                Tileset::Tile * newTile = new Tileset::Tile(tileSetting[DEFINITION_TILESET_POSITION]);
                                tileSetting.lookupValue(DEFINITION_TILESET_DELAY, newTile->delayInMilliseconds);
                                if (oldTile)    { oldTile->next = newTile; }
                                else            { tileset->tiles[index] = newTile; }
                                oldTile = newTile;
                            }
                        }
                    }
                }
            }
        }
        catch(libconfig::SettingTypeException e) { }
    }

    // GET THE ALPHA COLOR IF ANY
    if (_setting.exists(DEFINITION_ALPHA))
    {
        setAlphaColor((int)_setting[DEFINITION_ALPHA][0], (int)_setting[DEFINITION_ALPHA][1], (int)_setting[DEFINITION_ALPHA][2]);
    }
    else
    {
        alpha = false;
        for (int i=0; i<3; i++) alphaColor[i] = 0;
    }

    // HANDLE THE DISPLAY
    if (_setting.exists(DEFINITION_DISPLAY))
    {
        std::string displayStr;
        _setting.lookupValue(DEFINITION_DISPLAY, displayStr);
    }

    if (!fashion->getStyle()->getWidth())    { fashion->getStyle()->setWidth(getWidth()); }
    if (!fashion->getStyle()->getHeight())   { fashion->getStyle()->setHeight(getHeight()); }

}

Image::Image(const std::string & _id, Image * _image):
    splashouilleImpl::Object(_id), display(crop), original(0), tileset(0), tileIndex(-1)
{
    type        = TYPE_IMAGE;
    setFilename(_image->getFilename());

    tileset = _image->getTileset();
    if (tileset) { tileset->nbUsages++; }

    cloneFashion(_image);

    // TODO: since the image surfaces are static, the following line is weird
    int r,g,b; if ((alpha = _image->getAlphaColor(r,g,b))) { setAlphaColor(r,g,b); }
}

Image::Image(const std::string & _id): splashouilleImpl::Object(_id), tileset(0), tileIndex(-1)
{
    type        = TYPE_IMAGE;
    original    = 0;
    surface     = 0;
}

Image::~Image()
{
    if (surface && surface!=original)   { SDL_FreeSurface(surface); }

    // UPDATE THE SURFACES CACHE
    std::map<std::string, Image::Surface*>::iterator it = surfaces.find(filename);
    if (it!=surfaces.end())
    {
        if (!--it->second->nbUsages)
        {
            SDL_FreeSurface(it->second->surface);
            surfaces.erase(it);
        }
    }

    // UPDATE THE TILESETS CACHE
    if (tileset && !--tileset->nbUsages)
    {
        for (std::list<Image::Tileset*>::iterator it = tilesets.begin(); it!=tilesets.end(); it++)
        {
            if ((*it)==tileset) { it=tilesets.erase(it); delete tileset; tileset=0;}
        }
    }

}

/**
 * Set the tile index of the image
 * @param _tileIndex is the new tile index
 */
void Image::setTileIndex(int _tileIndex)
{
    if (_tileIndex!=tileIndex && tileset && tileset->tiles[_tileIndex])
    {
        // SAVE THE CURRENT STYLE
        Fashion * newFashion = new Fashion();
        newFashion->getStyle()->copy(fashion->getStyle());

        // REMOVE THE OLD FASHIONS
        for (FashionMap::iterator vIt=fashions.begin(); vIt!=fashions.end(); vIt++) { delete vIt->second; }
        fashions.clear();

        // BUILD A NEW FASHION
        fashionId           = "default";
        fashion             = newFashion;
        initialTimestamp    = -1;
        fashions.insert(std::pair<std::string, Fashion*>(fashionId, fashion));

        Tileset::Tile * tile = tileset->tiles[_tileIndex];

        // CHECK IF THE TILESET ANIMATION IS LOOPING
        Tileset::Tile * tileTmp = tile;
        int period = 0;
        int loop = true;
        while (tileTmp) {
            period += tileTmp->delayInMilliseconds;
            if (!tileTmp->delayInMilliseconds) { loop=false;}
            tileTmp=tileTmp->next;
        };
        if (!loop) { period = 0; }

        // CREATE THE TRANSITIONS
        int timestamp = 0;
        while (tile)
        {
            splashouille::Style * style = fashion->addTransition(timestamp, timestamp, 1, 1, period);
            style->setPosition(tile->position[0], tile->position[1]);
            if (tile->size[0])  { style->setWidth(tile->size[0]); }
            if (tile->size[1])  { style->setHeight(tile->size[1]); }
            if (tile->offset[0]){ style->setRelativeLeft(tile->offset[0]); }
            if (tile->offset[1]){ style->setRelativeTop(tile->offset[1]); }

            timestamp+=tile->delayInMilliseconds;
            tile = tile->next;
        };

        tileIndex = _tileIndex;

    }
    else
    {
        splashouille::Style * style = fashion->getStyle();
        style->setWidth(0); style->setHeight(0);
    }
}

 /**
  * Render the object into the surface canvas
  * @param _surface is the surface to fill
  * @param _offset is the parent offset
  * @return true
  */
bool Image::render(SDL_Surface * _surface, SDL_Rect * _offset)
{
    const splashouille::Style * style = fashion->getCurrent();

    // UPDATE THE SDL_RECT POSITION IF NECESSARY
    if (style->getDisplay() && surface)
    {
        SDL_SetAlpha(surface, SDL_SRCALPHA | SDL_RLEACCEL, style->getOpacity());

        // HANDLE THE POSITION REGARDING THE PARENT OFFSET (IF ANY)
        SDL_Rect vPosition;
        SDL_Rect vSource;
        splashouille::Engine::copy(&vPosition, position);
        splashouille::Engine::copy(&vSource, source);
        if (_offset) {
            splashouille::Engine::offset(&vPosition, _offset);

            if (vPosition.y<_offset->y)
            {
                if (vPosition.h>(_offset->y-vPosition.y)) { vPosition.h-=(_offset->y-vPosition.y); } else { vPosition.h = 0; }
                vSource.y-=(vPosition.y-_offset->y);
                vPosition.y = _offset->y;
            }

            if (vPosition.x<_offset->x)
            {
                if (vPosition.w>(_offset->x-vPosition.x)) { vPosition.w-=(_offset->x-vPosition.x); } else { vPosition.w = 0; }
                vSource.x-=(vPosition.x-_offset->x);
                vPosition.x = _offset->x;
            }

            vSource.w = vPosition.w;
            vSource.h = vPosition.h;
        }

        // DRAW THE IMAGE
        if (vPosition.w>0 && vPosition.h>0) { SDL_BlitSurface(surface, &vSource, _surface, &vPosition); }
    }


    return true;
}

/**
 * Log the Image to the standard output
 * @param _rank is the log rank
 */
void Image::log(int _rank) const
{
    std::string offset; offset.append(4*_rank,' ');
    std::cout<<offset<<"+ Image (id: "<<id<<") ("<<filename<<") (fashions: "<<fashions.size()<<") ("<<getWidth()<<"x"<<getHeight()<<") (state: "<<(surface?"OK)":"KO)")<<" (time: "<<initialTimestamp<<")";
    if (alpha) { std::cout<<" (alpha: ["<<alphaColor[0]<<","<<alphaColor[1]<<","<<alphaColor[2]<<"])"; }
    std::cout<<std::endl;

    fashion->log(_rank);
}

