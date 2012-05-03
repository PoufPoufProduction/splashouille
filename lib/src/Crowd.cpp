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
#include <splashouilleImpl/Animation.hpp>
#include <splashouilleImpl/Crowd.hpp>
#include <splashouilleImpl/Fashion.hpp>
#include <splashouilleImpl/Style.hpp>
#include <iostream>
#include <iomanip>
#include <limits>

#include <SDL.h>

#define CMP(a,b,o) ((o)?(a<=b):(b<=a))

using namespace splashouilleImpl;

int Crowd::garbageNumber = 0;

Crowd::Crowd(Animation * _animation): animation(_animation) { garbageNumber++; }
Crowd::~Crowd() { garbageNumber--; }

/**
 * Return a pointer to the tag-related list (create if doesn't exist)
 * @param _tag is the tagged list
 * @return the list
 */
std::list<Object*>* Crowd::getTagObjects(const std::string & _tag)
{
    std::list<Object *> * objects = 0;
    std::map<std::string, std::list<Object *>*>::iterator objIt = crowd.find(_tag);
    if (objIt==crowd.end())
    {
        // build the new tag list and insert it into the crowd
        objects = new std::list<splashouilleImpl::Object*>;
        crowd.insert(std::pair<std::string, std::list<Object *>*>(_tag, objects));
    }
    else
    {
        objects = objIt->second;
    }
    return objects;
}


/**
 * Insert an active object into the crowd
 * @param _timestamp is the insertion timestamp
 * @param _object is the object
 * @return true
 */
bool Crowd::insertObject(int _timestamp, splashouille::Object * _object)
{
    Object *                object      = dynamic_cast<Object*>(_object);

    // Look the object into the z-indexed list
    bool        rc = (object && (library.find(object->getId())==library.end()));
    if (rc)
    {
        if (object->inCrowd())
        {
            // Set the object initial timestamp in the crowd
            object->setInitialTimestamp(_timestamp);

            // Insert the object into the list sorted by z-index
            std::list<Object*> *            objects     = getTagObjects(_object->getTag());
            std::list<Object*>::iterator    position    = objects->begin();
            for (std::list<Object*>::iterator it=objects->begin(); it!=objects->end(); it++)
            {
                if ( (*it)->getZIndex() <= object->getZIndex() ) { position = it; position++; }
            }
            objects->insert(position, object);

            // Insert in the general library
            library.insert(std::pair<std::string, Object*>(object->getId(), object));
        }

        // Initialize the object (if has been already in a crowd)
        dynamic_cast<Fashion*>(object->getFashion())->clear();

        // Link the animation with its parent
        if (object->isAnimation()) { dynamic_cast<Animation*>(object)->setParent(animation); }

        // Call the callback listener
        if (object->getListener()) { object->getListener()->onShow(_timestamp, object); }
    }
    else
    {
        if (object) { object->setInitialTimestamp(_timestamp); }
    }

    if (object) { object->getStyle()->touch(); }

    if (Engine::debug)
    {
        std::cout<<std::setw(STD_LABEL)<<std::left<<"Crowd::insertObject"
             <<" (id:"<<(_object?_object->getId():"0")<<") (timestamp: "<<_timestamp<<") (return: "<<(rc?"OK":"KO")<<")"<<std::endl;
    }

    return rc;
}

/**
 * Get an object from its id
 * @param _id is the object to get
 * @return the object if found (0 else)
 */
splashouille::Object * Crowd::getObject(const std::string & _id)
{
    std::map<std::string, Object*>::iterator it = library.find(_id);
    return (it!=library.end())?it->second:0;
}

/**
 * Remove an object from the crowd (do not delete the instance)
 * @param _id is the object to delete
 * @return the instance pointer (0 if deleted failed)
 */
splashouille::Object * Crowd::dropObject(const std::string & _id)
{
    std::map<std::string, Object*>::iterator it  = library.find(_id);
    return dropObject(it);
}

/**
 * Remove an object from the crowd (do not delete the instance)
 * @param _obj is the object to delete as pointer
 * @return the instance pointer (0 if deleted failed)
 */
splashouille::Object * Crowd::dropObject(splashouille::Object * _obj)
{
    splashouille::Object * ret = 0;
    for (std::map<std::string, Object* >::iterator it=library.begin(); it!=library.end(); it++)
    {
        if (it->second == _obj) { ret = dropObject(it); break; }
    }
    return ret;
}


