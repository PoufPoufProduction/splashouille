#ifndef SPLASHOUILLE_PLAYER_HPP_
#define SPLASHOUILLE_PLAYER_HPP_

#include <string>
#include <splashouille/Engine.hpp>
#include <SDL.h>

namespace splashouille
{

const static char           OPTION_FPS          = 'f';
const static char           OPTION_SIZE         = 's';
const static char           OPTION_DEBUG        = 'd';
const static char           OPTION_VERBOSE      = 'e';
const static char           OPTION_VERSION      = 'v';
const static char           OPTION_HELP         = 'h';

class Player : public splashouille::Engine::Listener
{
private:
    int                         screenSize[2];
    int                         screenDepth;
    std::string                 filename;
    splashouille::Engine *      engine;
    bool                        running;
    int                         fps;
    bool                        debug;
    bool                        verbose;
public:
    Player():screenDepth(32), engine(0), running(true), fps(0), debug(false), verbose(false)
    {
        screenSize[0] = 640;
        screenSize[1] = 480;
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
    bool onEvent(SDL_Event & _event, int _timeStampInMilliSeconds);

    /**
     * Callback on the quit event
     * @return true if the event is consumed
     */
    bool onStop();

    /**
     * Run the player
     */
    void run();

    /**
     * Parse the command line arguments
     * @param _argc is the number of arguments
     * @param _argv is the arguments values
     * @return true if correct
     */
    bool getopt(int _argc, char ** _argv);
};
}

#endif
