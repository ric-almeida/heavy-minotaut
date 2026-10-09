
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Implementation file for the computation of the lookahead upward simulation  *
 *  (strict or non-strict). For more detailed explanations please consult           *
 *  README.txt/Publications.                                                        *
 * 																					*
 ************************************************************************************/

#include "up_simulation.hh"

bool defend(const Automaton& aut, Step& step, const state q, vector<vector<pair<transition,size_t>> >& trans_botup,
            const vector<bool> isFinal, const vector<typerank> ranks,
            const vector<vector<bool> >& param_rel, const bool strict,
            const vector<vector<bool> >& W,
            Time& timeout_start, seconds timeout = 0)
{

    check_timeout(aut, timeout_start, timeout);

    state p = step.getState();

    if (p==q) return true;

    if (OPT[WEAK_ACCEPTANCE_COND]==OFF && isFinal[p] && !isFinal[q])
        return false;

    if (OPT[WEAK_ACCEPTANCE_COND]==ON && (step.isLastStep() || step.isFirstStep()) && isFinal[p] && !isFinal[q])
        return false;

    if (!step.isFirstStep() && W[p][q])
        return true;

    if(step.isLastStep())
        return W[p][q];

    Step parent = step.getParent();
    symbol s = parent.getSymbol();
    unsigned int index = step.getIndex();

    vector<pair<transition, size_t> > transitions = vectorVectorPairTransitionIntAt(trans_botup,q);

    unsigned int size = transitions.size();

    vector<state> children;
    pair<transition,unsigned int> pair_;
    for (unsigned int k=0; k<size; k++) {
        pair_ = transitions.at(k);
        if (pair_.second!=index || (pair_.first).GetSymbol() != s)
            continue;
        children = (pair_.first).GetChildren();
        children.erase(children.begin()+index);   // the siblings of q
        vector<state> children2 = mapGetState(parent.getChildren());
        children2.erase(children2.begin()+index); // the siblings of p
        if (!areInRelIter(children2, children, param_rel, strict))
            continue;
        if (defend(aut,parent,(pair_.first).GetParent(),trans_botup,isFinal,ranks,
                   param_rel,strict,W,timeout_start, timeout))
            return true;
    }

    return false;
}

bool attack(const Automaton& aut, Step* leafStep, Step& rootStep, const state q,
            const unsigned int depth, const unsigned int la,
            vector<vector<pair<transition,size_t>> >& trans_botup, const vector<bool> isFinal,
            const vector<typerank> ranks,
            const vector<vector<bool> >& param_rel, const bool strict, const vector<vector<bool> >& W,
            Time& timeout_start, seconds timeout = 0)
{

    check_timeout(aut, timeout_start, timeout);

    if (depth==la)
        return !defend(aut,*leafStep,q,trans_botup,isFinal,ranks,param_rel,strict,W,timeout_start, timeout);

    bool result_def;
    if (depth>0)
    {
        result_def = defend(aut,*leafStep,q,trans_botup,isFinal,ranks,param_rel,strict,W,timeout_start, timeout);
        if (result_def) return false;
    }

    vector<pair<transition,size_t>> trans_from_p = vectorVectorPairTransitionIntAt(trans_botup,rootStep.getState());
    if (trans_from_p.empty())
    {
        if (depth==0)       // in this case, leafStep and rootStep refer to the same.
        {
            if (isFinal[leafStep->getState()] && !isFinal[q])
                return true;
            else
                return false;
        }
        else
            return !result_def;
    }

    unsigned int size = trans_from_p.size();
    transition trans;
    for (unsigned int i=0; i<size; i++)
    {
        trans = trans_from_p.at(i).first;
        Step parent = Step(trans.GetParent(), trans.GetSymbol(), ranks[trans.GetSymbol()], trans.GetChildren(), false);
        rootStep.setParent(&parent);
        rootStep.setIndex(trans_from_p.at(i).second);
        parent.setChild(rootStep, trans_from_p.at(i).second);

        if (attack(aut, leafStep,parent,q,depth+1,la,trans_botup,isFinal,ranks,param_rel,strict,W, timeout_start, timeout))
            return true;
    }

    return false;
}

float refine(const Automaton& aut, const vector<bool> isFinal, const vector<typerank> ranks,
             vector<vector<pair<transition,size_t>> >& trans_botup,
             const unsigned int la, const vector<vector<bool> >& param_rel,
             const bool strict, vector<vector<bool> >& W, const unsigned int n,
             Time& timeout_start = Epoch, seconds timeout = 0)
{
    unsigned int counter = 0, visits = 0;

    try
    {
        while (true)
        {
            for (state p=0; p<n; p++)
                for (state q=0; q<n; q++)
                {
                    visits++;

                    if (!W[p][q] || q==p) {     // False remains False; and W always includes the id relation.
                        if (++counter == n*n) {
                            throw finished();
                        }
                        else
                            continue;
                    }

                    Step first = Step(p,true);
                    if (attack(aut, &first, first, q, 0, la, trans_botup, isFinal, ranks,
                               param_rel, strict, W, timeout_start, timeout)) {
                        W[p][q] = false;
                        counter = 0;
                    }
                    else
                        if (++counter == n*n) 	throw finished();
                }
        }
    }
    catch (finished& e)
    {
    }

    return (float) visits / ((float) n*n);
}


/* Returns a 3D vector post_len such that post_len[p][s][i] contains the number of transitions
   (bottom-up) by s which have p as the i-th child. */
