
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Header file declaring auxiliary functions that call libvata's states        *
 *  quotienting procedure.                                                          *
 * 																					*
 ************************************************************************************/

#ifndef QUOTIENTING_HH
#define QUOTIENTING_HH


#include "Common/automatonHelper.hh"
#include "Common/simulationHelper.hh"
#include "dw_simulation.hh"
#include "up_simulation.hh"
#include "combined_relation.hh"

using std::vector;

using statesMap = VATA::ExplicitTreeAut::StateToStateMap;

statesMap convertPreorderToBinEquiv(const Automaton &aut, const vector<vector<bool> >& P);
AutData quotientAutomaton(const AutData &old_autData, const vector<vector<bool> >& W);

AutData quotient_with_combined_relation(const AutData &autData,
                                          unsigned int la_dw, unsigned int la_up,
                                          unsigned int numb_states = 0,
                                          unsigned int greatest_symbol = 0,
                                          Time& timeout_start = Epoch, seconds timeout = 0);
AutData quotient_with_dw_la_sim(const AutData &autData,
                    unsigned int la_dw,
                    unsigned int greatest_state  = 0,
                    unsigned int greatest_symbol = 0,
                    string log_humanread_filename = "");
AutData quotient_with_up_la_of_id_sim(const AutData& autData, unsigned int la,
                                      const vector<vector<bool> >& up_la_of_id = {},
                                      unsigned int greatest_state = 0, unsigned int greatest_symbol = 0,
                                      Time& timeout_start = Epoch, seconds timeout = 0);

#endif // QUOTIENTING_HH
