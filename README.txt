SPLASHOUILLE README
===================

Splashouille is a dynamic animations engine. This framework allows users to easily define their own animations thanks to configuration files and to handle them dynamicly by using API.

The code is C++


COPYRIGHT AND LICENSE
=====================

All of Splashouille's source code is released under the GNU GPL version 3. Later versions are permitted.


LINKS
=====

Homepage  http://poufpoufproduction.fr
Source    https://github.com/PoufPoufProduction/splashouille
Email     johannc@poufpoufproduction.fr


DEPENDENCIES
============

To build splashouille you need the 1.2 Development Libraries for SDL, SDL_image and SDL_mixer
You also need the libconfig version 9.0


BUILDING SPLASHOUILLE
=====================

To build splashouille, go to the lib directory and run the following commands:

	make

If you want the framework installed system-wide, as root, install with:

	make install

Splashouille will be installed into '/usr/local'


COMPILING WITH SPLASHOUILLE
===========================

Compile with /usr/local/include, and link with the options -L/usr/local/lib and -lsplashouille.