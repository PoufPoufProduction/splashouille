#include <splashouille/Library.hpp>
#include <splashouille/Timeline.hpp>
#include <splashouille/Crowd.hpp>
#include <splashouille/Object.hpp>
#include <splashouille/Solid.hpp>
#include <splashouille/Image.hpp>
#include <splashouille/Style.hpp>
#include <splashouille/Fashion.hpp>
#include <splashouille/Animation.hpp>
#include <splashouille/Defines.hpp>

#include <Balls.hpp>

#include <iostream>

#define BALLS  true

using namespace splashouille;

/**
 * Callback on a object
 * @param _object is an object of the crowd
 * @return false for ending the current process
 */
bool Balls::onObject(splashouille::Object * _object)
{
    bool ret = true;
    if (_object->getStyle()->getTop()>640)
    {
        engine->getCrowd()->dropObject(_object);
        engine->getLibrary()->deleteObject(_object);
        ret = false;
    }
    return ret;
}

/**
 * Callback called each second
 * @param _frame is the frame number from the beginning of the animation
 * @param _frameSec is the number of frames played during the last second
 * @param _second is the current second
 */
void Balls::onSecond(int _frame, int _frameSec, int _second)
{
    std::cout<<"Frame: "<<_frame<<" Second: "<<_second<<" FPS: "<<_frameSec<<" Size: "<<engine->getCrowd()->getSize()<<std::endl;
}

/**
 * Callback called on each frame
 * @param _frame is the frame number from the beginning of the animation
 * @param _timeStampInMilliSeconds is the current timestamp
 */
void Balls::onFrame(int _frame UNUSED, int _timeStampInMilliSeconds)
{
    static int lastTimeStamp = 0;

    if (!engine->isOnPause())
    {
        engine->getCrowd()->forEach(this);

        // A function would be better
        int number = 1+_timeStampInMilliSeconds/2000;

        // Add a sphere
        if (number >= engine->getCrowd()->getSize() && _timeStampInMilliSeconds>lastTimeStamp+(500/number) )
        {
            lastTimeStamp = _timeStampInMilliSeconds;
            char    id[128];
            snprintf(id, 128, "sphere%06d", _timeStampInMilliSeconds%1000000);
            Object * obj = engine->getLibrary()->copyObject(BALLS?"sphere":"solid", id);
            if (!BALLS) { obj->getStyle()->setBackgroundColor(128+(rand()%128), 128+(rand()%128), 128+(rand()%128)); }

            float r = -10.0 - 20.0 * (rand()%1000)/1000;
            Style * style = obj->getFashion()->addTransition(0, 1000, r, 1);
            style->setTop(800);

            r = 416*(rand()%1000)/1000;
            style = obj->getFashion()->addTransition(0, 1000, .5, .5);
            style->setLeft(r);

            engine->getCrowd()->insertObject(_timeStampInMilliSeconds, obj);
        }
    }
}

/**
 * The onEvent Callback
 * @param _event is the SDL_Event
 */
bool Balls::onEvent(SDL_Event & _event, int _timestamp UNUSED)
{
    if (_event.type == SDL_KEYDOWN) {
        Uint8 *keystates = SDL_GetKeyState( NULL );

        if (keystates[' ']) { engine->pause(); }
    }
    return true;
}

/**
 * Callback on the quit event
 * @return true if the event is consumed
 */
bool Balls::onStop() { running = false; return true; }

/**
 * Run the player
 */
void Balls::run()
{
    // CREATE THE SDL WINDOW
    SDL_Surface * screen = SDL_SetVideoMode(screenSize[0], screenSize[1], screenDepth, SDL_SWSURFACE | SDL_DOUBLEBUF );

    // OPEN AND PARSE THE CONFIGURATION FILE
    do {
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 96, 128, 224));
        SDL_Flip(screen);


        // CREATE AND PREPARE THE ANIMATION
        engine = splashouille::Engine::createEngine();
        engine->addListener(this);

        Image * sphere = engine->getLibrary()->createImage("sphere");
        Solid * solid = engine->getLibrary()->createSolid("solid");
        if (!sphere->setFilename("res/img/sphere.bmp"))
        {
            std::cout<<"Can not find res/img/sphere.bmp - Run balls from the splashouille/ folder."<<std::endl;
            running = false;
        }
        else
        {
            // BUILD THE ANIMATION OF THE REFERENCE BALL
            sphere->setAlphaColor();
            sphere->getStyle()->setTop(640);
            sphere->getStyle()->setLeft(208);
            sphere->getStyle()->setWidth(64);
            sphere->getStyle()->setHeight(64);
            sphere->getStyle()->setPosition(0,0);
            for (int y=0; y<4; y++) for (int x=0; x<8; x++)
            {
                int speed = 10;
                Style * style = sphere->getFashion()->addTransition(speed*(x+y*8), speed*(x+y*8), 1, 1, 32*speed);
                style->setPosition(x*64, y*64);
            }

            // BUILD THE SOLID
            solid->getStyle()->setTop(640);
            solid->getStyle()->setLeft(208);
            solid->getStyle()->setWidth(64);
            solid->getStyle()->setHeight(64);
            solid->getStyle()->setBackgroundColor(255, 0, 0);
            solid->getStyle()->setOpacity(200);

            // RUN THE ENGINE
            engine->setFPS(60);
            engine->run(screen);
        }

        splashouille::Engine::deleteEngine(engine);
    } while(running);


}

/**
 * The player main function
 * @param argc is the number of arguments
 * @param argv is the argument values
 * @return 1
 */
int main()
{
    std::cout<<"SPLASHOUILLE Balls v1 - PoufPouf Production 2011"<<std::endl;

    Balls *    balls = new Balls();
    balls->run();

    return 1;
}
