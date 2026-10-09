
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Header file for the transitions saturation procedure.                       *
 * 																					*
 ************************************************************************************/

#ifndef SATURATION_HH
#define SATURATION_HH


#include "Common/automatonHelper.hh"
#include "dw_simulation.hh"
#include "up_simulation.hh"

AutData saturate(const AutData &autData,
                  const vector<vector<bool> >& rel, set<transition>& new_trans,
                  unsigned int greatest_state = 0, Time &timeout_start = Epoch, seconds timeout = 0);
AutData saturate(const AutData &autData,
                  const vector<vector<bool> >& rel,
                  unsigned int greatest_state = 0, Time &timeout_start = Epoch, seconds timeout = 0);

AutData saturate_with_la_dw_sim(const AutData& autData, unsigned int la, set<transition>& new_trans,
                                unsigned int greatest_state = 0,
                                Time& timeout_start = Epoch, seconds timeout = 0);
AutData saturate_with_la_dw_sim(const AutData& autData, unsigned int la,
                                unsigned int greatest_state = 0,
                                Time& timeout_start = Epoch, seconds timeout = 0);

AutData saturate_with_la_up_sim_of_id(const AutData& autData, unsigned int la, set<transition>& new_trans,
                                      unsigned int greatest_state = 0, symbol greatest_symbol = 0,
                                      Time& timeout_start = Epoch, seconds timeout = 0);
AutData saturate_with_la_up_sim_of_id(const AutData& autData, unsigned int la,
                                      unsigned int greatest_state = 0, symbol greatest_symbol = 0,
                                      Time& timeout_start = Epoch, seconds timeout = 0);

#endif // SATURATION_HH
