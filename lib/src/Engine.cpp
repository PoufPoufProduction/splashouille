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

#include <splashouilleImpl/Timeline.hpp>
#include <splashouilleImpl/Engine.hpp>
#include <splashouilleImpl/Library.hpp>
#include <splashouilleImpl/Animation.hpp>
#include <splashouilleImpl/Fashion.hpp>
#include <splashouilleImpl/Style.hpp>
#include <splashouilleImpl/Crowd.hpp>
#include <libconfig.h++>
#include <iostream>
#include <iomanip>

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_thread.h>


using namespace splashouilleImpl;

/** splashouille init SDL method */
bool splashouille::Engine::init()
{
    bool ret = true; 

    //Initialisation de tous les sous-systèmes de SDL
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )                         { ret = false; }

    //Initialisation de SDL_mixer
    if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )     { ret = false; }

    return ret;
}

/** splashouille factory methodes */
splashouille::Engine * splashouille::Engine::createEngine()
{
    splashouilleImpl::Library * l       = new splashouilleImpl::Library();
    splashouille::Engine *      ret     = new splashouilleImpl::Engine(l);
    l->insertObject(ROOT, ret);
    return ret;
}
void splashouille::Engine::deleteEngine(splashouille::Engine * _engine)
{
    delete dynamic_cast<splashouilleImpl::Engine*>(_engine);
}

/** Static values */
bool                                Engine::debug           = false;
splashouille::Object *              Engine::mouse           = 0;
int                                 Engine::mouseOffset[2]  = {0, 0};
splashouille::Engine::mouseModeEnum Engine::mouseMode       = splashouille::Engine::inactive;
std::string                         Engine::locale          = "fr";


/**
 * Add an SDL_Rect to another one
 * @param _source is the SDL_Rect to update
 * @param _add is the SDL_Rect to add
 */
void splashouille::Engine::add(SDL_Rect * _dest, const SDL_Rect * _source)
{
    if (_dest && _source)
    {
        if ( !_dest->w || !_dest->h)
        {
            _dest->x = _source->x;
            _dest->y = _source->y;
            _dest->w = _source->w;
            _dest->h = _source->h;
        }
        else
        if ( _source->w && _source->h )
        {
            int x1 = _dest->x+_dest->w;
            int x2 = _source->x+_source->w;
            int y1 = _dest->y+_dest->h;
            int y2 = _source->y+_source->h;

            _dest->x = _dest->x<_source->x?_dest->x:_source->x;
            _dest->y = _dest->y<_source->y?_dest->y:_source->y;
            _dest->w = (x1>x2?x1:x2)-_dest->x;
            _dest->h = (y1>y2?y1:y2)-_dest->y;
        }
    }
}

/**
 * offset an SDL_Rect with another one
 * @param _source is the SDL_Rect to update
 * @param _add is the SDL_Rect to offset
 */
void splashouille::Engine::offset(SDL_Rect * _dest, const SDL_Rect * _source)
{
    if (_dest && _source)
    {
        if ( _source->w && _source->h )
        {
            _dest->w = (_dest->x+_dest->w<_source->w)?_dest->w:(_source->w>_dest->x?_source->w-_dest->x:0);
            _dest->h = (_dest->y+_dest->h<_source->h)?_dest->h:(_source->h>_dest->y?_source->h-_dest->y:0);
            _dest->x = _dest->x+_source->x;
            _dest->y = _dest->y+_source->y;
        }
    }
}

/**
 * Copy an SDL_Rect
 * @param _source is the SDL_Rect to update
 * @param _add is the SDL_Rect to copy
 */
void splashouille::Engine::copy(SDL_Rect * _dest, const SDL_Rect * _source)
{
    if (_dest && _source) { _dest->x = _source->x; _dest->y = _source->y; _dest->w = _source->w; _dest->h = _source->h; }
}

/**
 * are two rects intersecting ?
 * @param _first is the first rect
 * @param _second is the second rect
 * @return true if the two rects are intersecting
 */
bool  splashouille::Engine::inter(const SDL_Rect * _first, const SDL_Rect * _second)
{
    bool    ret = true;

    if (_first->x + _first->w < _second->x)         { ret = false; }
    if (_first->y + _first->h < _second->y)         { ret = false; }
    if (_first->x > _second->x + _second->w )       { ret = false; }
    if (_first->y > _second->y + _second->h )       { ret = false; }

    return ret;
}


// TODO: are Object and Animation constructors both mandatory ?
Engine::Engine(Library * _library): Object(ROOT), Animation(ROOT, _library),
    library(_library), running(false), frame(0), background(0), fps(0), onPause(true)
{
    isStatic(true);
}

