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
#include <splashouilleImpl/Map.hpp>
#include <splashouilleImpl/Fashion.hpp>
#include <splashouilleImpl/Library.hpp>
#include <splashouilleImpl/Crowd.hpp>
#include <splashouille/Style.hpp>
#include <libconfig.h++>
#include <iostream>
#include <cmath>

#include <SDL.h>

using namespace splashouilleImpl;

int Map::counter = 0;

Map::Map(const std::string & _id, libconfig::Setting & _setting, splashouille::Library * _library) :
    splashouilleImpl::Object(_id), library(_library), map(0), mode(ortho), first(true)
{
    type = TYPE_MAP;
    size[0] = size[1] = 1;
    size[2] = size[3] = 16;

    // TAG VALUE BY DEFAULT
    char msgtmp[128];
    snprintf(msgtmp, 128, "map%03d", counter++);
    tag.assign(msgtmp);

    // FASHION AND STYLE IMPORT
    Object::import(_setting);

    // GET THE TYPE OF THE MAP
    std::string strtmp;
    _setting.lookupValue(DEFINITION_MODE, strtmp);
    if (!strtmp.compare("ortho"))   { mode = ortho; } else
    if (!strtmp.compare("iso"))     { mode = iso; }

    // GET THE TILESET
    if (_setting.exists(DEFINITION_TILESET))
    {
        std::string tilesetId;
        _setting.lookupValue(DEFINITION_TILESET, tilesetId);
        if ((tileset = _library->getImageById(tilesetId)))
        {
            // LOAD THE TILES SIZE
            if (_setting.exists(DEFINITION_SIZE) && _setting[DEFINITION_SIZE].getType() == libconfig::Setting::TypeArray)
            {
                for (int i=0; i<_setting[DEFINITION_SIZE].getLength() && i<6 ; i++)
                    size[i] = _setting[DEFINITION_SIZE][i];
            }

            map = new int[size[0]*size[1]];
            memset(map, 0, size[0]*size[1]*sizeof(int));

            // LOAD THE TILES MAP
            if (_setting.exists(DEFINITION_TILES) && _setting[DEFINITION_TILES].getType() == libconfig::Setting::TypeArray &&
                _setting[DEFINITION_TILES].getLength()==size[0]*size[1])
            {
                for (int j=0; j<size[1]; j++) for (int i=0; i<size[0]; i++)
                {
                    map[i+j*size[0]] = _setting[DEFINITION_TILES][i+j*size[0]];
                }
            }
        }
    }
}

Map::Map(const std::string & _id, Map * _map, splashouille::Library * _library) :
    splashouilleImpl::Object(_id), library(_library), map(0), mode(ortho), first(true)
{
    type = TYPE_MAP;
    size[0] = size[1] = 1;
    size[2] = size[3] = 16;

    // Copy the fashion
    cloneFashion(_map);

    // TODO
}

Map::Map(const std::string & _id, splashouille::Library * _library) :
    splashouilleImpl::Object(_id), library(_library), map(0), mode(ortho), first(true)
{
    type = TYPE_MAP;
    size[0] = size[1] = 1;
    size[2] = size[3] = 16;

    // TODO
}

Map::~Map()
{
    delete map;
}

/** @return the tileset image */
splashouille::Image * Map::getTileset() const { return tileset; }

/**
 * Parse the tiles
 * @param _object are the tiles image
 * @param _user is the operation
 * @return true
 */
bool Map::onObject(splashouille::Object * _object, int _user)
{
    if (_object->isImage())
    {
        switch (_user)
        {
        // UPDATE THE TILES POSITION
        case updateTiles :
            {
                int                         state   = _object->getState();
                int                         posX    = state%size[0];
                int                         posY    = state/size[0];
                const splashouille::Style * style = fashion->getCurrent();
                float                       p[2];
                style->getPosition(p[0], p[1]);

                // FORWARD THE RELATIVE POSITION OF THE MAP
                _object->getStyle()->setRelativeLeft(style->getRelativeLeft());
                _object->getStyle()->setRelativeTop(style->getRelativeTop());

                // COMPUTE THE TILE POSITION AND MARK OUTSIDE TILES
                switch(mode)
                {
                case ortho:
                    _object->getStyle()->setLeft(style->getLeft() + posX*size[2] - std::floor(p[0]));
                    _object->getStyle()->setTop(style->getTop() + posY*size[3] - std::floor(p[1]));

                    if (posX<current.position[0] || posX>current.position[2] || posY<current.position[1] || posY>current.position[3])
                    {   toDelete.push_front(_object); }
                    break;
                case iso:
                    _object->getStyle()->setLeft((float) style->getLeft() + (posX - posY + size[1] - 1) * size[2] - std::floor(p[0]));
                    _object->getStyle()->setTop((float) style->getTop() + (posX + posY) * size[3] - std::floor(p[1]));

                   if ( (posX-posY)<current.position[0] || (posX-posY)>current.position[2] ||
                        (posX+posY)<current.position[1] || (posX+posY)>current.position[3])
                    {   toDelete.push_front(_object); }

                    break;
                }
            }
            break;

        // REMOVE ALL THE TILES
        case removeTiles:
            toDelete.push_front(_object);
            break;

        default:
            break;
        }
    }

    return true;
}

