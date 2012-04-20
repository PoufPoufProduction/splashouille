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

#include <splashouille/Defines.hpp>
#include <splashouilleImpl/Object.hpp>
#include <splashouilleImpl/Engine.hpp>
#include <splashouilleImpl/Library.hpp>
#include <splashouilleImpl/Solid.hpp>
#include <splashouilleImpl/Image.hpp>
#include <splashouilleImpl/Animation.hpp>
#include <splashouilleImpl/Sound.hpp>
#include <splashouilleImpl/Fashion.hpp>
#include <splashouilleImpl/Style.hpp>
#include <libconfig.h++>
#include <iostream>
#include <iomanip>

using namespace splashouilleImpl;

int Library::nbObjects = 0;

Library::Library():listener(0) {}

Library::~Library()
{
    listener = 0;
    for (std::map<std::string, splashouille::Object *>::iterator it = library.begin(); it != library.end(); it++)
    {
        deleteObject(it, false);
    }
    library.clear();
}

/**
 * Import and prepare the library
 * @param _library is the objects library
 * @return true if everything is fine
 */
bool Library::import(libconfig::Setting & _library)
{
    bool            ret = true;
    try { for (int i=0; i<_library.getLength(); i++) { createObject(_library[i]); } }
    catch(libconfig::SettingTypeException e) { ret = false; }

    if (Engine::debug)
    {
        std::cout<<std::setw(STD_LABEL)<<std::left<<"Library::import" <<" (size: "<<library.size()<<")"<<std::endl;
    }

    return ret;
}

/**
 * Create an object from a configuration setting
 * @param _setting is the configuration setting
 * @return a new allocated object
 */
splashouille::Object * Library::createObject(libconfig::Setting & _setting)
{
    std::string             type;       // Type of the object
    std::string             id;         // Object id
    splashouille::Object *  ret = 0;    // Object pointer (0 if failed)

    if (_setting.exists(DEFINITION_ID))  { _setting.lookupValue(DEFINITION_ID, id); }
    else                                 { char m[128]; snprintf(m, 128, "__object%05d", nbObjects++); id.assign(m); }

    if (id.size())
    {
        if (!(ret=getObjectById(id)))
        {
            _setting.lookupValue(TYPE, type);

            if (!type.compare(TYPE_SOLID))      { ret = new splashouilleImpl::Solid(id, _setting); }            else
            if (!type.compare(TYPE_IMAGE))      { ret = new splashouilleImpl::Image(id, _setting); }            else
            if (!type.compare(TYPE_ANIMATION))  { ret = new splashouilleImpl::Animation(id, _setting, this); }  else
            if (!type.compare(TYPE_SOUND))      { ret = new splashouilleImpl::Sound(id, _setting); }

            if (ret) { insertObject(id, ret); }
        }
    }

    if (Engine::debug)
    {
        std::cout<<std::setw(STD_LABEL)<<std::left<<"Library::createObject"
             <<" (type: "<<type<<") (id: "<<id<<") (return: "<<(ret?"OK":"KO")<<")"<<std::endl;
    }

    return ret;
}

/** Create empty object */
splashouille::Object * Library::createObject(const std::string & _id, const std::string & _type)
{
    splashouille::Object * ret = 0;
    if (!_type.compare(TYPE_SOLID))     { ret = createSolid(_id); }     else
    if (!_type.compare(TYPE_IMAGE))     { ret = createImage(_id); }     else
    if (!_type.compare(TYPE_ANIMATION)) { ret = createAnimation(_id); } else
    if (!_type.compare(TYPE_SOUND))     { ret = createSound(_id); }
    return ret;
}

splashouille::Solid * Library::createSolid(const std::string & _id)
{
    splashouille::Solid * ret = new splashouilleImpl::Solid(_id);
    if (ret) { insertObject(_id, ret); }

    if (Engine::debug)
    {
         std::cout<<std::setw(STD_LABEL)<<std::left<<"Library::createSolid"
             <<" (type: "<<TYPE_SOLID<<") (id: "<<_id<<") (return: "<<(ret?"OK":"KO")<<")"<<std::endl;
    }

    return ret;
}

splashouille::Image * Library::createImage(const std::string & _id)
{
    splashouille::Image * ret = new splashouilleImpl::Image(_id);
    if (ret) { insertObject(_id, ret); }


    if (Engine::debug)
    {
        std::cout<<std::setw(STD_LABEL)<<std::left<<"Library::createImage"
             <<" (type: "<<TYPE_IMAGE<<") (id: "<<_id<<") (return: "<<(ret?"OK":"KO")<<")"<<std::endl;
    }

    return ret;
}

splashouille::Animation * Library::createAnimation(const std::string & _id)
{
    splashouille::Animation * ret = new splashouilleImpl::Animation(_id, this);
    if (ret) { insertObject(_id, ret); }

    if (Engine::debug)
    {
        std::cout<<std::setw(STD_LABEL)<<std::left<<"Library::createAnimation"
             <<" (type: "<<TYPE_ANIMATION<<") (id: "<<_id<<") (return: "<<(ret?"OK":"KO")<<")"<<std::endl;
    }

    return ret;
}

splashouille::Sound * Library::createSound(const std::string & _id)
{
    splashouille::Sound * ret = new splashouilleImpl::Sound(_id);
    if (ret) { insertObject(_id, ret); }

    if (Engine::debug)
    {
        std::cout<<std::setw(STD_LABEL)<<std::left<<"Library::createSound"
             <<" (type: "<<TYPE_SOUND<<") (id: "<<_id<<") (return: "<<(ret?"OK":"KO")<<")"<<std::endl;
    }

    return ret;
}


/**
 * Copy an object from an existing object
 * @param _parent is the object id to copy
 * @param _id is the new object id
 * @return a new allocated object
 */
