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

#ifndef SPLASHOUILLE_SOUND_HPP_
#define SPLASHOUILLE_SOUND_HPP_

#include <splashouille/Object.hpp>

namespace splashouille
{
/**
 * THE SOUND OBJECT\n
 * Because of the SDL use, there are two types of sound : chunk and music
 */
class Sound : virtual public splashouille::Object
{
public:

    /** Change the main volume */
    static void changeVolume(int _value);
    static void changeVolume(float _value);

    /** Chunk accessor */
    virtual bool getChunk() const = 0;

    /** @return the sound filename as string */
    virtual const std::string & getFilename() const = 0;

    /**
     * create the sound from its filename
     * @param _filename is the image filename
     * @param _isChunk is true if the sound is a chunk
     * @return true is the image is new (ie the filename has never been used) 
     */
    virtual bool setFilename(const std::string & _filename, bool _isChunk) = 0;
};

}


#endif

 
 
