
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Header file for the computation of the lookahead downward simulation        *
 *  (strict or non-strict). Also provided is an alternative version                 *
 *  using 3-Value Logic instead of 2-Value Logic. This is not the default. For more *
 *  detailed explanations please consult README.txt/Publications.                   *
 * 																					*
 ************************************************************************************/

#ifndef _DW_SIMULATION_HH_
#define _DW_SIMULATION_HH_

#include "Common/exceptions.hh"
#include "Common/automatonHelper.hh"
#include "Common/simulationHelper.hh"
#include "step.hh"
#include "Common/statisticalResults.hh"
#include "Common/executionOptions.hh"
#include "Common/historiesOfAttacks.hh"

extern bool OPT_RESTART_REFINE_3;
extern string filename_Gl;

bool attack_loop(const Automaton& aut, const unsigned int numb_states,
                 const state initialState,
                 const vector<typerank>& ranks,
                 const vector<vector<transition> >& all_trans, const unsigned int index,
                 vector<MaybeTransition>& combination, vector<vector<Step*> > steps,
                 const state q, const unsigned int depth, const unsigned int la,
                 const vector<vector<bool> >& W,
                 globalHist&  historyOfBadAtks_global,
                 globalHist&  historyOfGoodAtks_global,
                 float& codeGenerationTime, float& historiesTimeConsump,
                 Time& timeout_start, seconds timeout = 0);

bool attack_loop_with_3VL(const Automaton& aut, const unsigned int numb_states, const state initialState, const vector<bool>& isFinal,
                 const vector<typerank>& ranks, const unsigned int la,
                 vector<vector<bool> >& W, bool &hasWChanged, const vector<vector<transition> >& all_trans,
                 const unsigned int index,
                 vector<MaybeTransition>& combination, vector<vector<Step*> > steps, const state q,
                 const unsigned int depth, vector<vector<codes_map > >& historyOfBadAtks,
                 vector<vector<codes_map >>& historyOfGoodAtks,
                 float& codeGenerationTime, float& historiesTimeConsump);

bool attack_loop_pr(const Automaton& aut, const unsigned int numb_states,
                 const state initialState,
                 const vector<typerank>& ranks, const unsigned int la,
                 const vector<vector<transition> >& all_trans, const unsigned int index,
                 vector<MaybeTransition>& combination, vector<vector<Step*> > steps, const state q, const unsigned int depth,
                 Time& timeout_start, seconds timeout = 0);

vector<vector<bool> > dw_simulation(const AutData &autData, const unsigned int la,
                                    float *refinements = NULL, bool use_lvata = true,
                                    Time& timeout_start = Epoch, seconds timeout = 0);
vector<vector<bool> > dw_simulation_strict(const AutData& autData, const unsigned int la,
                                           float* refinements = NULL, bool use_lvata = true);
vector<vector<bool> > dw_simulation_larger(const AutData &autData, const unsigned int la,
                                    float *refinements = NULL, bool use_lvata = true,
                                    Time& timeout_start = Epoch, seconds timeout = 0);

void pre_refine_lin(vector<vector<bool> >& W, const Automaton& aut,
                    const unsigned int depth,
                    const unsigned int numb_states,
                    const unsigned int numb_symbols,
                    const vector<typerank>& ranks,
                    const state initialState);
void pre_refine_branch(vector<vector<bool> >& W, const Automaton& aut,
                        const unsigned int depth,
                        const unsigned int numb_states,
                        const vector<typerank>& ranks,
                        const state initialState,
                        Time& timeout_start = Epoch, seconds timeout = 0);

#endif
