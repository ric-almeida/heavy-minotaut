
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Implementation file for the computation of the combined preorder, as        *
 *  defined in 'Lukas Holik et al. A Uniform (Bi-)Simulation-Based Framework for    *
 *  Reducing Tree Automata. ENTCS 2009'.                                            *
 * 																					*
 ************************************************************************************/

#include "combined_relation.hh"

vector<vector<bool> > combined_relation(const AutData& aut, const unsigned int la_dw, const unsigned int la_up,
                                        const unsigned int numb_states, const unsigned int greatest_symbol,
                                        Time& timeout_start, seconds timeout)
{

    vector<vector<bool> > W_dw, W_up, W_composed, W_combined;

    float numb_ref_dw;
    W_dw = dw_simulation(aut, la_dw, &numb_ref_dw, true, timeout_start, timeout);

    float numb_ref_up;
    W_up = up_simulation(aut, la_up, W_dw, false, numb_states, greatest_symbol, &numb_ref_up, timeout_start, timeout);

    W_composed = createBoolMatrix(numb_states,numb_states,false);

    /* Computing the composition of the two relations. */
    for (unsigned int i=0; i<numb_states; i++)
        for (unsigned int j=0; j<numb_states; j++)
        {
            if (!W_dw.at(i).at(j))
                continue;

            for (unsigned int k=0; k<numb_states; k++)
                if (W_up.at(k).at(j))
                    W_composed.at(i).at(k) = true;

        }

    W_combined = W_composed;

    /* Computing the combined relation. */
    for (unsigned int q=0; q<numb_states; q++)
        for (unsigned int r=0; r<numb_states; r++)
        {
            if (!W_composed.at(q).at(r))
                continue;

            for (unsigned int s=0; s<numb_states; s++)
                if (W_dw.at(r).at(s) && !W_composed.at(q).at(s))
                {
                    W_combined.at(q).at(r) = false;
                    continue;
                }
        }

    return W_combined;

}
