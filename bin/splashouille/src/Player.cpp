#include <splashouille/Library.hpp>
#include <splashouille/Timeline.hpp>
#include <splashouille/Crowd.hpp>
#include <splashouille/Object.hpp>
#include <splashouille/Style.hpp>
#include <splashouille/Fashion.hpp>
#include <splashouille/Animation.hpp>
#include <splashouille/Defines.hpp>

#include <Player.hpp>

#include <SDL_mixer.h>

#include <libconfig.h++>

#include <iostream>
#include <iomanip>
#include <getopt.h>


using namespace splashouille;

/**
 * Callback called each second
 * @param _frame is the frame number from the beginning of the animation
 * @param _frameSec is the number of frames played during the last second
 * @param _second is the current second
 */
void Player::onSecond(int _frame UNUSED, int _frameSec UNUSED, int _second UNUSED)
{
    if (verbose)
    {
        std::cout<<"splashouille (second: "<<_second<<") (frame: "<<_frame<<") (fps: "<<_frameSec<<")"<<std::endl;
    }
}

/**
 * Callback called on each frame
 * @param _frame is the frame number from the beginning of the animation
 * @param _timeStampInMilliSeconds is the current timestamp
 */
void Player::onFrame(int _frame UNUSED, int _timeStampInMilliSeconds UNUSED)
{
}

/**
 * The onEvent Callback
 * @param _event is the SDL_Event
 */
bool Player::onEvent(SDL_Event & _event, int _timeStampInMilliSeconds UNUSED)
{

    if (_event.type == SDL_KEYDOWN) {
        Uint8 *keystates = SDL_GetKeyState( NULL );

        if (keystates[' '])             { engine->pause(); }
        else if (keystates[SDLK_F5])    { engine->stop(); }
        else if (keystates['q'])        { engine->stop(); running = false; }
    }
    return true;
}

/**
 * Callback on the quit event
 * @return true if the event is consumed
 */
bool Player::onStop() { running = false; return true; }

/**
 * Parse the command line arguments
 * @param _argc is the number of arguments
 * @param _argv is the arguments values
 * @return true if correct
 */
bool Player::getopt(int _argc, char ** _argv)
{
    int     c;
    bool    ret = true;

    do
    {
        int                     option_index = 0;
        static struct option    long_options[] =
        { {"size",        1, 0, splashouille::OPTION_SIZE },
          {"fps",         1, 0, splashouille::OPTION_FPS },
          {"debug",       0, 0, splashouille::OPTION_DEBUG },
          {"verbose",     0, 0, splashouille::OPTION_VERBOSE },
          {0, 0, 0, 0} };

        c=getopt_long(_argc, _argv, "s:f:de", long_options, &option_index);


        switch (c) {
            case -1:            break;
            case '?':           ret = false; break;
            case 0:             break;
            case OPTION_SIZE:   sscanf(optarg,"%dx%d", &screenSize[0], &screenSize[1]); break;
            case OPTION_FPS:    fps = atoi(optarg); break;
            case OPTION_DEBUG:  debug = true; break;
            case OPTION_VERBOSE:verbose = true; break;
            default:
                if (option_index>=0 && option_index<static_cast<int>(sizeof(long_options)/sizeof(struct option)))
                {
                    std::cout<<"option :"<<long_options[option_index].val<<" : "<<optarg<<std::endl;
                }
                break;
        }
    } while(c!=-1);

    if (optind==_argc-1)    { filename.assign(_argv[optind++]); } else { ret = false; }

    return ret;
}

/**
 * Run the player
 */
void Player::run()
{
    // INIT THE SPLASHOUILLE FRAMEWORK
    splashouille::Engine::init();

    // CREATE THE SDL WINDOW
    std::cout<<"  Screen size: ("<<screenSize[0]<<" x "<<screenSize[1]<<") (depth: "<<screenDepth<<")"<<std::endl;
    SDL_Surface * screen = SDL_SetVideoMode(screenSize[0], screenSize[1], screenDepth, SDL_SWSURFACE );

    // OPEN AND PARSE THE CONFIGURATION FILE
    do {
        bool                rc              = true;
        libconfig::Config * configuration   = new libconfig::Config();
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));SDL_Flip(screen); 

        try { configuration->readFile(filename.c_str()); rc = true;}
        catch(libconfig::FileIOException e) { std::cerr<<e.what()<<std::endl; rc = false; }
        catch(libconfig::ParseException  e) { std::cerr<<e.what()<<std::endl; rc = false; }
        if (!rc) { exit(-1); }

        // CREATE AND PREPARE THE ANIMATION
        engine = splashouille::Engine::createEngine();
        if (fps)    { std::cout<<"  Frames per seconds: "<<fps<<std::endl; engine->setFPS(fps); }
        if (debug)  { std::cout<<"  Debug mode"<<std::endl; engine->setDebug(); }
        engine->setLocale("en");
        rc = engine->import(configuration);

        if (!rc) { std::cerr<<"error on import"<<std::endl; exit(-1); }
        else
        {
            engine->addListener(this);
            // RUN THE APPLICATION
            engine->run(screen, splashouille::Animation::color );
        }

        delete configuration;
        splashouille::Engine::deleteEngine(engine);
    } while(running);


}

/**
 * The player main function
 * @param argc is the number of arguments
 * @param argv is the argument values
 * @return 1
 */
int main(int argc, char ** argv)
{

    std::cout<<"splashouille by Pouf-Pouf Production 2012"<<std::endl;

    Player *    player = new Player();
    if ( player->getopt(argc, argv))
    {
        player->run();
        delete player;
    }
    else
    { std::cout<<"Usage: splashouille [OPTIONS] FILE"<<std::endl; return 0; }

    return 1;

}
