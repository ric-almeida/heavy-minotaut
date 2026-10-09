
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Implementation file for the transitions pruning procedure.                  *
 * 																					*
 ************************************************************************************/

#include "pruning.hh"

/* Departing states are compared using relation rel1 and children states are compared
 * using rel2 (which may be strict or not, depending on the flag given). */
AutData prune(const AutData& autData, const vector<vector<bool> >& rel1,
                const vector<vector<bool> >& rel2, const bool flag_strictness, set<transition> &pruned_trans)
{

    AutData autData_p = copyAutWithoutTrans(autData);
    Automaton aut_p = getAut(autData_p), aut = getAut(autData);

    state parent1;
    symbol symb;
    vector<state> children1, children2;

    bool bad;
    for (const lv_transition& trans1 : aut)
    {
        bad = false;
        parent1 = trans1.GetParent();
        symb = trans1.GetSymbol();
        children1 = trans1.GetChildren();

        for (const lv_transition& trans2 : aut)
            if (symb == trans2.GetSymbol())
            {
                if (areInRel(parent1,trans2.GetParent(),rel1))
                {
                    children2 = trans2.GetChildren();
                    if (areInRelIter(children1, children2, rel2, flag_strictness))
                    {
                        /* Then trans2 is "better" than trans1, and so the latter will not
                        *  be in the pruned automaton. */
                        bad = true;
                        pruned_trans.insert(trans1);
                        break;
                    }
                }
            }
        if (!bad)
            aut_p.AddTransition(children1,symb,parent1);
    }

    return wrapAutData(aut_p,getRanks(autData));

}

AutData prune(const AutData& autData, const vector<vector<bool> >& rel1,
                const vector<vector<bool> >& rel2, const bool flag_strictness)
{
    set<transition> empty;
    return prune(autData,rel1,rel2,flag_strictness,empty);
}

AutData prune_with_strict_up_la_sim_of_id_and_id(const AutData& autData,
                                                 unsigned int la_up,
                                                 set<transition>& pruned_trans,
                                                 bool ru_in_the_end,
                                                 const vector<vector<bool> > &rel,
                                                 unsigned int greatest_state,
                                                 unsigned int greatest_symbol,
                                                 Time& timeout_start, seconds timeout)
{
    vector<vector<bool> > rel_ = (rel.empty()) ?
            up_simulation_of_id_strict(autData,la_up,greatest_state,greatest_symbol,NULL,timeout_start,timeout)
              : rel;

    AutData autData_f = prune(autData,rel_,generateIdRelation(autData,greatest_state),false,pruned_trans);

    if (langIsEmpty(autData_f))
        outputText("");

    if (ru_in_the_end)
        autData_f = removeUselessStates(autData_f);

    if (langIsEmpty(autData_f))
        outputText("");

    return autData_f;
}

AutData prune_with_strict_up_la_sim_of_id_and_id(const AutData& autData,
                                                 unsigned int la_up,
                                                 bool ru_in_the_end,
                                                 const vector<vector<bool> > &rel,
                                                 unsigned int greatest_state,
                                                 unsigned int greatest_symbol,
                                                 Time& timeout_start, seconds timeout)
{
    set<transition> empty;
    return prune_with_strict_up_la_sim_of_id_and_id(autData,la_up,empty,ru_in_the_end,rel,
                                                    greatest_state,greatest_symbol,timeout_start,timeout);
}

AutData prune_with_strict_up_sim_of_id_and_dw_la_sim(const AutData& autData,
                                                     unsigned int la_dw,
                                                     set<transition>& pruned_trans,
                                                     bool ru_in_the_end,
                                                     const vector<vector<bool> > &rel1, const vector<vector<bool> > &rel2,
                                                     unsigned int greatest_state,
                                                     unsigned int greatest_symbol,
                                                     Time& timeout_start, seconds timeout)
{
    vector<vector<bool> > rel1_ = (rel1.empty()) ?
            up_simulation_of_id_strict(autData,1,greatest_state,greatest_symbol,NULL,timeout_start,timeout)
              : rel1;
    vector<vector<bool> > rel2_ = (rel2.empty()) ?
            dw_simulation(autData,la_dw,NULL,true,timeout_start,timeout) : rel2;

    AutData autData_f = prune(autData,rel1_,rel2_,false,pruned_trans);

    if (ru_in_the_end)
        autData_f = removeUselessStates(autData_f);

    return autData_f;
}

AutData prune_with_strict_up_sim_of_id_and_dw_la_sim(const AutData& autData,
                                                     unsigned int la_dw,
                                                     bool ru_in_the_end,
                                                     const vector<vector<bool> > &rel1, const vector<vector<bool> > &rel2,
                                                     unsigned int greatest_state,
                                                     unsigned int greatest_symbol,
                                                     Time& timeout_start, seconds timeout)
{
    set<transition> empty;
    return prune_with_strict_up_sim_of_id_and_dw_la_sim(autData,la_dw,empty,ru_in_the_end,rel1,rel2,
                                                        greatest_state,greatest_symbol,timeout_start,timeout);
}

AutData prune_with_up_la_sim_of_dw_sim_and_strict_dw_sim(const AutData& autData,
                                                         unsigned int la_up,
                                                         set<transition>& pruned_trans,
                                                         bool ru_in_the_end,
                                                         unsigned int greatest_state,
                                                         unsigned int greatest_symbol,
                                                         Time& timeout_start, seconds timeout)
{
    vector<vector<bool> > rel2 = dw_simulation(autData,1,NULL,true,timeout_start,timeout);

    vector<vector<bool> > rel1 = up_simulation(autData,la_up,rel2,false,greatest_state,greatest_symbol,
                                               NULL,timeout_start,timeout);

    AutData autData_f = prune(autData,rel1,rel2,true,pruned_trans);

    if (ru_in_the_end)
        autData_f = removeUselessStates(autData_f);

    return autData_f;
}

AutData prune_with_up_la_sim_of_dw_sim_and_strict_dw_sim(const AutData& autData,
                                                         unsigned int la_up, bool ru_in_the_end,
                                                         unsigned int greatest_state,
                                                         unsigned int greatest_symbol,
                                                         Time& timeout_start, seconds timeout)
{
    set<transition> empty;
    return prune_with_up_la_sim_of_dw_sim_and_strict_dw_sim(autData,la_up,empty,ru_in_the_end,
                                                            greatest_state,greatest_symbol,timeout_start,timeout);
}