vector<vector<vector<unsigned int> > > getPostLen(const vector<vector<pair<transition,size_t>> >& trans_botup,
                                                  const unsigned int numb_states, const unsigned int greatest_symbol,
                                                  const vector<typerank>& ranks)
{

    vector<unsigned int> vec_indexes(getGreatestRank(ranks),0);
    vector<vector<unsigned int> > vec_symbols(greatest_symbol+1, vec_indexes);
    vector<vector<vector<unsigned int> > > post_len(numb_states, vec_symbols);

    unsigned int size = trans_botup.size();
    for (unsigned int state=0; state<size; state++)
    {
        const vector<pair<transition,size_t> >& trans_to_state = trans_botup.at(state);
        // trans that have 'state' as one of the children
        for (const pair<transition,size_t> pair_ : trans_to_state)
        {
           // try {
                post_len.at(state).at(pair_.first.GetSymbol()).at(pair_.second)++;
           /* }
            catch (const std::out_of_range& oor) {
                std::cerr << "Out of Range error: " << oor.what()
                          << "when trying to increment the position [" << state
                          << "][" << pair_.first.GetSymbol() << "][" << pair_.second << "] of post_len.";
                exit_with_error("\n");
            }*/
        }
    }

    return post_len;
}

vector<vector<bool> >& pre_refine(vector<vector<bool> >& W, const Automaton& aut,
                                  const vector<vector<vector<unsigned int> > >& post_len,
                                  const unsigned int numb_states, const unsigned int greatest_symbol,
                                  const typerank greatest_rank,
                                  Time& timeout_start = Epoch, seconds timeout = 0)
{

    // might be easy to adapt to depths greater than 1
    for (unsigned int p=0; p<numb_states; p++)
        for (unsigned int q=0; q<numb_states && q!=p; q++)
        {
            check_timeout(aut, timeout_start, timeout);

            for (unsigned int s=0; s<greatest_symbol+1 && W.at(p).at(q); s++)
                for (unsigned int i=0; i<greatest_rank && W.at(p).at(q); i++)
                    if (post_len.at(p).at(s).at(i) && !post_len.at(q).at(s).at(i))
                        W.at(p).at(q) = false;
        }

    return W;

}


vector<vector<bool> > up_simulation(const AutData& autData,
                                    const unsigned int la,
                                    const vector<vector<bool> >& param_rel, const bool param_strict,
                                    const unsigned int greatest_state,
                                    const unsigned int greatest_symbol, float *refinements,
                                    Time& timeout_start, seconds timeout)
{

    /* Handling the optional arguments */
    unsigned int n = (greatest_state==0) ? getGreatestUsedState(autData)+1 : greatest_state+1;
    unsigned int greatest_symbol_ = (greatest_symbol==0) ? getGreatestUsedSymbol(autData) : greatest_symbol;
    if (refinements != NULL) *refinements = 0;

    const Automaton& aut = getAut(autData);
    const vector<typerank>& ranks = getRanks(autData);

    vector<vector<bool> > W = createBoolMatrix(n,n,true);
    initializeW(aut, W, n);

    if (n==0) return W;

    vector<bool> isFinal = getIsFinal(aut,n);

    auto start = std::chrono::high_resolution_clock::now();
    vector<vector<pair<transition,size_t>> > trans_botup = obtainTransBotUp(aut,n);
    auto elapsed = std::chrono::high_resolution_clock::now() - start;

    // Pre-refinement
    start = std::chrono::high_resolution_clock::now();
    vector<vector<vector<unsigned int> > > post_len = getPostLen(trans_botup, n, greatest_symbol_, ranks);
    W = pre_refine(W, aut, post_len, n, greatest_symbol_, getGreatestRank(ranks), timeout_start);
    elapsed = std::chrono::high_resolution_clock::now() - start;

    float refinements_ = refine(aut, isFinal, ranks, trans_botup, la, param_rel, param_strict,
                         W, n, timeout_start, timeout);
    if (refinements != NULL) *refinements = refinements_;

    return W;

}

vector<vector<bool> > up_simulation_strict(const AutData& autData, const unsigned int la,
                                           const vector<vector<bool> >& param_rel, const bool strict,
                                           const unsigned int greatest_state, const unsigned int numb_symbols,
                                           float* refinements,
                                           Time& timeout_start, seconds timeout)
{
    vector<vector<bool> > W = up_simulation(autData, la, param_rel, strict, greatest_state, numb_symbols, refinements, /*ranks,*/ timeout_start, timeout);

    //extractStrictRelation(W);

    return strictRel(W);
}

vector<vector<bool> > up_simulation_larger(const AutData& autData,
                                           const unsigned int la,
                                           const vector<vector<bool> >& param_rel, const bool param_strict,
                                           const unsigned int n,
                                           const unsigned int greatest_symbol, float *refinements,
                                           Time& timeout_start, seconds timeout)
{
    return invRel(up_simulation(autData,la,param_rel,param_strict,n,greatest_symbol,refinements,timeout_start,timeout));
}

vector<vector<bool> > up_simulation_of_id(const AutData& autData,
                                          const unsigned int la, const unsigned int greatest_state,
                                          const unsigned int greatest_symbol, float *refinements,
                                          Time& timeout_start, seconds timeout)
{
    return up_simulation(autData, la, generateIdRelation(autData,greatest_state), false, greatest_state, greatest_symbol, refinements, timeout_start, timeout);
}

vector<vector<bool> > up_simulation_of_id_strict(const AutData& autData,
                                                 const unsigned int la, const unsigned int greatest_state,
                                                 const unsigned int greatest_symbol, float *refinements,
                                                 Time& timeout_start, seconds timeout)
{
    vector<vector<bool> > W = up_simulation_of_id(autData,la,greatest_state,greatest_symbol,refinements,timeout_start,timeout);

    //extractStrictRelation(W);

    return strictRel(W);
}
