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

#include <splashouille/Style.hpp>
#include <splashouilleImpl/Engine.hpp>
#include <splashouilleImpl/Animation.hpp>
#include <splashouilleImpl/Fashion.hpp>
#include <splashouilleImpl/Crowd.hpp>
#include <splashouilleImpl/Timeline.hpp>
#include <libconfig.h++>
#include <iostream>
#include <iomanip>

#include <SDL.h>

using namespace splashouilleImpl;

Animation::Animation(const std::string & _id, libconfig::Setting & _setting, splashouille::Library * _library) :
    splashouilleImpl::Object(_id),
    numberOfPixels(0), nbUpdateRects(0), timeline(0), library(_library), donotmove(true), parent(0)
{
    type = TYPE_ANIMATION;

    crowd       = new Crowd(this);
    memset(updateRects, 0, nbUpdateRectsMax*sizeof(SDL_Rect));
    initTimelines();
    import(_setting);

}

Animation::Animation(const std::string & _id, Animation * _animation UNUSED, splashouille::Library * _library) :
    splashouilleImpl::Object(_id),
    numberOfPixels(0), nbUpdateRects(0), timeline(0), library(_library), donotmove(true), parent(0)
{
    type = TYPE_ANIMATION;

    crowd       = new Crowd(this);
    memset(updateRects, 0, nbUpdateRectsMax*sizeof(SDL_Rect));
    initTimelines();

    // TODO
}

Animation::Animation(const std::string & _id, splashouille::Library * _library):
    splashouilleImpl::Object(_id),
    numberOfPixels(0), nbUpdateRects(0), timeline(0), library(_library), donotmove(true), parent(0)
{
    type = TYPE_ANIMATION;

    crowd       = new Crowd(this);
    memset(updateRects, 0, nbUpdateRectsMax*sizeof(SDL_Rect));
    initTimelines();
}

Animation::~Animation()
{
    delete crowd;
    for (TimelineMap::iterator vIt=timelines.begin(); vIt!=timelines.end(); vIt++) { delete vIt->second; }
    if (surface && !isStatic()) { SDL_FreeSurface(surface); }
}

/**
 * initialize the timeline(s)
 */
void Animation::initTimelines()
{
    timeline = new Timeline(this);
    timelines.insert(std::pair<std::string, Timeline*>("default", timeline));
}


splashouille::Timeline * Animation::getTimeline()   { return timeline; }
splashouille::Crowd *    Animation::getCrowd()      { return crowd; }

/**
 * Import an animation from configuration file
 * @param _setting is the configuration setting
 * @return true if succeed
 */
bool Animation::import(libconfig::Setting & _setting)
{
    // Fashion and style import
    Object::import(_setting);

    // Is the animation static
    if (_setting.exists(DEFINITION_STATIC)) { _setting.lookupValue(DEFINITION_STATIC, donotmove); }

    // Handle the timeline(s)
    if (_setting.exists(TIMELINES))
    {
        delete timeline; timelines.clear();
        try {
            for (int i=_setting[TIMELINES].getLength()-1; i>=0; i--)
            {
                libconfig::Setting & setting = _setting[TIMELINES][i];
                timeline = new Timeline(this);
                timeline->import(library, setting[TIMELINE]);
                std::string timelineId;
                setting.lookupValue(TIMELINE_ID, timelineId);
                timelines.insert(std::pair<std::string, Timeline*>(timelineId, timeline));

            }
        }
        catch(libconfig::SettingTypeException e) { }
    }
    else
    if (_setting.exists(TIMELINE))  { timeline->import(library, _setting[TIMELINE]); }

    return true;
}

/**
 * Add an update rect into the animation
 * @param _updateRect is the updateRect to insert
 */