Engine::~Engine()
{
    delete library;
    for (std::list<ListenerElement*>::iterator it=listeners.begin(); it!=listeners.end(); it++) { delete (*it); }
}

/** Some accessors */
splashouille::Library *      Engine::getLibrary()    { return library; }

static libconfig::Config * config_thread = 0;
/** The thread import method */
static int thread_import(void * _engine)
{
    bool ret = true;
    Engine * engine = reinterpret_cast<Engine*>(_engine);

    try { ret = engine->getLibrary()->import(config_thread->lookup("splashouille.animation.library"));}
    catch (libconfig::SettingNotFoundException e) { std::cerr<<e.what()<<std::endl; ret = false; }

    try { ret = engine->import(config_thread->lookup("splashouille.animation"));}
    catch (libconfig::SettingNotFoundException e) { std::cerr<<e.what()<<std::endl; ret = false; }

    engine->setProgress(100);

    return ret;
}

/**
 * Import the engine from configuration file
 * @param _config is the configuration setting
 * @param _thread is true for importing in a separated thread
 * @return true if succeed
 * @see getProgress() to get the progress
 */
bool Engine::import(libconfig::Config * _config, bool _thread)
{
    bool ret = true;
    progress = 0;
    config_thread = _config;

    if (_thread)
    {
        SDL_Thread * thread = SDL_CreateThread(thread_import, this);
        if (!thread) { ret = false; }
    }
    else
    {
        ret = thread_import(this);
    }


    return ret;
}

/**
 * Stop the engine
 */
void Engine::stop()
{
    running   = false;
}

/**
 * Pause/Play the engine
 */
void Engine::pause()
{
    if (!(onPause=!onPause)) { int delta=now-begin; now = SDL_GetTicks(); begin = now-delta; }
}

/**
 * Add a listener
 * @param _listener is a new Engine::Listener
 * @param _priority is the optional listener priority (1 first, then 2, etc.)
 */
void Engine::addListener(splashouille::Engine::Listener * _listener, int _priority)
{
    ListenerElement * listener = new ListenerElement(_listener, _priority);

    std::list<ListenerElement*>::iterator position = listeners.begin();
    for (std::list<ListenerElement*>::iterator it=listeners.begin(); it!=listeners.end(); it++)
    {
        if ( (*it)->priority <= _priority) { position = it; }
    }
    position++;
    listeners.insert(position, listener);
}

/**
 * Delete a listener
 * @param _listener is the listener to delete
 * @return true if the listener has been found and deleted
 */
bool Engine::deleteListener(splashouille::Engine::Listener * _listener)
{
    bool ret = false;

    for (std::list<ListenerElement*>::iterator it=listeners.begin(); it!=listeners.end(); it++)
    {
        if ( (*it)->listener <= _listener) { delete (*it); it = listeners.erase(it); ret = true; }
    }

    return ret;
}

/**
 * Flip the surface
 * @param _surface
 */
void Engine::flip(SDL_Surface * _surface)
{
    SDL_Flip(_surface);

    // TO INVESTIGATE: FLIC WITH SLOW HARDWARE
    /*
    if (numberOfPixels > surface->w*surface->h) { SDL_Flip(_surface); }
    else                                        { SDL_UpdateRects(_surface, nbUpdateRects, updateRects); }
    */
}


/**
 * Erase the updated parts (if any)
 */
void Engine::fillWithBackground()
{
    if (numberOfPixels && nbUpdateRects && background)
    {
        if (numberOfPixels > surface->w*surface->h)
        {
            SDL_BlitSurface(background, 0, surface, 0);
        }
        else
        {
            for (int i=0; i<nbUpdateRects; i++)
            {
                SDL_BlitSurface(background, &updateRects[i], surface , &updateRects[i]);
                //Debug: SDL_FillRect(surface, &updateRects[i], SDL_MapRGB(surface->format, rand()%255, rand()%255, rand()%255));
            }
        }
    }
}

/**
 * Run the animation
 * @param _surface is the SDL surface for rendering the animation
 * @return true if everything is fine
 */
