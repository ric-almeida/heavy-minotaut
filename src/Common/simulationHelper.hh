
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Header file for a collection of utility functions which aid in the          *
 *  computation of a lookahead simulation, whether downwardly or upwardly.          *
 * 																					*
 ************************************************************************************/

#ifndef _SIMULATIONHELPER_HH_
#define _SIMULATIONHELPER_HH_


#include <boost/variant/variant.hpp>
#include <boost/variant/get.hpp>

#include "../Common/common.hh"
#include "../Common/exceptions.hh"
#include "../Common/executionOptions.hh"
#include "../step.hh"
#include "automatonHelper.hh"


class MaybeTransition
{
	public:
		MaybeTransition();
        MaybeTransition(const transition trans);
		bool isATransition();
		transition getTransition();
        typerank getRank(const vector<typerank> &ranks);
		
	private:
		transition trans;
		bool isATrans = false;
};

typedef unsigned int defence;

/* A variant type-definition for defence, and two alternative 'get' classes for this type.
 * Not currently being used. */
typedef boost::variant< bool, unsigned int > defence_;

class my_get : public boost::static_visitor<>
{
public:
    unsigned int operator()(unsigned int & i) const
    {
        return i;
    }
    bool operator()(bool & b) const
    {
        return b;
    }
};

class get_generic : public boost::static_visitor<>
{
public:
    template <typename T>

    T operator()(T & x) const
    {
        return x;
    }
};

const bool areMapsOrdered = true /*false*/;
typedef std::conditional<areMapsOrdered,
                         map<node_no,defence>,
                         unordered_map<node_no,defence>>::type nodes_map; /* Used in SG Hist */
typedef std::conditional<areMapsOrdered,
                         map<code,defence>,
                         unordered_map<code,defence>>::type codes_map; /* Used in Global Hist */


extern defence success, fail, strong_fail, weak_fail;
/* 2VL uses 'success' and 'fail'.
 * 3VL uses 'success', 'weak_fail' and 'strong_fail'. */

bool isSuccess(defence def);
bool isStrongFail(defence def);
bool isWeakFail(defence def);
bool isFail(defence def);
bool isBetter(defence def1, defence def2);
bool isWorse(defence def1, defence def2);

transitions mapGetTrans(const Automaton &, const vector<Step *> &);
vector<state> mapGetState(vector<Step> &);
void extendAttack(vector<vector<Step*> >& steps, const unsigned int depth, vector<MaybeTransition>& transitions, const vector<typerank>& ranks);

void printVectorVectorMaybeTransitions(const vector<vector<MaybeTransition> >&);
typerank getRank(const symbol);
void initializeW(const Automaton &aut, vector<vector<bool> >& W, const unsigned int n, const bool strict=false, const bool default_value=true);
vector<vector<bool> > invRel(const vector<vector<bool> >& rel);
vector<vector<bool> > transClosure(vector<vector<bool> >);
void asymTransClosure(vector<vector<bool> >&, const unsigned int);
vector<vector<bool> > strictRel(vector<vector<bool> > R);
bool areInRel(state, state, const vector<vector<bool> >&);
bool areInRelIter(const vector<state>&, const vector<state>&, const vector<vector<bool> >&, const bool);
vector<vector<bool> > generateIdRelation(const unsigned int greatest_state);
vector<vector<bool> > generateIdRelation(const AutData& autData, const unsigned int greatest_state = 0);
unsigned int getSizeOfRel(const vector<vector<bool> >&/*, const unsigned int*/);
vector<transition> moveInitialTransitionsToBeginning(vector<transition>& vec);
vector<transition> orderTransBySymbolsRankings(vector<transition>& vec, const vector<typerank>& ranks);
vector<pair<transition, size_t> > &vectorVectorPairTransitionIntAt(vector<vector<pair<transition, size_t> > > &, const state);
void convertBinaryRelToBoolMatrix(const Automaton &aut, const stateDiscontBinaryRelation &binRel, vector<vector<bool> > &W);
vector<state> findGreaterStates(const vector<vector<bool> >& rel, state p);
vector<vector<state> > findGreaterTupStates(const Automaton& aut, const vector<vector<bool> >& rel,
                                            vector<vector<state> >& greaterStatesCach,
                                            const vector<state>& tup,
                                            Time &timeout_start = Epoch, seconds timeout = 0);
string w2String(const Automaton &aut, const vector<vector<bool> >&, const stateDict* dict = NULL);
void printW(const Automaton &aut, const vector<vector<bool> >&, const stateDict* = NULL);
void printStateBinRelation(const stateDiscontBinaryRelation& binRel);
void printAttack(const Automaton& aut, Step& step);


#endif
