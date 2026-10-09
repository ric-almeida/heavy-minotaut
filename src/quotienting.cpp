
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Implementation file with auxiliary functions that call libvata's states     *
 *  quotienting procedure.                                                          *
 * 																					*
 ************************************************************************************/

#include "quotienting.hh"

/* Converts a states preorder (represented as a boolean matrix) into an equivalence binary relation
 * with libvata's type;                      */
statesMap convertPreorderToBinEquiv(const Automaton& aut, const vector<vector<bool> >& P)
{
    unsigned int n = P.size();
    statesMap map;
    state initialState = getInitialState(aut);

    for (unsigned int i = 0; i < n && i != initialState; i++) {
        for (unsigned int j = 0; j <= i; j++) {
            bool result1 = false, result2 = false;
            try
            {
                result1 = P.at(i).at(j);
            }
            catch (std::out_of_range& e)
            {
                std::cout << "out_of_range error when trying to access position (" << i << "," << j << ") of P";
                exit_with_error("");
            }
            try
            {
                result2 = P.at(j).at(i);
            }
            catch (std::out_of_range& e)
            {
                std::cout << "out_of_range error when trying to access position (" << j << "," << i << ") of P";
                exit_with_error("");
            }

            if (result1 && result2)
            {
                map.insert(std::make_pair(i, j));
            }
        }
    }

    return map;
}

/* Calls convertPreorderToBinEquiv to obtain a states map from the given preorder,
 * and then calls libvata's function for quotienting automata with that map. */
AutData quotientAutomaton(const AutData& old_autData, const vector<vector<bool> >& W)
{

    if (W.size()==0) return old_autData;

    Automaton old_aut = getAut(old_autData);
    statesMap map = convertPreorderToBinEquiv(old_aut,W);

    Automaton aut = removeInitialState(old_aut);
    aut = aut.CollapseStates(map);
    aut = addInitialState(aut);

    AutData new_autData = wrapAutData(aut,getRanks(old_autData));

    return new_autData;

}

AutData quotient_with_up_la_of_id_sim(const AutData& autData, unsigned int la,
                                      const vector<vector<bool> > &up_la_of_id,
                                      unsigned int greatest_state, unsigned int greatest_symbol,
                                      Time& timeout_start, seconds timeout)
{
    vector<vector<bool> > up_la_of_id_ = (up_la_of_id.empty()) ?
                up_simulation_of_id(autData,la,greatest_state,greatest_symbol,NULL,timeout_start,timeout) : up_la_of_id;

    return quotientAutomaton(autData, up_la_of_id_);
}


AutData quotient_with_dw_la_sim(const AutData& autData,
                                unsigned int la_dw,
                                unsigned int greatest_state,
                                unsigned int greatest_symbol,
                                string log_humanread_filename)
{

    const Automaton aut_i = getAut(autData);
    greatest_state  = !greatest_state  ? getGreatestUsedState(aut_i)  : greatest_state;
    greatest_symbol = !greatest_symbol ? getGreatestUsedSymbol(aut_i) : greatest_symbol;

    auto start   = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::high_resolution_clock::now() - start;   // Just setting the auto type.

    //AutData aut = removeUselessStates(autData);

    /* Computing the dw-la sim. */
    outputText("Computing the dw-" + std::to_string(la_dw) + " simulation... \n", log_humanread_filename);
    vector<vector<bool> > dw_la;
    start = std::chrono::high_resolution_clock::now();
    dw_la = dw_simulation(autData, la_dw);
    elapsed = std::chrono::high_resolution_clock::now() - start;

    /* Quotient with dw-la sim. */
    start = std::chrono::high_resolution_clock::now();
    AutData aut_quot = quotientAutomaton(autData,transClosure(dw_la));
    elapsed = std::chrono::high_resolution_clock::now() - start;

    return aut_quot;
}

AutData quotient_with_combined_relation(const AutData& autData, unsigned int la_dw, unsigned int la_up,
                                          unsigned int numb_states, unsigned int greatest_symbol,
                                          Time& timeout_start, seconds timeout)
{

    const Automaton aut = getAut(autData);

    numb_states     = !numb_states  ? getGreatestUsedState(aut)+1   : numb_states;
    greatest_symbol = !greatest_symbol ? getGreatestUsedSymbol(aut) : greatest_symbol;

    vector<vector<bool> > W_combined =
            combined_relation(autData, la_dw, la_up, numb_states, greatest_symbol, timeout_start, timeout);

    return quotientAutomaton(autData, W_combined);

}