/**
 * Remove an object from the crowd (do not delete the instance)
 * @param _it is the map iterator
 * @return the instance pointer (0 if deleted failed)
 */
splashouille::Object * Crowd::dropObject(std::map<std::string, Object *>::iterator _it)
{
    Object * ret = 0;
    if (_it!=library.end())
    {
        Object *                        object      = _it->second;
        std::list<Object*> *            objects     = getTagObjects(object->getTag());
        std::list<Object*>::iterator    position    = objects->end();

        for (std::list<Object*>::iterator it=objects->begin(); it!=objects->end(); it++)
        {
            if (_it->second == *it) { position = it; }
        }

        if (position!=objects->end()) {
            // Remove the object from the crowd
            library.erase(_it);
            objects->erase(position);

            // Call the callback methods
            if (object->getListener()) { object->getListener()->onHide(object); }
            object->outCrowd();

            // Add an update area in the parent animation
            animation->addUpdateRect(object->getPosition());
        }

        if (Engine::debug)
        {
            std::cout<<std::setw(STD_LABEL)<<std::left<<"Crowd::dropObject"<<" (return: "<<(object?"OK":"KO")<<")"<<std::endl;
        }
    }
    return ret;
}

/**
 * Set the zIndex of an object
 * @param _obj is the object to change
 * @param _zIndex is the new zIndex
 * @return true if succeed
 */
bool Crowd::setZIndex(splashouille::Object * _obj, int _zIndex)
{
    bool ret = false;
    if (_obj)
    {
        Object *                        object      = dynamic_cast<Object*>(_obj);
        std::list<Object*> *            objects     = getTagObjects(_obj->getTag());
        std::list<Object*>::iterator    position    = objects->end();

        for (std::list<Object*>::iterator it=objects->begin(); it!=objects->end(); it++)
        {
            if (_obj == *it) { position = it; }
        }

        // If the object is find
        if (position!=objects->end()) {
            // Set the new zIndex of the object and remove it from the list
            ret = true;
            object->setZIndex(_zIndex);
            objects->erase(position);

            // Insert the object in the zIndex sorted list in its new place
            position = objects->begin();
            for (std::list<Object*>::iterator it=objects->begin(); it!=objects->end(); it++)
            {
                if ( (*it)->getZIndex() <= object->getZIndex() ) { position = it; position++; }
            }
            objects->insert(position, object);
        }
    }
    return ret;
}

/**
 * Set the zIndex of an object
 * @param _id is the object to change
 * @param _zIndex is the new zIndex
 * @return true if succeed
 */
bool Crowd::setZIndex(const std::string & _id, int _zIndex)
{
    return setZIndex(getObject(_id), _zIndex);
}

/**
 * Get the crowd size
 * @return the crowd size as int
 */
int Crowd::getSize()
{
    return library.size();
}

/**
 * Log the Crowd to the standard output
 * @param _rank is the log rank
 */
void Crowd::log(int _rank) const
{
    std::string offset; offset.append(4*_rank,' ');
    std::cout<<offset<<"+ Crowd (size map: "<<library.size()<<") (tags list: "<<crowd.size()<<")"<<std::endl;

    for (std::map<std::string, std::list<Object*>*>::const_iterator it=crowd.begin(); it!=crowd.end(); it++)
    {
        std::list<Object*> *    objects     = it->second;
        int                     index       = 0;
        std::cout<<offset<<"    + Tag: "<<it->first<<" (size: "<< objects->size()<<")"<<std::endl;

        for (std::list<Object*>::const_iterator it=objects->begin(); it!=objects->end(); it++)
        {
            Object *            object      = (*it);
            std::cout<<offset<<"    ["<<index<<"] (key: "<<object->getId()<<") (z-index: "<<object->getZIndex()<<")"<<std::endl;
            index++;
        }
    }
}

/**
 * Handle the mouseEvent
 * @param _x is the mouse position on x-axis
 * @param _y is the mouse position on y-axis
 * @param _state is the button action (if any)
 */
