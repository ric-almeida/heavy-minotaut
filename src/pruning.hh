
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Header file for the transitions pruning procedure.                          *
 * 																					*
 ************************************************************************************/

#ifndef PRUNING_H
#define PRUNING_H


#include "Common/automatonHelper.hh"
#include "Common/simulationHelper.hh"
#include "dw_simulation.hh"
#include "up_simulation.hh"

using std::vector;

AutData prune(const AutData& autData, const vector<vector<bool> >& rel1,
                const vector<vector<bool> >& rel2, const bool flag_strictness, set<transition> &pruned_trans);
AutData prune(const AutData& autData, const vector<vector<bool> >& rel1,
                const vector<vector<bool> >& rel2, const bool flag_strictness);

AutData prune_with_strict_up_la_sim_of_id_and_id(const AutData& autData, unsigned int la_up,
                                                 set<transition> &pruned_trans,
                                                 bool ru_in_the_end = true,
                                                 const vector<vector<bool> >& rel = {},
                                                 unsigned int greatest_state = 0,
                                                 unsigned int greatest_symbol = 0,
                                                 Time& timeout_start = Epoch, seconds timeout = 0);
AutData prune_with_strict_up_la_sim_of_id_and_id(const AutData& autData,
                                                 unsigned int la_up,
                                                 bool ru_in_the_end,
                                                 const vector<vector<bool> > &rel,
                                                 unsigned int greatest_state,
                                                 unsigned int greatest_symbol,
                                                 Time& timeout_start, seconds timeout);

AutData prune_with_strict_up_sim_of_id_and_dw_la_sim(const AutData& autData, unsigned int la_dw,
                                                     set<transition> &pruned_trans,
                                                     bool ru_in_the_end = true,
                                                     const vector<vector<bool> >& rel1 = {},
                                                     const vector<vector<bool> >& rel2 = {},
                                                     unsigned int greatest_state = 0,
                                                     unsigned int greatest_symbol = 0,
                                                     Time& timeout_start = Epoch, seconds timeout = 0);
AutData prune_with_strict_up_sim_of_id_and_dw_la_sim(const AutData& autData, unsigned int la_dw,
                                                     bool ru_in_the_end = true,
                                                     const vector<vector<bool> >& rel1 = {},
                                                     const vector<vector<bool> >& rel2 = {},
                                                     unsigned int greatest_state = 0,
                                                     unsigned int greatest_symbol = 0,
                                                     Time& timeout_start = Epoch, seconds timeout = 0);

AutData prune_with_up_la_sim_of_dw_sim_and_strict_dw_sim(const AutData& autData,
                                                         unsigned int la_up,
                                                         set<transition>& pruned_trans,
                                                         bool ru_in_the_end = true,
                                                         unsigned int greatest_state = 0,
                                                         unsigned int greatest_symbol = 0,
                                                         Time& timeout_start = Epoch, seconds timeout = 0);
AutData prune_with_up_la_sim_of_dw_sim_and_strict_dw_sim(const AutData& autData,
                                                         unsigned int la_up,
                                                         bool ru_in_the_end = true,
                                                         unsigned int greatest_state = 0,
                                                         unsigned int greatest_symbol = 0,
                                                         Time& timeout_start = Epoch, seconds timeout = 0);


#endif // PRUNING_HH