splashouille::Object *  Library::copyObject(const std::string & _parent, const std::string & _id)
{
    splashouille::Object * ret = 0;

    if (!(ret=getObjectById(_id)))
    {
        splashouille::Object * parent = getObjectById(_parent);
        if (parent)
        {
            if (! parent->getType().compare(TYPE_SOLID))
            {
                ret = new splashouilleImpl::Solid(_id, dynamic_cast<splashouilleImpl::Solid*>(parent));
            }
            else if (! parent->getType().compare(TYPE_IMAGE))
            {
                ret = new splashouilleImpl::Image(_id, dynamic_cast<splashouilleImpl::Image*>(parent));
            }
            else if (! parent->getType().compare(TYPE_ANIMATION))
            {
                ret = new splashouilleImpl::Animation(_id, dynamic_cast<splashouilleImpl::Animation*>(parent), this);
            }
            else if (! parent->getType().compare(TYPE_SOUND))
            {
                ret = new splashouilleImpl::Sound(_id, dynamic_cast<splashouilleImpl::Sound*>(parent));
            }

            if (ret) { insertObject(_id, ret); }
        }
    }

    if (ret && ret->getType().compare(getObjectById(_parent)->getType())) { ret = 0; }

    if (Engine::debug)
    {
        std::cout<<std::setw(STD_LABEL)<<std::left<<"Library::copyObject"
             <<" (parent: "<<_parent<<") (id: "<<_id<<") (return: "<<(ret?"OK":"KO")<<")"<<std::endl;
    }

    return ret;
}


/**
 * The internal delete object
 * @param _it is the internal map iterator
 * @param _erase the iterator from the map
 * @return true if success
 */
bool Library::deleteObject(const std::map<std::string, splashouille::Object *>::iterator & _it, bool _erase)
{
    bool ret = (_it!=library.end());
    if (ret)
    {
        splashouille::Object * obj = _it->second;

        // Listener onDelete callback
        if (listener) { listener->onDelete(obj); }

        if (!obj->isEngine())
        {
            if (obj->isSolid())     { delete(dynamic_cast<splashouilleImpl::Solid*>(obj)); }    else
            if (obj->isImage())     { delete(dynamic_cast<splashouilleImpl::Image*>(obj)); }    else
            if (obj->isAnimation()) { delete(dynamic_cast<splashouilleImpl::Animation*>(obj));} else
            if (obj->isSound())     { delete(dynamic_cast<splashouilleImpl::Sound*>(obj)); }    else
                                    { ret = false; }
        }
    }
    // Log the operation
    if (Engine::debug)
    {
        std::cout<<std::setw(STD_LABEL)<<std::left<<"Library::deleteObject"
             <<" (id: "<<_it->first<<") (return: "<<(ret?"OK":"KO")<<")"<<std::endl;
    }

    // If ok delete the entry in the library
    if (ret && _erase) { library.erase(_it); }

    return ret;
}

/**
 * Delete an Object
 * @param _id is the object key to delete
 * @return true if succeed
 */
bool Library::deleteObject(const std::string & _id)
{
    return deleteObject(library.find(_id));
}

/**
 * Delete an object
 * @param _obj is the object to delete
 * @return true if succeed
 */
bool Library::deleteObject(splashouille::Object * _obj)
{
    // Find the object in the library
    std::map<std::string, splashouille::Object *>::iterator it;
    for (it = library.begin(); it!=library.end(); it++)
    {
        if (it->second==_obj) break;
    }

    return deleteObject(it);
}

/**
 * Insert a object into the library
 * @param _key is the element key as string
 * @param _object is the object to insert
 * @return true if succeed
 */
bool Library::insertObject(const std::string & _key, splashouille::Object * _object)
{
    std::pair<std::map<std::string, splashouille::Object *>::iterator, bool> ret;
    ret = library.insert(std::pair<std::string, splashouille::Object *>(_key, _object));

    if (listener) { listener->onCreate(_object); }

    return ret.second;
}


splashouille::Solid * Library::getSolidById(const std::string & _id) const
{
    splashouille::Solid *       ret = 0;
    splashouille::Object *      obj = getObjectById(_id);
    if (obj && !obj->getType().compare(TYPE_SOLID)) { ret = dynamic_cast<splashouille::Solid*>(obj); }
    return ret;
}
splashouille::Image * Library::getImageById(const std::string & _id) const
{
    splashouille::Image *       ret = 0;
    splashouille::Object *      obj = getObjectById(_id);
    if (obj && !obj->getType().compare(TYPE_IMAGE)) { ret = dynamic_cast<splashouille::Image*>(obj); }
    return ret;
}
splashouille::Animation * Library::getAnimationById(const std::string & _id) const
{
    splashouille::Animation *   ret = 0;
    splashouille::Object *      obj = getObjectById(_id);
    if (obj && !obj->getType().compare(TYPE_ANIMATION)) { ret = dynamic_cast<splashouille::Animation*>(obj); }
    return ret;
}
splashouille::Sound * Library::getSoundById(const std::string & _id) const
{
    splashouille::Sound *       ret = 0;
    splashouille::Object *      obj = getObjectById(_id);
    if (obj && !obj->getType().compare(TYPE_SOUND)) { ret = dynamic_cast<splashouille::Sound*>(obj); }
    return ret;
}


/**
 * Log the library to the standard output
 */
void Library::log() const
{
    std::cout<<"+ Library (size: "<<library.size()<<")"<<std::endl;
    for (std::map<std::string, splashouille::Object *>::const_iterator it = library.begin(); it != library.end(); it++)
    {
        std::cout<<"    ["<<it->first<<"]"<<std::endl;
        it->second->log(1);
    }
}