bool Engine::run(SDL_Surface * _surface)
{
    SDL_Event       event;
    int             frameSec        = 0;
    unsigned int    lastSecond      = 0;
    Uint32          lastNow         = 0;
    Uint16          mouseX          = 0;
    Uint16          mouseY          = 0;
    int             mouseState      = 0;
    bool            rc;
    int             delay           = fps?1000/fps:1;

    // Save the background
    background      = SDL_DisplayFormat(_surface);

    // Some initialisations
    getStyle()->setTop(0);
    getStyle()->setLeft(0);
    getStyle()->setWidth(_surface->w);
    getStyle()->setHeight(_surface->h);
    position->x = 0;
    position->y = 0;
    position->w = _surface->w;
    position->h = _surface->h;

    running = true;
    onPause = false;
    begin   = SDL_GetTicks();
    now     = begin;

    while(running)
    {
        // INIT THE EVENT
        event.type = 0;

        // Handle the poll event
        while (SDL_PollEvent(&event))
        {
            rc = false;

            // The QUIT EVENT
            if (event.type == SDL_QUIT)
            {
                stop();
                for (std::list<ListenerElement*>::iterator it=listeners.begin(); it!=listeners.end(); it++)
                {
                    if (!rc) { rc = (*it)->listener->onStop(); }
                }
            }
            else
            {
                // COMPUTE THE MOUSE EVENT IF ANY
                if (mouseMode == splashouille::Engine::active || mouseMode == splashouille::Engine::object)
                {

                    if (event.type == SDL_MOUSEBUTTONDOWN)
                    {
                        mouseState|=(1<<((int)event.button.button-1));
                    }
                    else
                    if (event.type == SDL_MOUSEBUTTONUP)
                    {
                        if (mouseState & (1<<((int)event.button.button-1)))
                        {
                            mouseState^=(1<<((int)event.button.button-1));
                        }
                    }
                    else
                    if (event.type == SDL_MOUSEMOTION)
                    {
                        mouseX = event.motion.x;
                        mouseY = event.motion.y;
                        if (mouse)
                        {
                            mouse->getStyle()->setLeft(mouseX - mouseOffset[0]*mouse->getStyle()->getWidth()/10);
                            mouse->getStyle()->setTop(mouseY - mouseOffset[1]*mouse->getStyle()->getHeight()/10);
                        }
                    }
                }

                // Forward the event to the listeners
                for (std::list<ListenerElement*>::iterator it=listeners.begin(); it!=listeners.end(); it++)
                {
                    if (!rc) { rc = (*it)->listener->onEvent(event, now-begin); }
                }
            }
        }

        // THE MILLISECOND IS THE LIMIT OF THE SDL_GETTICKS()
        if (!onPause && now>=lastNow+delay)
        {
            // FORWARD THE MOUSE EVENT (USER<0 MAKE THE MOUSE TEMPORARY INACTIVE)
            if ( (  mouseMode == splashouille::Engine::active) ||
                 (  mouseMode == splashouille::Engine::object && mouse &&
                    mouse->getStyle()->getDisplay() && mouse->getStyle()->getUser()>=0 ) )
            {
                mouseEvent(now-begin, event.button.x, event.button.y, true, mouseState);
            }

            // UPDATE THE ELEMENTS
            lastNow = now;

            // DEAL WITH THE ONFRAME LISTENER
            for (std::list<ListenerElement*>::iterator it=listeners.begin(); it!=listeners.end(); it++)
            {
                (*it)->listener->onFrame(frame, now-begin);
            }

            // HANDLE THE TIMELINE EVENT
            update(now-begin);

            // DEAL WITH THE ONSECOND LISTENER IF SECOND IS DIFFERENT
            if (lastSecond!=(now-begin)/1000)
            {
                lastSecond = (now-begin)/1000;
                for (std::list<ListenerElement*>::iterator it=listeners.begin(); it!=listeners.end(); it++)
                {
                    (*it)->listener->onSecond(frame, frameSec, lastSecond);
                }

                if (debug) {
                    std::cout<<std::setw(STD_LABEL)<<std::left<<"Engine::onSecond"
                            <<" (second: "<<lastSecond<<") (fps: "<<frameSec<<")"<<std::endl;
                    if (lastSecond%5==0) { log(); }
                }

                frameSec = 0;
            }

            // Render the main animation
            if (numberOfPixels)
            {
                render(_surface);
                if (_surface->flags&SDL_DOUBLEBUF) { flip(_surface); } else flip(_surface);
            }

            frame++; frameSec++;
            clear(); // TODO: move this in Animation::render
        }

        now = SDL_GetTicks();
    }

     SDL_FreeSurface(background);

    return true;
}

/**
 * Log the engine to the standard output
 * @param _rank is the log rank
 */
void Engine::log(int _rank) const
{
    std::cout<<"+ Engine (locale: "<<locale<<") (debug: "<<debug<<")"<<std::endl;
    Animation::log(_rank);

}



