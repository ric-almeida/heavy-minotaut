
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Header file containing definitions of all the exceptions used in this       *
 *  project.                                                                        *
 * 																					*
 ************************************************************************************/

#ifndef _EXCEPTIONS_HH_
#define _EXCEPTIONS_HH_

#include <exception>
//#include "common.hh"
#include "automatonHelper.hh"

using namespace std;

//using Automaton  =  VATA::ExplicitTreeAut;

struct autHasNoLeafRules : public exception
{
};

struct wHasChanged : public exception
{  
};

struct finished : public exception
{
};

/*struct timeout_ : public exception
{
};*/

struct timeout_ : public exception
{
    Automaton aut;

    // Delete me later
    //timeout_() { }

    timeout_(Automaton aut)
    {
        this->aut = aut;
    }

    Automaton getAut()
    {
        return this->aut;
    }
};

void check_timeout(const Automaton& aut, timespec timeout_start, unsigned int timeout = 0);
void check_timeout(const Automaton& aut, Time& timeout_start = Epoch, seconds timeout = 0);

#endif
