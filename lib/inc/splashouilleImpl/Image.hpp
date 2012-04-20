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

#ifndef SPLASHOUILLEIMPL_IMAGE_HPP_
#define SPLASHOUILLEIMPL_IMAGE_HPP_

#include <splashouille/Defines.hpp>
#include <splashouille/Image.hpp>
#include <splashouilleImpl/Object.hpp>

#include <string>
#include <map>

class SDL_Surface;
class SDL_Rect;


namespace splashouille
{
class Style;
}

namespace splashouilleImpl
{
class Library;

const static int TileSetSizeMax = 1024;

class Image : virtual public splashouille::Image, virtual public splashouilleImpl::Object
{
private:
    /**
     * The image surface garbage collector class
     * If nbUsages is null, the Surface is removed from the surfaces map
     * Warning: that point is the reason why it is mandatory to keep a not used reference image
     * into the library in order to not reload the image file.
     */
    class Surface
    {
    public:
        SDL_Surface *                       surface;                // A reference surface read from file
        int                                 nbUsages;               // The number of usage of the current surface
        Surface(SDL_Surface * _surface): surface(_surface), nbUsages(1) { }
    };

    /**
     * The tileset garbage collector class
     * If nbUsages is null, the Tileset is removed from the tilesets list
     */
    class Tileset
    {
    public:
        /**
         * A tile is composed by a 3x2 coordinates system and a delay/next pair for animation
         * With a null delayInMilliseconds the animation is stopped and the tile remains static
         */
        class Tile
        {
         public:
            int                             position[2];            // The tile position in the reference image
            int                             size[2];                // The tile size
            int                             offset[2];              // An offset if the tile larger than the grid cell
            int                             delayInMilliseconds;    // The delay before the next animation frame
            Tile *                          next;                   // The next animation frame
            Tile(): delayInMilliseconds(0) { position[0] = position[1] = size[0] = size[1] = offset[0] = offset[1] = 0; next = 0; }
            Tile(libconfig::Setting & _setting);
        };
    public:
        Tile *                              tiles[TileSetSizeMax];  // A tileset definition
        int                                 nbUsages;               // The number of usage of the current tileset
        Tileset():nbUsages(1) { memset(tiles, 0, TileSetSizeMax*sizeof(Tile*)); }
        ~Tileset();
    };

    static std::list<Tileset*>              tilesets;               // All the tilesets
    static std::map<std::string, Surface*>  surfaces;               // All the image bitmap are stored here

private:
    std::string                             filename;               // The image file name
    int                                     alphaColor[3];          // The alpha color
    bool                                    alpha;                  // True if alphaColor is used
    Display                                 display;                // The display mode
    SDL_Surface *                           original;               // A copy pointer to the original surface (from surfaces)
    Tileset *                               tileset;                // A coordonate tileset
    int                                     tileIndex;              // The tile index

    Image(const std::string & _id, libconfig::Setting & _setting);
    Image(const std::string & _id, Image * _image);
    Image(const std::string & _id);
    ~Image();

public:

    /** Accessors */
    bool                                    isImage() const                             { return true; }
    const std::string &                     getFilename() const                         { return filename; }
    SDL_Surface *                           getSurface()                                { return surface; }
    Display                                 getDisplay() const                          { return display; }
    Tileset *                               getTileset() const                          { return tileset; }
    int                                     getWidth() const;
    int                                     getHeight() const;
    bool                                    getAlphaColor(int & _r, int & _g, int & _b) const;

    bool                                    setFilename(const std::string & _filename);
    void                                    setAlphaColor(int _r=255, int _g=0, int _b=255);
    void                                    setSize(int _width, int _height);
    void                                    setDisplay(Display _display)                { display = _display; }
    void                                    setTileIndex(int _tileIndex);

    /**
     * Render the object into the surface canvas
     * @param _surface is the surface to fill
     * @param _offset is the parent offset
     * @return true
     */
    bool render(SDL_Surface * _surface, SDL_Rect * _offset = 0);

    /**
     * Log the image to the standard output
     * @param _rank is the log rank
     */
    void log(int _rank = 0) const;

    friend class Library;

};

}


#endif

 
 
 
 
