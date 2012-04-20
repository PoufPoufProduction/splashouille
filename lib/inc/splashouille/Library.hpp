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

#ifndef SPLASHOUILLE_LIBRARY_HPP_
#define SPLASHOUILLE_LIBRARY_HPP_

#include <splashouille/Object.hpp>

#include <string>

namespace libconfig
{
class Setting;
}

namespace splashouille
{
class Solid;
class Image;
class Animation;
class Sound;

/**
 * The main generic class
 */
class Library
{
public:
    /**
     * The class object listener
     */
    class Listener
    {
    public:
        virtual void onCreate       (splashouille::Object * _object UNUSED) {}
        virtual void onDelete       (splashouille::Object * _object UNUSED) {}
    };

public:

    /**
     * Import and prepare the library
     * @param _library is the objects library
     * @return true if everything is fine
     */
    virtual bool import(libconfig::Setting & _library) = 0;

    /**
     * Create an object from a configuration setting
     * @param _setting is the configuration setting
     * @return a new allocated object
     */
    virtual splashouille::Object * createObject(libconfig::Setting & _setting) = 0;

    /** Create empty object */
    virtual splashouille::Object *          createObject(const std::string & _id, const std::string & _type) = 0;
    virtual splashouille::Solid *           createSolid(const std::string & _id) = 0;
    virtual splashouille::Image *           createImage(const std::string & _id) = 0;
    virtual splashouille::Animation *       createAnimation(const std::string & _id) = 0;
    virtual splashouille::Sound *           createSound(const std::string & _id) = 0;

    /**
     * Copy an object from an existing object
     * @param _parent is the object id to copy
     * @param _id is the new object id
     * @return a new allocated object
     */
    virtual splashouille::Object * copyObject(const std::string & _parent, const std::string & _id) = 0;

    /**
     * Delete an object
     * @param _id is the instance key to delete
     * @return true if succeed
     */
    virtual bool deleteObject(const std::string & _id) = 0;

    /**
     * Delete an object
     * @param _obj is the object to delete
     * @return true if succeed
     */
    virtual bool deleteObject(splashouille::Object * _obj) = 0;

    /** Some accessors */
    virtual int                             getSize() const = 0;
    virtual splashouille::Object *          getObjectById(const std::string & _id) const = 0;
    virtual splashouille::Solid *           getSolidById(const std::string & _id) const = 0;
    virtual splashouille::Image *           getImageById(const std::string & _id) const = 0;
    virtual splashouille::Animation *       getAnimationById(const std::string & _id) const = 0;
    virtual splashouille::Sound *           getSoundById(const std::string & _id) const = 0;

    /**
     * set the library listener
     * @param _listener is the new library listener
     */
    virtual void setListener(splashouille::Library::Listener * _listener) = 0;

    /**
     * Log the library to the standard output
     */
    virtual void log() const = 0;

};

}


#endif
