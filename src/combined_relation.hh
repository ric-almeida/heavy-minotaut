
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Header file for the computation of the combined preorder, as                *
 *  defined in 'Lukas Holik et al. A Uniform (Bi-)Simulation-Based Framework for    *
 *  Reducing Tree Automata. ENTCS 2009'.                                            *
 * 																					*
 ************************************************************************************/


#ifndef COMBINED_PREORDER_H
#define COMBINED_PREORDER_H

#include "Common/exceptions.hh"
#include "Common/automatonHelper.hh"
#include "Common/simulationHelper.hh"
#include "dw_simulation.hh"
#include "up_simulation.hh"

vector<vector<bool> > combined_relation(const AutData &autData, /*vector<vector<bool> >& W_combined,*/
                      const unsigned int la_dw, const unsigned int la_up,
                      const unsigned int numb_states, const unsigned int greatest_symbol,
                      Time& timeout_start = Epoch, seconds timeout = 0);


#endif // COMBINED_PREORDER_H
