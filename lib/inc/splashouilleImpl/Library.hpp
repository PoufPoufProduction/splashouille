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

#ifndef SPLASHOUILLEIMPL_LIBRARY_HPP_
#define SPLASHOUILLEIMPL_LIBRARY_HPP_

#include <splashouille/Library.hpp>

#include <map>
#include <string>

namespace splashouilleImpl
{

class Library : public splashouille::Library
{
private:
    std::map<std::string, splashouille::Object *>   library;
    splashouille::Library::Listener *               listener;

    static int                                      nbObjects;

    /**
     * The internal delete object
     * @param _it is the internal map iterator
     * @param _erase the iterator from the map
     * @return true if success
     */
    bool deleteObject(const std::map<std::string, splashouille::Object *>::iterator & _it, bool _erase = true);

public:
    Library();
    ~Library();

    /**
     * Insert a object into the library
     * @param _key is the element key as string
     * @param _object is the object to insert
     * @return true if succeed
     */
    bool insertObject(const std::string & _key, splashouille::Object * _object);

    /**
     * Import and prepare the library
     * @param _library is the objects library
     * @return true if everything is fine
     */
    bool import(libconfig::Setting & _library);

    /**
     * Create an object from a configuration setting
     * @param _setting is the configuration setting
     * @return a new allocated object
     */
    splashouille::Object * createObject(libconfig::Setting & _setting);

    /** Create empty object */
    splashouille::Object *       createObject(const std::string & _id, const std::string & _type);
    splashouille::Solid *        createSolid(const std::string & _id);
    splashouille::Image *        createImage(const std::string & _id);
    splashouille::Animation *    createAnimation(const std::string & _id);
    splashouille::Sound *        createSound(const std::string & _id);

    /**
     * Copy an object from an existing object
     * @param _parent is the object id to copy
     * @param _id is the new object id
     * @return a new allocated object
     */
    splashouille::Object * copyObject(const std::string & _parent, const std::string & _id);

    /**
     * Delete an Object
     * @param _id is the Object key to delete
     * @return true if succeed
     */
    bool deleteObject(const std::string & _id);

    /**
     * Delete an Object
     * @param _obj is the object to delete
     * @return true if succeed
     */
    bool deleteObject(splashouille::Object * _obj);

    /** Some accessors */
    int                                     getSize() const { return library.size(); }
    splashouille::Solid *                   getSolidById(const std::string & _id) const;
    splashouille::Image *                   getImageById(const std::string & _id) const;
    splashouille::Animation *               getAnimationById(const std::string & _id) const;
    splashouille::Sound *                   getSoundById(const std::string & _id) const;
    splashouille::Object *                  getObjectById(const std::string& _id) const
    {
        std::map<std::string, splashouille::Object *>::const_iterator it = library.find(_id);
        return (it!=library.end())?it->second:0;
    }

    /**
     * set the library listener
     * @param _listener is the new library listener
     */
    void setListener(splashouille::Library::Listener * _listener) { listener = _listener; }

    /**
     * Log the library to the standard output
     */
    void log() const;


};

}

#endif