bool Crowd::mouseEvent(int _timestampInMilliSeconds, int _x, int _y , int _state)
{
    class MouseEvent : public Listener
    {
    private:
        int ts, x, y, state;
    public:
        MouseEvent(int _ts, int _x, int _y , int _state):ts(_ts),x(_x),y(_y),state(_state) {}
        bool onObject(splashouille::Object * _object)
        {
            bool ret = (_object==Engine::mouse);
            if (!ret)
            {
                if (_object->isAnimation())
                {
                    ret = dynamic_cast<Animation*>(_object)->mouseEvent(ts, x, y,state);
                }
                else
                {
                    ret = dynamic_cast<Object*>(_object)->mouseEvent(ts, x, y,state);
                }
            }
            return ret;
        }
    };
    MouseEvent mouseEventListener(_timestampInMilliSeconds, _x, _y, _state);
    forEach(&mouseEventListener, "", false);
    return true;
}

/**
 * Parse the crowd
 * @param _listener is the callback listener
 * @param _tag is the requested tag object (all objects if empty)
 * @param _ascendant is true for an z-index ascendant browsing (from farest to closest)
 */
void Crowd::forEach(Listener * _listener, const std::string & _tag, bool _ascendant) const
{
    bool rc = true;
    if (_tag.size())
    {
        // THE EASY CASE: PARSE ONE TAGGED LIST REGARDING THE ORDER (ASCENDANT OR DESCENDANT)
        std::map<std::string, std::list<Object*>*>::const_iterator it = crowd.find(_tag);
        if (it!=crowd.end())
        {
            std::list<Object*> * objects     = it->second;
            if (_ascendant)
            {
                for (std::list<Object*>::const_iterator it=objects->begin(); rc && it!=objects->end(); it++)
                {
                    rc = _listener->onObject(*it);
                }
            }
            else
            {
                for (std::list<Object*>::const_iterator it=objects->end(); rc && it!=objects->begin(); it--)
                {
                    std::list<Object*>::const_iterator prevIt = it;
                    prevIt--;
                    rc = _listener->onObject(*prevIt);
                }
            }
        }
    }
    else
    {
        // MERGE AND PARSE THE DIFFERENT TAGGED LISTS REGARDING THE Z-INDEX OF THEIR FIRST ELEMENT

        // FILL THE TAGS LIST BY ZINDEX ORDER
        std::list<objectParser>     tags;
        for (std::map<std::string, std::list<Object*>*>::const_iterator it=crowd.begin(); it!=crowd.end(); it++)
        {
            // SKIP THE EMPTY LIST
            if (it->second->begin()!=it->second->end())
            {
                // GET THE FIRST OBJECT OF THE TAGGED LIST
                objectIterator tag = it->second->begin();
                if (!_ascendant) { tag = it->second->end(); tag--; }

                // GET THE LAST OBJECT OF THE TAGGED LIST
                objectIterator lastTag = it->second->begin();
                if (_ascendant) { lastTag = it->second->end(); lastTag--; }

                // PLACE THE FIRST OBJECT IN THE PARSING LIST REGARDING ITS Z-INDEX
                std::list<objectParser>::iterator   position    = tags.begin();
                for (std::list<objectParser>::iterator itTag = tags.begin(); itTag!=tags.end(); itTag++)
                {
                    if ( CMP( (*((*itTag).first))->getZIndex(),(*tag)->getZIndex(), _ascendant) ) { position = itTag; position++; }
                }
                tags.insert(position, objectParser(tag, lastTag));
            }
        }

        // PARSE THE TAGS LIST
        while (rc && tags.size())
        {
            // THE CURRENT Z-INDEX IS (*((*tags.begin()).first))->getZIndex(), GET THE NEXT ONE FROM ANOTHER TAG LIST
            std::list<objectParser>::iterator   position    = tags.begin();
            int nextZIndex = _ascendant?std::numeric_limits<int>::max():std::numeric_limits<int>::min();
            if (tags.size()>1)
            {
                position++;
                nextZIndex = (*((*position).first))->getZIndex();
            }

            // TREAT EVERY OBJECT OF THE CURRENT TAG LIST UNTIL IT REACHES THE NEXT Z-INDEX
            bool tagNotEmpty = true;
            while ( rc && tagNotEmpty && CMP((*((*tags.begin()).first))->getZIndex(),nextZIndex,_ascendant) )
            {
                if ( (rc = _listener->onObject(*((*tags.begin()).first))))
                {
                    if ((*tags.begin()).first==(*tags.begin()).second)  { tagNotEmpty = false; }
                    else {
                        if (_ascendant) { (*tags.begin()).first++; } else { (*tags.begin()).first--; }
                    }
                }
            }

            // IF THE CURRENT TAG LIST IS NOT ENDED, INSERT IT IN THE PARSING LIST REGARDING THE CURRENT Z_INDEX
            if (rc)
            {
                if (tagNotEmpty)
                {
                    for (std::list<objectParser>::iterator itTag = tags.begin(); itTag!=tags.end(); itTag++)
                    {
                        if ( CMP((*((*itTag).first))->getZIndex(),nextZIndex,_ascendant) ) { position = itTag; position++; }
                    }
                    tags.insert(position, *tags.begin());
                }
                tags.erase(tags.begin());
            }
        };
    }
}

