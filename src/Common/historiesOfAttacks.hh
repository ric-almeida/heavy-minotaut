
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Header file defining 3 different versions of an optimization based          *
 *  on caching good and bad attacks. The motivation is that Duplicator may skip     *
 *  trying to find a defence for a certain attack because it has already been found *
 *  to be good (or bad), and therefore Duplicator may immediately declare defeat    *
 *  (or victory) on this particular attack. For a more detailed explanation please  *
 *  check README.txt/Publications.                                                  *
 * 																					*
 ************************************************************************************/

#ifndef HISTORIESOFATTACKS_HH
#define HISTORIESOFATTACKS_HH


#include "common.hh"
#include "simulationHelper.hh"

//const bool areSemiGlobalHistsUnified = true;
const bool areGlobalHistsBasedOnSets = true; // the alternative is maps

typedef bool defence2;
typedef vector<map<node_no,defence2>> semiGlobalHist;

typedef std::conditional<areGlobalHistsBasedOnSets,
                         set<code>,
                         codes_map>::type str;
typedef vector<vector<str>> globalHist;

globalHist newHistoryGlobalAttacks(unsigned int numb_states);
semiGlobalHist newHistorySemiGlobalAttacks(unsigned int numb_states);
vector<vector<bool> > newHistoryLocalAttacks(unsigned int numb_states, unsigned int numb_p_children);

bool attackIsInGlobalHist(code attack, state p, vector<vector<codes_map>>& history, state q);
bool attackIsInSemiGlobalHist(node_no attack, vector<nodes_map>& history, state q);
map<node_no,defence2>::iterator findAttackInSemiGlobalHist(node_no attack, semiGlobalHist& history, state q);
bool attackIsInGlobalHist(code attack, state p, vector<vector<set<code>>> & history, state q);
bool attackIsInLocalHist(unsigned int attack_index, vector<vector<bool> >& history, state q_i);

void insertAttackInGlobalHist(code attack, state p, vector<vector<codes_map>>& history, state q);
void insertAttackInGlobalHist(code attack, state p, vector<vector<set<code>>>& history, state q);
void insertAttackInSemiGlobalHist(node_no attack, vector<nodes_map>& history, state q);
void insertGoodAttackInSemiGlobalHist(node_no attack, vector<map<node_no,defence2>>& history, state q);
void insertBadAttackInSemiGlobalHist(node_no attack, vector<map<node_no,defence2>>& history, state q);
void insertAttackInLocalHist(unsigned int attack_index, vector<vector<bool> >& history, state q_i);


#endif // HISTORIESOFATTACKS_HH
