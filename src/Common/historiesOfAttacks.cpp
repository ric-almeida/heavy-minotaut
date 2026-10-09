
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Implementation file defining 3 different versions of an optimization based  *
 *  on caching good and bad attacks. The motivation is that Duplicator may skip     *
 *  trying to find a defence for a certain attack because it has already been found *
 *  to be good (or bad), and therefore Duplicator may immediately declare defeat    *
 *  (or victory) on this particular attack. For a more detailed explanation please  *
 *  check README.txt/Publications.                                                  *
 * 																					*
 ************************************************************************************/

#include "historiesOfAttacks.hh"

/* Functions for creating empy histories */

/* Could Global Histories be unified as well? */
globalHist newHistoryGlobalAttacks(unsigned int numb_states)
{
    globalHist history;

    if (areGlobalHistsBasedOnSets)
    {
        set<code> s;
        vector<set<code>> column(numb_states,s);
        vector<vector<set<code>>> hist(numb_states,column);
        history = hist;
    }
   /* Not currently being used.
    * else
    {
        codes_map map;
        vector<codes_map> column(numb_states,map);
        vector<vector<codes_map> > hist(numb_states,column);
        history = hist;
    }*/

    return history;
}

semiGlobalHist newHistorySemiGlobalAttacks(unsigned int numb_states)
{
    semiGlobalHist history;

    map<node_no,defence2> empty_map;
    vector<map<node_no,defence2>> vec(numb_states,empty_map);
    history = vec;

    return history;
}

vector<vector<bool> > newHistoryLocalAttacks(unsigned int numb_states, unsigned int numb_p_children)
{
    vector<bool> column(numb_p_children,false);

    vector<vector<bool> > history(numb_states,column);

    return history;
}


/* Functions for looking up attacks in the histories,
 * for several different type of history representations and scopes possible.
   The type of encodings of the attacks depend on the scope of the history. */

bool attackIsInGlobalHist(code attack, state p, vector<vector<codes_map>>& history, state q)
{
    return (history.at(q).at(p).find(attack)) != history.at(q).at(p).end();
}

bool attackIsInGlobalHist(code attack, state p, vector<vector<set<code>>>& history, state q)
{
    return (history.at(q).at(p).find(attack)) != history.at(q).at(p).end();
}

bool attackIsInSemiGlobalHist(node_no attack, vector<nodes_map>& history, state q)
{
    return (history.at(q).find(attack) != history.at(q).end());
}

map<node_no,defence2>::iterator findAttackInSemiGlobalHist(node_no attack, semiGlobalHist& history, state q)
{
    map<node_no,defence2>::iterator it = history.at(q).find(attack);
    return it;
}

bool attackIsInLocalHist(unsigned int attack_index, vector<vector<bool> >& history, state q_i)
{
    return history.at(q_i).at(attack_index);
}


/* Functions for inserting attacks in the histories,
 * for several different type of history representations and scopes possible.
   The type of encodings of the attacks depends on the scope of the history. */

void insertAttackInGlobalHist(code attack, state p, vector<vector<codes_map>>& history, state q)
{
    pair<code,defence> pair_ (attack,success);
    history.at(q).at(p).insert(pair_);
}

void insertAttackInGlobalHist(code attack, state p, vector<vector<set<code>>>& history, state q)
{
    history.at(q).at(p).insert(attack);
}

///* This Function is specific to the 3-Value-Logic strategy */
//void insertAttackInGlobalHist(code attack, state p, vector<vector<codes_map>>& history, state q,
//                              defence type_of_defence)
//{
//    pair<code<defence> pair_ (attack,type_of_defence);
//    history.at(q).insert(pair_);
//}

void insertAttackInSemiGlobalHist(node_no attack, vector<nodes_map>& history, state q)
{
    pair<node_no,defence> pair_ (attack,success);
    history.at(q).insert(pair_);
}

void insertGoodAttackInSemiGlobalHist(node_no attack, vector<map<node_no,defence2>>& history, state q)
{
    pair<node_no,defence2> pair_ (attack,true);
    history.at(q).insert(pair_);
}

void insertBadAttackInSemiGlobalHist(node_no attack, vector<map<node_no,defence2>>& history, state q)
{
    pair<node_no,defence2> pair_ (attack,false);
    history.at(q).insert(pair_);
}

///* This Function is specific to the 3-Value-Logic strategy */
//void insertAttackInSemiGlobalHist(node_no attack, vector<nodes_map> history, state q,
//                                  defence type_of_defence)
//{
//    pair<node_no,defence> pair_ (attack,type_of_defence);
//    history.at(q).insert(pair_);
//}

void insertAttackInLocalHist(unsigned int attack_index, vector<vector<bool> >& history, state q_i)
{
    history.at(q_i).at(attack_index) = true;
}
