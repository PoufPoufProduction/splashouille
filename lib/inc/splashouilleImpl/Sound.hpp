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

#ifndef SPLASHOUILLEIMPL_SOUND_HPP_
#define SPLASHOUILLEIMPL_SOUND_HPP_

#include <splashouille/Defines.hpp>
#include <splashouille/Sound.hpp>
#include <splashouilleImpl/Object.hpp>

#include <SDL_mixer.h>

namespace splashouille
{
class Style;
}

namespace splashouilleImpl
{
class Library;

class Sound : virtual public splashouille::Sound, virtual public splashouilleImpl::Object
{
public:
    union Mix_Sound
    {
        Mix_Music *     music;
        Mix_Chunk *     chunk;
    };
    static int                              volume;
private:
    class Garbage
    {
    public:
        Mix_Sound                           sound;
        bool                                isChunk;
        int                                 nbUsages;

        Garbage(Mix_Music * _music): isChunk(false)  { sound.music = _music; nbUsages = 1; }
        Garbage(Mix_Chunk * _chunk): isChunk(true)   { sound.chunk = _chunk; nbUsages = 1; }
    };
    static std::map<std::string, Garbage*>  sounds;               // All the sounds are stored here

private:
    Sound(const std::string & _id, libconfig::Setting & _setting);
    Sound(const std::string & _id, Sound * _sound);
    Sound(const std::string & _id);
    ~Sound();

    bool                                    isChunk;
    std::string                             filename;
    Mix_Sound *                             sound;

public:
    /** Accessors */
    bool                                    isSound() const                             { return true; }
    bool                                    getChunk() const                            { return isChunk; }
    void                                    setChunk(bool _isChunk)                     { isChunk = _isChunk; }
    const std::string &                     getFilename() const                         { return filename; }

    /**
     * create the sound from its filename
     * @param _filename is the image filename
     * @param _isChunk is true if the sound is a chunk
     * @return true is the image is new (ie the filename has never been used) 
     */
    bool setFilename(const std::string & _filename, bool _isChunk);

    /**
     * Render the object into the surface canvas
     * @param _surface is the surface to fill
     * @param _offset is the parent offset
     * @return true
     */
    bool render(SDL_Surface * _surface, SDL_Rect * _offset = 0);

    /**
     * Log the solid to the standard output
     * @param _rank is the log rank
     */
    void log(int _rank = 0) const;

    /**
     * crowd special callbacks
     */
    bool inCrowd(splashouilleImpl::Crowd * _crowd);
    bool outCrowd();


    friend class Library;
};

}


#endif


