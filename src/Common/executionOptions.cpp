
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Implementation file defining global variables for some of the               *
 *  execution options in this project. These include a flag indicating whether      *
 *  3 Value Logic should be used or not, among others.                              *
 * 																					*
 ************************************************************************************/

#include "executionOptions.hh"

const unsigned int ORDER_OF_ATTACKS_TRANS = 0,
    NO_PARTICULAR_ORDER   = 0,
    SEMI_ORDERED_BY_INITIAL_TRANS   = 1,
    SEMI_ORDERED_BY_SYMBOLS_RANKING = 2;

const unsigned int TYPE_OF_HISTORY_OF_GOOD_ATKS = 1;
const unsigned int NONE        = 0,
                   LOCAL       = 1,
                   SEMI_GLOBAL = 2,
                   GLOBAL      = 3,
                   GLOBAL_V2   = 4;
const unsigned int TYPE_OF_HISTORY_OF_BAD_ATKS = 2;

const unsigned int THREE_VALUES_LOGIC = 3;
const unsigned int OFF   = 0,
                   ON    = 1,
                   ON_V1 = 2,
                   ON_V2 = 3;

const unsigned int WEAK_ACCEPTANCE_COND = 4;

const unsigned int number_of_opts       = 5;

const int AUTO_PR = -1;

void startOPTDefaults() {
    OPT.resize(number_of_opts);

    OPT[ORDER_OF_ATTACKS_TRANS]       = /*SEMI_ORDERED_BY_SYMBOLS_RANKING;*/ NO_PARTICULAR_ORDER; /*SEMI_ORDERED_BY_INITIAL_TRANS;*/
    OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] = /*GLOBAL_V2;*/ /*GLOBAL;*/ /*SEMI_GLOBAL;*/ /*LOCAL;*/ NONE;
    OPT[TYPE_OF_HISTORY_OF_BAD_ATKS]  = /*GLOBAL_V2;*/ /*GLOBAL;*/ /*SEMI_GLOBAL;*/ /*LOCAL;*/ NONE;
    OPT[THREE_VALUES_LOGIC]           = OFF /*ON_V1*/ /*ON_V2*/;
    OPT[WEAK_ACCEPTANCE_COND]         = /*ON*/  OFF;

    pr_depth = AUTO_PR;
}

void setOPT(unsigned int option, unsigned int value)
{
    OPT.at(option) = value;
}

string getOPT(unsigned int code)
{
    if (code == TYPE_OF_HISTORY_OF_GOOD_ATKS)
    {
        if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == NONE) return "none";
        if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == GLOBAL) return "global";
        if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == SEMI_GLOBAL) return "semi-global";
        if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == LOCAL) return "local";
    }
    else
    if (code == TYPE_OF_HISTORY_OF_BAD_ATKS)
    {
        if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == NONE) return "none";
        if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == GLOBAL) return "global";
        if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == SEMI_GLOBAL) return "semi-global";
        if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == LOCAL) return "local";
    }
    else
        exit_with_error("getOPT(code) is only defined for history of attacks.");

    return "";
}

/* Possible future implementation.
 * vector<vector<opt> > genAllOptCombinations()
{
    vector<unsigned int> numbTrans = mapGetNumbElements(all_trans);

    numbTrans = {3, 5, 5, 2};

    vector<vector<int> > indexCombs = generateCombinations(numbTrans);

    return translateToTransitions(all_trans,indexCombs);
}*/
