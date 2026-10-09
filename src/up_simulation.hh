
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Header file for the computation of the lookahead upward simulation (strict  *
 *  or non-strict). For more detailed explanations please consult                   *
 *  README.txt/Publications.                                                        *
 * 																					*
 ************************************************************************************/

#ifndef _UP_SIMULATION_HH_
#define _UP_SIMULATION_HH_


#include "Common/exceptions.hh"
#include "Common/automatonHelper.hh"
#include "Common/simulationHelper.hh"
#include "step.hh"

extern bool OPT_RESTART_REFINE_3;

vector<vector<bool> > up_simulation(const AutData &autData, const unsigned int la,
                                    const vector<vector<bool> >& param_rel, const bool strict,
                                    const unsigned int greatest_state = 0, const unsigned int greatest_symbol = 0,
                                    float* refinements = NULL,
                                    Time& timeout_start = Epoch, seconds timeout = 0);
vector<vector<bool> > up_simulation_strict(const AutData &autData, const unsigned int la,
                                           const vector<vector<bool> >& param_rel, const bool strict,
                                           const unsigned int greatest_state = 0, const unsigned int greatest_symbol = 0,
                                           float* refinements = NULL,
                                           Time& timeout_start = Epoch, seconds timeout = 0);
vector<vector<bool> > up_simulation_larger(const AutData &autData, const unsigned int la,
                                           const vector<vector<bool> >& param_rel, const bool strict,
                                           const unsigned int greatest_state = 0, const unsigned int greatest_symbol = 0,
                                           float* refinements = NULL,
                                           Time& timeout_start = Epoch, seconds timeout = 0);
vector<vector<bool> > up_simulation_of_id(const AutData& autData,
                                          const unsigned int la,
                                          const unsigned int greatest_state = 0, const unsigned int greatest_symbol = 0,
                                          float* refinements = NULL,
                                          Time& timeout_start = Epoch, seconds timeout = 0);
vector<vector<bool> > up_simulation_of_id_strict(const AutData& autData,
                                                 const unsigned int la,
                                                 const unsigned int greatest_state = 0, const unsigned int greatest_symbol = 0,
                                                 float* refinements = NULL,
                                                 Time& timeout_start = Epoch, seconds timeout = 0);

#endif