/**
 * Update the current crowd
 * @param _timestamp is the current timestamp
 * @return number of pixels
 */
void Crowd::update( int _timestamp)
{
    for (std::map<std::string, Object *>::iterator it = library.begin(); it!=library.end(); it++)
    {
        if ((it->second)->update(_timestamp)) { animation->addUpdateRect((it->second)->getUpdateRect()); }
    }
}

/**
 * Render the current crowd
 * @param _surface is the surface to render
 * @param _offset is the parent offset
 */
void Crowd::render(SDL_Surface * _surface, SDL_Rect * _offset)
{
    class Render : public Listener
    {
        private :
            SDL_Surface *   surface;
            SDL_Rect *      offset;
        public:
        Render(SDL_Surface * _surface, SDL_Rect * _offset):surface(_surface), offset(_offset) {}
        bool onObject(splashouille::Object * _object) { dynamic_cast<Object*>(_object)->render(surface, offset); return true; }
    };
    Render  renderListener(_surface, _offset);
    forEach(&renderListener);
}


/**
 * Clear the crowd
 */
void Crowd::clear(const std::string & _tag)
{
    if (_tag.size())
    {
        // REMOVE JUST ONE TAG
        std::map<std::string, std::list<Object*>*>::iterator itTag = crowd.find(_tag);
        if (itTag!=crowd.end())
        {
            std::list<Object*> *                        objects     = itTag->second;
            std::map<std::string, Object *>::iterator   itMap;
            for (std::list<Object*>::iterator it=objects->begin(); it!=objects->end(); it++)
            {
                // UPDATE THE DROPED OBJECT
                if ((*it)->getListener()) { (*it)->getListener()->onHide(*it); }
                (*it)->outCrowd();
                animation->addUpdateRect((*it)->getPosition());

                // REMOVE FROM THE LOCAL LIBRARY
                itMap = library.find((*it)->getId());
                if (itMap!=library.end())
                {
                    library.erase(itMap);
                }
                else
                {
                    std::cout<<std::setw(STD_LABEL)<<std::left<<"Crowd::clear"<<" Error on library"<<std::endl;
                }

                // REMOVE FROM THE Z-INDEXED LIST
                it = objects->erase(it);
            }
            // REMOVE THE TAG LIST
            crowd.erase(itTag);
            delete objects;
        }
    }
    else
    {
        // REMOVE EVERYTHING: THE QUICK WAY - DELETE OBJECTS FROM THE LIBRARY
        for (std::map<std::string, Object *>::iterator it = library.begin(); it!=library.end(); it++)
        {
            // UPDATE THE DROPED OBJECT
            if ((it->second)->getListener()) { (it->second)->getListener()->onHide((it->second)); }
            (it->second)->outCrowd();
            animation->addUpdateRect((it->second)->getPosition());
        }
        library.clear();

        // REMOVE EVERY TAGGED Z-INDEXED LISTS
        for (std::map<std::string, std::list<Object*>*>::iterator it = crowd.begin(); it!=crowd.end(); it++)
        {
            delete it->second;
        }
        crowd.clear();
    }

    if (Engine::debug)
    {
        std::cout<<std::setw(STD_LABEL)<<std::left<<"Crowd::clear"<<" (animation: "<<animation->getId()<<
            ") (tag: "<<_tag<<")"<<std::endl;
    }

}

/**
 * Forward the callback
 * @return true
 */
bool Crowd::outCrowd()
{
    for (std::map<std::string, Object *>::iterator it = library.begin(); it!=library.end(); it++)
    {
        (it->second)->outCrowd();
    }

    return true;
}