void Animation::addUpdateRect(const SDL_Rect * _updateRect)
{
    if (isStatic() && parent)
    {
        // STATIC ANIMATIONS DO NOT HANDLE ANY UPDATE RECT: THEY FORWARD THEM TO THEIR PARENT
        SDL_Rect updateRect;
        splashouille::Engine::copy(&updateRect, _updateRect);
        splashouille::Engine::offset(&updateRect, position);

        parent->addUpdateRect(&updateRect);
    }
    else
    {
        // CHECK IF THERE IS ANY INTERSECTION WITH PREVIOUS UPDATE RECTS
        int first = -1;
        for (int i=0; i<nbUpdateRects; i++)
        {
            if (first<0)
            {
                if (splashouille::Engine::inter(&updateRects[i], _updateRect))
                {
                    splashouille::Engine::add(&updateRects[i], _updateRect);
                    first = i;
                }
            }
            else
            {
                if (splashouille::Engine::inter(&updateRects[first], &updateRects[i]))
                {
                    splashouille::Engine::add(&updateRects[first], &updateRects[i]);
                    updateRects[i].x = updateRects[i].y = updateRects[i].w = updateRects[i].h = 0;
                }
            }
        }

        // ADD THE NEW UPDATE RECT IF IT DIDN'T INTERSECT ANYONE
        if (nbUpdateRects<nbUpdateRectsMax && first<0)
        {
            splashouille::Engine::copy(&updateRects[nbUpdateRects++], _updateRect);
            numberOfPixels+=_updateRect->w*_updateRect->h;
        }

        // DYNAMIC ANIMATIONS FORWARD THE UPDATE RECTS INFORMATION TO THE PARENT IF
        // IT WASN'T ALREADY DONE BY THE OBJECT::UPDATE METHOD (THE WHOLE ANIMATION IS ON MOVE)
        if (parent && !hasChanged)
        {
            SDL_Rect updateRect;
            splashouille::Engine::copy(&updateRect, _updateRect);
            splashouille::Engine::offset(&updateRect, position);
            parent->addUpdateRect(&updateRect);
        }
    }
}

/**
 * Update the object regarding the timestamp and add the area to update if any
 * @param _timestamp is the current timestamp
 * @return number of pixels involved in the modification
 */
bool Animation::update(int _timestamp)
{
    // Handle the initial timestamp
    if (initialTimestamp<0) { initialTimestamp = _timestamp; }

    int localTimestamp = _timestamp - initialTimestamp;
    if (localTimestamp<0) { localTimestamp=0; }

    bool                    ret=false;

    // UPDATE THE ANIMATION AS AN OBJECT
    // FOR STATIC ANIMATIONS, THIS CALL ONLY MATTERS THE FIRST TIME (THEN IT'S NOT SUPPOSED TO MOVE)
    hasChanged=splashouilleImpl::Object::update(_timestamp);
    if (!isStatic() && hasChanged && parent) { parent->addUpdateRect(updateArea); }

    // UPDATE THE TIMELINE
    timeline->update(_timestamp);
    crowd->update(_timestamp);
    return ret;
}

/**
 * Handle the mouseEvent
 * @param _timestampInMilliSeconds is the current timestamp
 * @param _x is the mouse position on x-axis
 * @param _y is the mouse position on y-axis
 * @param _state is the button action (if any)
 */
bool Animation::mouseEvent(int _timestampInMilliSeconds, int _x, int _y, int _state)
{
    bool ret = false;

    if ((_x>=position->x && _x<=position->x+position->w && _y>=position->y && _y<=position->y+position->h) ||
        (mouseOver) )
    {
        ret = crowd->mouseEvent(_timestampInMilliSeconds, _x-position->x, _y-position->y, _state);
    }
    Object::mouseEvent(_timestampInMilliSeconds, _x, _y, _state);

    return ret;
}

/**
 * Change the current timeline
 * @param _timelineId is the timeline Id as String
 * @return true if the timeline is found
 */
