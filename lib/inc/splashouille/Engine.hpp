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

#ifndef SPLASHOUILLE_ENGINE_HPP_
#define SPLASHOUILLE_ENGINE_HPP_

#include <string>

#include <SDL.h>
#include <splashouille/Defines.hpp>
#include <splashouille/Animation.hpp>

namespace libconfig
{
class Config;
}

namespace splashouille
{

class Library;

/**
 * The animation class object
 */
class Engine : virtual public Animation
{
public:
    /**
     * The animation listener class
     * Used for getting information and callback durin animation process
     */
    class Listener
    {
    public:
        /**
         * Callback called each second
         * @param _frame is the frame number from the beginning of the animation
         * @param _frameSec is the number of frames played during the last second
         * @param _second is the current second
         */
        virtual void onSecond(int _frame UNUSED, int _frameSec UNUSED, int _second UNUSED) = 0;

        /**
         * Callback called on each frame
         * @param _frame is the frame number from the beginning of the animation
         * @param _timeStampInMilliSeconds is the current timestamp
         */
        virtual void onFrame(int _frame UNUSED, int _timeStampInMilliSeconds UNUSED) = 0;

        /**
         * Callback on SDL event
         * @param _event is the incoming SDL_Event
         * @return true if the event is consumed
         */
        virtual bool onEvent(SDL_Event & _event UNUSED, int _timeStampInMilliSeconds UNUSED) = 0;

        /**
         * Callback on the quit event
         * @return true if the event is consumed
         */
        virtual bool onStop() = 0;
    };

    /** splashouille init SDL method */
    static bool                             init();

    /** splashouille factory methodes */
    static splashouille::Engine *           createEngine();
    static void                             deleteEngine(splashouille::Engine * _engine);

    /**
     * Copy an SDL_Rect
     * @param _dest is the SDL_Rect to update
     * @param _source is the SDL_Rect to copy
     */
    static void                             copy(SDL_Rect * _dest, const SDL_Rect * _source);

    /**
     * Add an SDL_Rect to another one
     * @param _dest is the SDL_Rect to update
     * @param _source is the SDL_Rect to add
     */
    static void                             add(SDL_Rect * _dest, const SDL_Rect * _source);

    /**
     * offset an SDL_Rect with another one
     * @param _dest is the SDL_Rect to update
     * @param _source is the SDL_Rect to offset
     */
    static void                             offset(SDL_Rect * _dest, const SDL_Rect * _source);

    /**
     * are two rects intersecting ?
     * @param _first is the first rect
     * @param _second is the second rect
     * @return true if the two rects are intersecting
     */
    static bool                             inter(const SDL_Rect * _first, const SDL_Rect * _second);

public:

    /** Some accessors */
    virtual splashouille::Library *         getLibrary() = 0;
    virtual bool                            isRunning() = 0;
    virtual bool                            isOnPause() = 0;
    virtual void                            setLocale(const std::string & _locale) = 0;
    virtual const std::string &             getLocale() = 0;
    virtual int                             getProgress() const = 0;
    virtual void                            setDebug(bool _value = true) = 0;

    /**
     * No mouse handling
     */
    virtual void setNoMouse() = 0;

    /**
     * Set the mouse pointer cursor (disable the noMouse value)
     * @param _object is the object used as mouse pointer
     * @param _offsetX is the x-offset of the cursor: 0 is the left side, 10 is the right side
     * @param _offsetY is the y-offset of the cursor: 0 is the top side, 10 is the bottom side
     */
    virtual void setMouse(splashouille::Object * _object, int _offsetX = 0, int _offsetY = 0) = 0;

    /**
     * Import the engine from configuration file
     * @param _config is the configuration setting
     * @param _thread is true for importing in a separated thread
     * @return true if succeed
     * @see getProgress() to get the progress
     */
    virtual bool import(libconfig::Config * _config, bool _thread = false) = 0;

    /**
     * Add a listener
     * @param _listener is a new Engine::Listener
     * @param _priority is the optional listener priority (1 first, then 2, etc.)
     */
    virtual void addListener(splashouille::Engine::Listener * _listener, int _priority = 1) = 0;

    /**
     * Delete a listener
     * @param _listener is the listener to delete
     * @return true if the listener has been found and deleted
     */
    virtual bool deleteListener(splashouille::Engine::Listener * _listener) = 0;

    /**
     * Stop the engine
     */
    virtual void stop() = 0;

    /**
     * Pause/Play the engine
     */
    virtual void pause() = 0;

    /**
     * Set the framerate
     * @param _fps is the new framerate per second
     */
    virtual void setFPS(int _fps) = 0;

    /**
     * Run the animation
     * @param _surface is the SDL surface for rendering the animation
     * @return true if everything is fine
     */
    virtual bool run(SDL_Surface * _surface ) = 0;
};

}


#endif
 
