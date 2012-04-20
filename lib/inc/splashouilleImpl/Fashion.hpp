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

#ifndef SPLASHOUILLEIMPL_FASHION_HPP_
#define SPLASHOUILLEIMPL_FASHION_HPP_

#include <splashouille/Fashion.hpp>
#include <list>

namespace splashouille
{
class Style;
}

namespace splashouilleImpl
{

/**
 * The class which makes the style update.
 */
class Fashion : public splashouille::Fashion
{
private:

    /**
     * The class transition is used for moving style. It transforms an initial style into a new style.
     */
    class Transition
    {
    private:
        double                      a, b, c;                        // The ratio function f(t)=at3+bt2+ct
    public:
        /**
         * Transition constructor
         * @param _timestampIn is the beginning timestamp of the animation
         * @param _timestampOut is the ending timestamp of the animation
         * @param _speedIn is the speed at the begin of the animation
         * @param _speedOut is the speed at the end of the animation
         * @param _period is the period of the transition
         */
        Transition(int _timestampIn, int _timestampOut, double _speedIn, double _speedOut, int _period);

        /**
         * Another constructor
         */
        Transition(Transition * _transition);

        /**
         * Transition destructor
         * WARNING: Delete the style provided in constructor parameter
         */
        ~Transition();

        /**
         * Compute the ratio of the next style regarding the timestamp
         * @param _timestamp is the current timestamp
         */
        float ratio(int _timestamp);

        /** Attributes */
        splashouille::Style *       style;                          // The next style
        int                         timestampIn;                    // The timestamp in
        int                         timestampOut;                   // The timestamp out
        double                      speedIn;                        // The speed in
        double                      speedOut;                       // The speed out
        int                         period;                         // The transition period
        int                         nbTimes;                        // The number of period revolutions
        bool                        done;                           // The transition is done
    };

private:
    splashouille::Style *       last;                               // The last current style
    splashouille::Style *       current;                            // The current style
    splashouille::Style *       style;                              // The initial style
    std::list<Transition*>      transitions;                        // A list of transition
    int                         timestamp;                          // The last timestamp when current has been computed

    /**
     * Remove the old transitions (regarding the current timestamp) by updating the initial style
     * @param _timestamp is the current timestamp
     * @param _active is set to true if a transaction is currently active
     */
    void removeOldTransitions(int _timestamp, bool & _active);

    /**
     * Add an animation to the current style
     * @param _transition is the transition to add
     * @return the style of the object at the end of the animation
     */
    splashouille::Style * addTransition(Transition * _transition);

public:
    Fashion(splashouille::Fashion * _fashion);
    Fashion();

    /**
     * Fashion destructor: delete the local copy of the initial style
     */
    ~Fashion();

    /**
     * Import a fashion
     * @param _fashion is the parent fashion
     */
    void clone(splashouille::Fashion * _fashion);

    /**
     * Import a fashion
     * @param _setting is the fashion configuration (as list of transitions)
     */
    void import(libconfig::Setting & _setting);

    /**
     * Import a transition from the configuration file
     * @param _setting is the transition setting
     * @return the new allocated style of the object at the end of the animation
     */
    splashouille::Style * importTransition(libconfig::Setting & _setting);

    /**
     * Add an animation to the current style
     * @param _timestampIn is the beginning timestamp of the animation
     * @param _timestampOut is the ending timestamp of the animation
     * @param _speedIn is the speed at the begin of the animation
     * @param _speedOut is the speed at the end of the animation
     * @param _period is the period of the transition
     * @return the style of the object at the end of the animation
     */
    splashouille::Style * addTransition(int _timestampIn, int _timestampOut, double _speedIn = 1, double _speedOut = 1, int _period=0);

    /**
     * Get the number of active transitions
     * @return the number of active transitions as int
     */
    int getNumberTransitions() const { return transitions.size(); }

    /**
     * Compute and get the current style computed from the initial style and the transitions list
     * @param _timestamp is the current timestamp
     * @return the current style
     */
    splashouille::Style * getStyle(int timestamp);

    /**
     * Get the current style
     * @return the current style but const (because automaticaly generated)
     */
    const splashouille::Style * getCurrent() { return current; }

    /**
     * Get the initial style
     * @return the initial style
     */
    splashouille::Style * getStyle() { return style; }

    /**
     * Get the list of transitions
     * @return the list of transition as cont
     */
    const std::list<Transition*> &  getTransitions() { return transitions; }

    /**
     * Clear the transition regarding the new timestamp
     * @param _style is the new initial style
     */
    void clear(splashouille::Style * _style = 0);

    /**
     * Log a fashion to the standard output
     * @param _rank is the log rank
     */
    void log(int _rank = 0) const;

};

}


#endif