bool Animation::changeTimeline(const std::string & _timelineId, bool _updateInitialTimeStamp )
{
    bool ret = false;
    for (TimelineMap::iterator vIt=timelines.begin(); vIt!=timelines.end(); vIt++)
    {
        if (!vIt->first.compare(_timelineId))
        {
            // Update the animation initialtimestamp
            ret = true;
            timeline = vIt->second;
            timeline->clear();

            if (_updateInitialTimeStamp) { initialTimestamp = -1; }
        }
    }

    if (Engine::debug)
    {
        std::cout<<std::setw(STD_LABEL)<<std::left<<"Animation::changeTimeline"<<" (id:"<<id
            <<") (timeline: "<<_timelineId<<") (timestamp: "<<initialTimestamp<<" ["<<_updateInitialTimeStamp<<"]) (ret: "
            <<(ret?"OK":"KO")<<")"<<std::endl;
    }

    return ret;
}



/**
 * Erase the updated parts (if any)
 */
void Animation::fillWithBackground()
{
    if (numberOfPixels && nbUpdateRects)
    {
        if (numberOfPixels > surface->w*surface->h)
        {
            SDL_FillRect(surface, 0, SDL_MapRGBA(surface->format, 0, 0, 0, 255));
        }
        else
        {
            for (int i=0; i<nbUpdateRects; i++)
            {
                SDL_FillRect(surface, &updateRects[i], SDL_MapRGBA(surface->format, 0, 0, 0, 255));
            }
        }
    }
}

/**
 * Render the object into the surface canvas
 * @param _surface is the surface to fill
 * @param _offset is the parent offset
 * @return true
 */
bool Animation::render(SDL_Surface * _surface, SDL_Rect * _offset)
{
    bool                        ret     = true;
    const splashouille::Style * style   = fashion->getCurrent();

    // Rebuild the surface if it is too small (create a greater surface than needed for avoiding re-creation)
    if (!surface || (surface->w<position->w || surface->h<position->h))
    {
        if (isStatic())
        {
            surface = _surface;
        }
        else
        {
            if (surface) { SDL_FreeSurface(surface); }
            SDL_Surface * tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, position->w*1.4, position->h*1.4, 32,
                                                     RED_MASK, GREEN_MASK, BLUE_MASK, ALPHA_MASK);
            surface = SDL_DisplayFormat(tmp);
            SDL_FreeSurface(tmp);
        }
    }

    if (isStatic() || numberOfPixels)
    {
        // Remove the modified areas by filling with transparent color
        fillWithBackground();

        // Render the animation elements
        if (style->getDisplay())
        {
            // Update the offset of the current animation depending on its static attribute
            SDL_Rect offset;
            SDL_Rect * p_offset = 0;
            if (isStatic())
            {
                offset.x = (_offset?_offset->x : 0) + position->x;
                offset.y = (_offset?_offset->y : 0) + position->y;
                offset.w = _offset?min(_offset->x+_offset->w, offset.x+position->w) - offset.x : position->w;
                offset.h = _offset?min(_offset->y+_offset->h, offset.y+position->h) - offset.y : position->h;
                p_offset = & offset;
            }

            // Render the crowd
            crowd->render(surface, p_offset);
        }
    }

    // Draw the animation surface if it is not static
    if (!isStatic() && style->getDisplay())
    {
        const splashouille::Style * style = fashion->getCurrent();
        if (style->getDisplay())
        {
            SDL_SetAlpha(surface, SDL_SRCALPHA | SDL_RLEACCEL, style->getOpacity());

            // Handle the position regarding the parent offset (if any)
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

            // Draw the image
            if (vPosition.w>0 && vPosition.h>0) { SDL_BlitSurface(surface, &vSource, _surface, &vPosition); }
        }
    }
    return ret;
}

/**
 * Forward the callback
 * @return true
 */
bool Animation::outCrowd() { return crowd->outCrowd(); }


/**
 * Log the animation to the standard output
 * @param _rank is the log rank
 */
void Animation::log(int _rank) const
{
    std::string offset; offset.append(4*_rank,' ');
    std::cout<<offset<<"+ Animation (id: "<<id<<") (static: "<<(isStatic()?"true":"false")<<") (timestamp: "
             <<initialTimestamp<<") (timelines: "<<timelines.size()<<") (fashions: "<<fashions.size()<<") (state: "
             <<state<<")"<<std::endl;
    fashion->log(_rank);
    timeline->log(_rank+1);
    crowd->log(_rank+1);

}


