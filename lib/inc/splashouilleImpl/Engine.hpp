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

#ifndef SPLASHOUILLEIMPL_ENGINE_HPP_
#define SPLASHOUILLEIMPL_ENGINE_HPP_

#include <splashouille/Engine.hpp>
#include <splashouilleImpl/Animation.hpp>
#include <list>

namespace splashouilleImpl
{
class Library;

class Engine : virtual public splashouille::Engine, virtual public splashouilleImpl::Animation
{
public:
    static splashouille::Object *       mouse;          // The object as mouse pointer
    static int                          mouseOffset[2]; // The mouse offset
    static mouseModeEnum                mouseMode;      // The mouse mode
    static std::string                  locale;         // The locale value ["fr", "en"]
    static bool                         debug;          // Debug mode

private:
    Library *                           library;        // The general library
    bool                                running;        // Is the animation currently running
    int                                 frame;          // The frame number (from the beginning of the animation)
    SDL_Surface *                       background;     // The background surface
    int                                 fps;            // The framerate per second
    bool                                onPause;        // Is the engine on pause
    Uint32                              begin;          // The animation begin timestamp
    Uint32                              now;            // The now animation
    int                                 progress;       // The progress value used during the threaded import

    class ListenerElement {
    public:
        splashouille::Engine::Listener * listener;
        int priority;
        ListenerElement(splashouille::Engine::Listener * _listener, int _priority):listener(_listener), priority(_priority){}
    };
    std::list<ListenerElement*>         listeners;

    /**
    * Flip the surface
    * @param _surface
    */
    void flip(SDL_Surface * _surface);

public:
    Engine(Library * _library);
    ~Engine();

    /** Some accessors */
    splashouille::Library *             getLibrary();
    bool                                isRunning()                             { return running; }
    bool                                isOnPause()                             { return onPause; }
    const std::string &                 getLocale()                             { return locale; }
    int                                 getProgress() const                     { return progress; }
    splashouille::Object *              getMouse() const                        { return mouse; }
    void                                setDebug(bool _value = true)            { debug = _value; }
    void                                setLocale(const std::string & _locale)  { locale = _locale; }
    void                                setProgress(int _value=100)             { progress = _value; }

    /**
     * Set the mouse mode
     * @param _mode is the mouse mode
     */
    static void setMouseMode(mouseModeEnum _mode)
    {
        mouseMode = _mode;
        if (_mode==none || _mode==object) { SDL_ShowCursor(SDL_DISABLE); }
    }

    /**
     * Import an animation from configuration file
     * @param _setting is the configuration setting
     * @param _library is the library
     * @return true if succeed
     */
    bool import(libconfig::Setting & _setting) { return Animation::import(_setting); }

    /**
     * Set the mouse pointer cursor
     * @param _object is the object used as mouse pointer
     * @param _offsetX is the x-offset of the cursor: 0 is the left side, 10 is the right side
     * @param _offsetY is the y-offset of the cursor: 0 is the top side, 10 is the bottom side
     */
    static void setMouse(splashouille::Object * _object, int _offsetX = 0, int _offsetY = 0)
    {
        mouse = _object; mouseOffset[0] = _offsetX; mouseOffset[1] = _offsetY; setMouseMode(object);
    }

    /**
     * Import the engine from configuration file
     * @param _config is the configuration setting
     * @param _thread is true for importing in a separated thread
     * @return true if succeed
     * @see getProgress() to get the progress
     */
    bool import(libconfig::Config * _config, bool _thread = false);

    /**
     * Add a listener
     * @param _listener is a new Engine::Listener
     * @param _priority is the optional listener priority (1 first, then 2, etc.)
     */
    void addListener(splashouille::Engine::Listener * _listener, int _priority = 1);

    /**
     * Delete a listener
     * @param _listener is the listener to delete
     * @return true if the listener has been found and deleted
     */
    bool deleteListener(splashouille::Engine::Listener * _listener);

    /**
     * Stop the engine
     */
    void stop();

    /**
     * Pause/Play the engine
     */
    void pause();

    /**
     * Set the framerate
     * @param _fps is the new framerate per second
     */
    void setFPS(int _fps) { fps = _fps; }

    /**
     * Run the animation
     * @param _surface is the SDL surface for rendering the animation
     * @return true if everything is fine
     */
    bool run(SDL_Surface * _surface);

    /**
     * Log the engine to the standard output
     * @param _rank is the log rank
     */
    void log(int _rank = 0) const;

    /**
     * Erase the updated parts (if any)
     */
    void fillWithBackground();

    bool isEngine() const { return true; }


};

}


#endif

 
