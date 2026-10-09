
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Implementation file for the transitions saturation procedure.               *
 * 																					*
 ************************************************************************************/

#include "saturation.hh"

/* Tuples of states are represented by vector<state>. */

/* Definition of a map of tuples of states to vectors of tuples of states. */
using mymap = map<vector<state>,vector<vector<state>>>;

/* Saturates the transitions of the automaton using S(rel,rel^{-1}). */
AutData saturate(const AutData& autData, const vector<vector<bool> >& rel,
                 set<transition>& new_trans, unsigned int greatest_state, Time &timeout_start, seconds timeout)
{
    Automaton aut = getAut(autData), aut_f = aut;
    unsigned int n = (greatest_state==0) ? getGreatestUsedState(aut)+1 : greatest_state+1;
    const vector<typerank>& ranks_ = /*ranks.empty() ?*/ getRanks(autData) /*: ranks*/;

    /* Definition of a vector of tuples of states.
     * A tuple of states tup at a position i will read:
     * "the state i is smaller w.r.t. R_2 than each state in tup." */
    vector<vector<state> > greaterStatesCach(n);

    /* Definition of a vector of maps: one map for each different rank value in the alphabet. */
    vector<mymap> maps(getGreatestRank(ranks_));

    for (unsigned int p=0; p<rel.size(); p++)
    {
        for (unsigned int q=0; q<rel.size(); q++)
        {
            if (rel[p][q])
            {
                for (const transition& trans2 : aut[q])
                {
                    check_timeout(aut, timeout_start, timeout);

                    symbol sym = trans2.GetSymbol();
                    const vector<state> tuple = trans2.GetChildren();
                    vector<vector<state>> greaterTuples;

                    auto find = maps.at(ranks_[sym]-1).find(tuple);
                    if (find == maps.at(ranks_[sym]-1).end())
                    {
                        greaterTuples = findGreaterTupStates(aut, rel, greaterStatesCach, tuple, timeout_start, timeout);
                        pair<vector<state>,vector<vector<state>>> pair_ (tuple,greaterTuples);
                        maps.at(ranks_[sym]-1).insert(pair_);
                    }
                    else
                    {
                        greaterTuples = find->second;
                    }
                    for (vector<state>& tup : greaterTuples)
                    {
                        if (!aut_f.ContainsTransition(tup,sym,p))
                        {
                            transition* new_t = new transition(p,sym,tup);
                            aut_f.AddTransition(*new_t);
                            new_trans.insert(*new_t);
                        }
                    }
                }
            }
        }
    }

    return wrapAutData(aut_f,ranks_);
}

AutData saturate(const AutData& autData, const vector<vector<bool> >& rel,
                 unsigned int greatest_state, Time& timeout_start, seconds timeout)
{
    set<transition> empty;
    return saturate(autData,rel,empty,greatest_state,timeout_start,timeout);
}

/* Saturates the transitions of the automaton using S(la_dw_sim_larger,la_dw_sim). */
AutData saturate_with_la_dw_sim(const AutData& autData, unsigned int la, set<transition>& new_trans,
                                unsigned int greatest_state, Time& timeout_start, seconds timeout)
{
    const vector<vector<bool> > la_dw_sim_larger = dw_simulation_larger(autData, la, NULL, true, timeout_start, timeout);

    return saturate(autData, la_dw_sim_larger, new_trans, greatest_state, timeout_start, timeout);
}

AutData saturate_with_la_dw_sim(const AutData& autData, unsigned int la,
                                unsigned int greatest_state, Time& timeout_start, seconds timeout)
{
    set<transition> empty;
    return saturate_with_la_dw_sim(autData,la,empty,greatest_state,timeout_start,timeout);
}

/*
 @
*/
AutData saturate_with_la_up_sim_of_id(const AutData& autData, unsigned int la, set<transition>& new_trans,
                                      unsigned int greatest_state, symbol greatest_symbol,
                                      Time& timeout_start, seconds timeout)
{
    const vector<vector<bool> > la_up_sim_of_id = up_simulation_of_id(autData, la, greatest_state, greatest_symbol, NULL, timeout_start, timeout);

    return saturate(autData, la_up_sim_of_id, new_trans, greatest_state);
}

AutData saturate_with_la_up_sim_of_id(const AutData& autData, unsigned int la,
                                      unsigned int greatest_state, symbol greatest_symbol,
                                      Time& timeout_start, seconds timeout)
{
    set<transition> empty;
    return saturate_with_la_up_sim_of_id(autData,la,empty,greatest_state,greatest_symbol,timeout_start,timeout);
}
