#ifndef SPLASHOUILLE_BALLS_HPP_
#define SPLASHOUILLE__HPP_

#include <string>
#include <splashouille/Engine.hpp>
#include <splashouille/Crowd.hpp>
#include <SDL.h>

namespace splashouille
{
class Balls : public splashouille::Engine::Listener, public splashouille::Crowd::Listener
{
private:
    int                         screenSize[2];
    int                         screenDepth;
    splashouille::Engine *      engine;
    bool                        running;
public:
    Balls():screenDepth(32), engine(0), running(true)
    {
        screenSize[0] = 480;
        screenSize[1] = 640;
    }

    /**
     * Callback called each second
     * @param _frame is the frame number from the beginning of the animation
     * @param _frameSec is the number of frames played during the last second
     * @param _second is the current second
     */
    void onSecond(int _frame, int _frameSec, int _second);

    /**
     * Callback called on each frame
     * @param _frame is the frame number from the beginning of the animation
     * @param _timeStampInMilliSeconds is the current timestamp
     */
    void onFrame(int _frame, int _timeStampInMilliSeconds);

    /**
     * The onEvent Callback
     * @param _event is the SDL_Event
     */
    bool onEvent(SDL_Event & _event, int _timestamp);

    /**
     * Callback on the quit event
     * @return true if the event is consumed
     */
    bool onStop();

    /**
     * Callback on a object
     * @param _object is an object of the crowd
     * @return false for ending the current process
     */
    bool onObject(splashouille::Object * _object);

    /**
     * Run the player
     */
    void run();
};
}

#endif
