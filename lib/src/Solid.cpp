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
#include <splashouilleImpl/Solid.hpp>
#include <splashouilleImpl/Fashion.hpp>
#include <splashouille/Style.hpp>
#include <libconfig.h++>
#include <iostream>

#include <SDL.h>


using namespace splashouilleImpl;

Solid::Solid(const std::string & _id, libconfig::Setting & _setting) : splashouilleImpl::Object(_id)
{
    type = TYPE_SOLID;

    // Fashion and style import
    Object::import(_setting);
}

Solid::Solid(const std::string & _id, Solid * _solid) : splashouilleImpl::Object(_id)
{
    type = TYPE_SOLID;

    // Copy the fashion
    cloneFashion(_solid);
}

Solid::Solid(const std::string & _id) : splashouilleImpl::Object(_id)
{
    type = TYPE_SOLID;
}

Solid::~Solid()
{
    outCrowd();
}

/**
 * Delete the surface when out of crowd
 * @return true
 */
bool Solid::outCrowd()
{
    if (surface)    { SDL_FreeSurface(surface); surface = 0; }
    return true;
}

/**
 * Render the object into the surface canvas
 * @param _surface is the surface to fill
 * @param _offset is the parent offset
 * @return true
 */
bool Solid::render(SDL_Surface * _surface, SDL_Rect * _offset)
{
    const splashouille::Style * style = fashion->getCurrent();

    // Update the SDL_Rect position if necessary
    //if (style->getDisplay() && updateArea->w && updateArea->h)
    if (style->getDisplay())
    {
        // Rebuild the surface if it is too small (create a greater surface than needed for avoiding creation)
        if (!surface || (surface->w<position->w || surface->h<position->h))
        {
            if (surface) { SDL_FreeSurface(surface); }
            if (position->w==0) { position->w=1; }
            if (position->h==0) { position->h=1; }

            // Note: not really expert with the SDL_Surface behaviour, not sure to know why tmp is necessary
            SDL_Surface * tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, position->w*1.4, position->h*1.4, 32,
                                                     RED_MASK, GREEN_MASK, BLUE_MASK, ALPHA_MASK);
            surface = SDL_DisplayFormat(tmp);
            SDL_FreeSurface(tmp);

            style->getBackgroundColor(red, green, blue);
            SDL_FillRect(surface, 0, SDL_MapRGB(surface->format, red, green, blue));
        }

        // Handle the color if necessary
        int r,g,b; style->getBackgroundColor(r, g, b);
        if ((red!=r)||(green!=g)||(blue!=b))
        {
            red = r; green = g; blue = b;
            SDL_FillRect(surface, 0, SDL_MapRGB(surface->format, red, green, blue));
        }

        // Handle the solid opacity
        SDL_SetAlpha(surface, SDL_SRCALPHA | SDL_RLEACCEL, style->getOpacity());

        // Handle the parent offset if any
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

        // Draw the solid
        if (vPosition.w>0 && vPosition.h>0) { SDL_BlitSurface(surface, &vSource, _surface, &vPosition); }

    }

    return true;
}

/**
 * Log the Solid to the standard output
 * @param _rank is the log rank
 */
void Solid::log(int _rank) const
{
    std::string offset; offset.append(4*_rank,' ');
    std::cout<<offset<<"+ Solid (id: "<<id<<") (timestamp: "<<initialTimestamp<<") (fashions: "<<fashions.size()<<")"<<std::endl;
    fashion->log(_rank);
}
