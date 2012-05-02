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

#ifndef SPLASHOUILLE_DEFINES_HPP_
#define SPLASHOUILLE_DEFINES_HPP_

#ifdef __GNUC__
#define UNUSED __attribute__((unused))
#else
#define UNUSED
#endif

#define ROOT                        "root"

#define TYPE                        "type"
#define TYPE_SOLID                  "solid"
#define TYPE_IMAGE                  "image"
#define TYPE_ANIMATION              "animation"
#define TYPE_SOUND                  "sound"

#define MOUSE                       "mouse"
#define MOUSE_TOPLEFT               "topleft"
#define MOUSE_TOP                   "top"
#define MOUSE_TOPRIGHT              "topright"
#define MOUSE_LEFT                  "left"
#define MOUSE_CENTER                "center"
#define MOUSE_RIGHT                 "right"
#define MOUSE_BOTTOMLEFT            "bottomleft"
#define MOUSE_BOTTOM                "bottom"
#define MOUSE_BOTTOMRIGHT           "bottomright"

#define STYLE                       "style"
#define STYLE_LEFT                  "left"
#define STYLE_TOP                   "top"
#define STYLE_RELATIVE_LEFT         "relative-left"
#define STYLE_RELATIVE_TOP          "relative-top"
#define STYLE_WIDTH                 "width"
#define STYLE_HEIGHT                "height"
#define STYLE_POSITION              "position"
#define STYLE_POSITION_X            "position-x"
#define STYLE_POSITION_Y            "position-y"
#define STYLE_BACKGROUNDCOLOR       "background-color"
#define STYLE_DISPLAY               "display"
#define STYLE_OPACITY               "opacity"

#define DEFINITION                  "definition"
#define DEFINITION_ZINDEX           "z-index"
#define DEFINITION_TAG              "tag"
#define DEFINITION_FILENAME         "filename"
#define DEFINITION_TILESET          "tileset"
#define DEFINITION_TILESET_INDEX    "id"
#define DEFINITION_TILESET_POSITION "position"
#define DEFINITION_TILESET_ANIM     "animation"
#define DEFINITION_TILESET_DELAY    "delayInMilliSeconds"
#define DEFINITION_TILE             "tile"
#define DEFINITION_ALPHA            "alpha-color"
#define DEFINITION_ID               "id"
#define DEFINITION_STATIC           "static"
#define DEFINITION_DISPLAY          "display"
#define DEFINITION_DISPLAY_CROP     "crop"
#define DEFINITION_DISPLAY_SCALE    "scale"
#define DEFINITION_WIDTH            "width"
#define DEFINITION_HEIGHT           "height"
#define DEFINITION_STATE            "state"
#define DEFINITION_CHUNK            "chunk"

#define FASHION                     "fashion"
#define FASHIONS                    "fashions"
#define FASHION_ID                  "id"
#define FASHION_TIMESTAMP           "timeStampInMilliSeconds"
#define FASHION_TIMESTAMP_SHORT     "ts"
#define FASHION_SPEEDIN             "speedIn"
#define FASHION_SPEEDOUT            "speedOut"
#define FASHION_PERIOD              "period"

#define TIMELINE                    "timeline"
#define TIMELINES                   "timelines"
#define TIMELINE_ID                 "id"
#define EVENT_TIMESTAMP             "timeStampInMilliSeconds"
#define EVENT_TIMESTAMP_SHORT       "ts"
#define EVENT_PARENT                "parent"
#define EVENT_ID                    "eventId"
#define EVENT_OBJECTID              "id"
#define EVENT_VALUE                 "value"
#define EVENT_OPTION                "option"
#define EVENT_TYPE                  "event"
#define EVENT_TYPE_CREATE           "create"
#define EVENT_TYPE_COPY             "copy"
#define EVENT_TYPE_CLOSE            "close"
#define EVENT_TYPE_GOTO             "goto"
#define EVENT_TYPE_PAUSE            "pause"
#define EVENT_TYPE_TIMELINE         "timeline"
#define EVENT_TYPE_CLEAR            "clear"
#define EVENT_TYPE_FASHION          "fashion"
#define EVENT_TYPE_STATE            "state"


#define STD_LABEL                   24

#define RED_MASK                    0xFF000000
#define GREEN_MASK                  0x00FF0000
#define BLUE_MASK                   0x0000FF00
#define ALPHA_MASK                  0x000000FF




#endif 