/**
 * Compute the limits of the viewed part of the map regarding its position and size
 * @param _style is the current style of the maps
 * @param _limits are the returned computed limits
 */
void Map::getLimits(const splashouille::Style * _style, limits * _limits)
{
    float p[2];
    _style->getPosition(p[0], p[1]);

    switch(mode)
    {
    case ortho:
        _limits->position[0]    = std::floor(p[0]/size[2]);
        _limits->position[1]    = std::floor(p[1]/size[3]);

        p[0]+=_style->getWidth();
        p[1]+=_style->getHeight();
        _limits->position[2]    = std::ceil(p[0]/size[2])-1;
        _limits->position[3]    = std::ceil(p[1]/size[3])-1;
        break;

    case iso:
        _limits->position[0]    = - size[1] + std::floor(p[0]/size[2]);
        _limits->position[1]    = - 1 + std::floor(p[1]/size[3]);

        p[0]+=_style->getWidth();
        p[1]+=_style->getHeight();
        _limits->position[2]    = - size[1] + std::ceil(p[0]/size[2]);;
        _limits->position[3]    = std::ceil(p[1]/size[3]);

        break;
    }
}

/**
 * Update the object regarding the timestamp and add the area to update if any
 * @param _timestamp is the current timestamp
 * @return number of pixels involved in the modification
 */
bool Map::update(int _timestamp)
{
    char msg[128];

    if (splashouilleImpl::Object::update(_timestamp))
    {
        const splashouille::Style * style = fashion->getCurrent();

        getLimits(style, &current);
        getLimits(&lastStyle, &last);

        // INSERT THE NEW TILES INSIDE THE VIEW
        switch(mode)
        {
        case ortho:
            for (int j=current.position[1]; j<=current.position[3]; j++) for (int i=current.position[0]; i<=current.position[2]; i++)
            if (i>=0 && j>=0 && i<size[0] && j<size[1])
            if (first || i<last.position[0] || i>last.position[2] || j<last.position[1] || j>last.position[3] )
            if (map[i+j*size[0]]>=0)
            {
                snprintf(msg, 128, "%s%05d%05d", getId().c_str(), i, j);
                splashouilleImpl::Image * img = dynamic_cast<splashouilleImpl::Image*>(library->copyObject(tileset->getId(), msg));
                img->setTileIndex(map[i+j*size[0]]);
                img->setTag(getTag());
                img->setZIndex(getZIndex()+(size[1]-j));
                img->setState(i+j*size[0]);
                crowd->insertObject(initialTimestamp, img);
            }
            break;
        case iso:
            for (int j=0; j<size[1]; j++) for (int i=0; i<size[0]; i++) // CAN BE OPTIMIZED REGARDING THE LIMITS
            if ((i-j)>=current.position[0] && (i-j)<=current.position[2] && (i+j)>=current.position[1] && (i+j)<=current.position[3])
            if (first || current.position[0]<last.position[0] || current.position[1]<last.position[1] ||
                current.position[2]>last.position[2] || current.position[3]>last.position[3] )
            if (map[i+j*size[0]]>=0)
            {
                snprintf(msg, 128, "%s%05d%05d", getId().c_str(), i, j);
                splashouilleImpl::Image * img = dynamic_cast<splashouilleImpl::Image*>(library->copyObject(tileset->getId(), msg));
                img->setTileIndex(map[i+j*size[0]]);
                img->setTag(getTag());
                img->setZIndex(getZIndex()+i+j);
                img->setState(i+j*size[0]);
                crowd->insertObject(initialTimestamp, img);
            }
            break;
        };

        toDelete.clear();
        crowd->forEach(this, getTag(), true, updateTiles);
        first = false;

        // REMOVE OUT TILES
        while (toDelete.size())
        {
            crowd->dropObject(toDelete.front()->getId());
            library->deleteObject(toDelete.front());
            toDelete.pop_front();
        };

        // SAVE THE CURRENT STYLE
        lastStyle.copy(style);
    }

    return 0;
}


/**
 * Remove all associated tiles
 * @return true
 */
bool Map::outCrowd()
{
    toDelete.clear();
    crowd->forEach(this, getTag(), true, removeTiles);

    while (toDelete.size())
    {
        crowd->dropObject(toDelete.front()->getId());
        library->deleteObject(toDelete.front());
        toDelete.pop_front();
    };
    return true;
}

/**
 * Log the Map to the standard output
 * @param _rank is the log rank
 */
void Map::log(int _rank) const
{
    std::string offset; offset.append(4*_rank,' ');
    std::cout<<offset<<"+ Map (id: "<<id<<") (timestamp: "<<initialTimestamp<<") (fashions: "<<fashions.size()<<")"<<std::endl;
    fashion->log(_rank);
}
