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
#include <splashouilleImpl/Sound.hpp>
#include <splashouilleImpl/Fashion.hpp>
#include <splashouille/Style.hpp>
#include <libconfig.h++>
#include <iostream>

#include <SDL_mixer.h>

using namespace splashouilleImpl;

int splashouilleImpl::Sound::volume = MIX_MAX_VOLUME;

std::map<std::string, Sound::Garbage*>  Sound::sounds;


void splashouille::Sound::changeVolume(int _value)
{
    if (_value<0)               { _value = 0; }
    if (_value>MIX_MAX_VOLUME)  { _value = MIX_MAX_VOLUME; }
    splashouilleImpl::Sound::volume = _value;
}
void splashouille::Sound::changeVolume(float _value)
{
    int value = MIX_MAX_VOLUME*_value;
    changeVolume(value);
}

/**
 * create the sound from its filename
 * @param _filename is the image filename
 * @param _isChunk is true if the sound is a chunk
 * @return true is the image is new (ie the filename has never been used) 
 */
bool Sound::setFilename(const std::string & _filename, bool _isChunk)
{
    bool ret    = true;
    filename    = _filename;

    std::map<std::string, Sound::Garbage*>::iterator it = sounds.find(filename);
    if (it!=sounds.end())
    {
        sound   = &it->second->sound;
        isChunk = it->second->isChunk;
        it->second->nbUsages++;
        ret = false;
    }
    else
    {
        isChunk = _isChunk;
        if (isChunk)
        {
            Garbage * garbage = new Garbage(Mix_LoadWAV(_filename.c_str()));
            sounds.insert(std::pair<std::string, Sound::Garbage*>(filename, garbage));
            sound = &garbage->sound;
        }
        else
        {
            Garbage * garbage = new Garbage(Mix_LoadMUS(_filename.c_str()));
            sounds.insert(std::pair<std::string, Sound::Garbage*>(filename, garbage));
            sound = &garbage->sound;
        }
    }

    return ret;
}

Sound::Sound(const std::string & _id, libconfig::Setting & _setting) : splashouilleImpl::Object(_id)
{
    type = TYPE_SOUND;
    sound = 0;
    isChunk = false;

    // Fashion and style import
    Object::import(_setting);

    // Get the chunk
    _setting.lookupValue(DEFINITION_CHUNK, isChunk);

    // Get the filename
    if (_setting[DEFINITION_FILENAME].getType() == libconfig::Setting::TypeGroup)
    {
        _setting[DEFINITION_FILENAME].lookupValue(Engine::locale, filename);
    }
    else
    {
        _setting.lookupValue(DEFINITION_FILENAME, filename);
    }
    setFilename(filename, isChunk);
}

Sound::Sound(const std::string & _id, Sound * _sound) : splashouilleImpl::Object(_id)
{
    type = TYPE_SOUND;
    sound = 0;

    setFilename(_sound->getFilename(), _sound->getChunk());

    // Copy the fashion
    cloneFashion(_sound);
}

Sound::Sound(const std::string & _id) : splashouilleImpl::Object(_id)
{
    type = TYPE_SOLID;
    sound = 0;
}

Sound::~Sound()
{
    // Update the sound garbages
    std::map<std::string, Sound::Garbage*>::iterator it = sounds.find(filename);
    if (it!=sounds.end())
    {
        if (!--it->second->nbUsages)
        {
            if (it->second->isChunk)    { Mix_FreeChunk( it->second->sound.chunk); }
            else                        { Mix_FreeMusic( it->second->sound.music); }
            sounds.erase(it);
        }
    }
}

/**
 * crowd special callbacks
 */
bool Sound::inCrowd()
{
    if (isChunk)
    {
        if (sound && sound->chunk)
        {
            const splashouille::Style * style = fashion->getStyle();
            Mix_VolumeChunk(sound->chunk, volume * style->getOpacity() / 255);
            Mix_PlayChannel( -1, sound->chunk,0) ;
        }
    }
    else
    {
        Mix_VolumeMusic(0);
        if (sound && sound->music && !Mix_PlayingMusic() ) { Mix_PlayMusic( sound->music,-1) ; }
    }
    return !isChunk;
}

bool Sound::outCrowd()
{
    if (!isChunk && sound && sound->music ) { Mix_HaltMusic(); }

    return true;
}

/**
 * Render the object into the surface canvas
 * @param _surface is the surface to fill
 * @param _offset is the parent offset
 * @return true
 */
bool Sound::render(SDL_Surface * _surface UNUSED, SDL_Rect * _offset UNUSED)
{
    const splashouille::Style * style = fashion->getCurrent();

    if (style->getDisplay() && !isChunk)
    {
        Mix_VolumeMusic(volume * style->getOpacity() / 255);
    }
    return true;

}


/**
 * Log the Sound to the standard output
 * @param _rank is the log rank
 */
void Sound::log(int _rank) const
{
    std::string offset; offset.append(4*_rank,' ');
    std::cout<<offset<<"+ Sound (id: "<<id<<") (timestamp: "<<initialTimestamp<<") (fashions: "<<fashions.size()<<")"<<std::endl;
    fashion->log(_rank);
}
