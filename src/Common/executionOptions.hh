
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Header file containing declarations of global variables for some of the     *
 *  execution options in this project. These include a flag indicating whether      *
 *  3 Value Logic will be used or not, among others.                                *
 * 																					*
 ************************************************************************************/

#ifndef EXECUTIONOPTIONS_HH
#define EXECUTIONOPTIONS_HH

#include "common.hh"

// Aliases
typedef unsigned int opt;

// Opts
extern const unsigned int ORDER_OF_ATTACKS_TRANS;
extern const unsigned int NO_PARTICULAR_ORDER,
                          SEMI_ORDERED_BY_INITIAL_TRANS,
                          SEMI_ORDERED_BY_SYMBOLS_RANKING;

extern const unsigned int TYPE_OF_HISTORY_OF_GOOD_ATKS;
extern const unsigned int NONE,
                          LOCAL,
                          SEMI_GLOBAL,
                          GLOBAL,
                          GLOBAL_V2;
extern const unsigned int TYPE_OF_HISTORY_OF_BAD_ATKS;

extern const unsigned int THREE_VALUES_LOGIC;
extern const unsigned int OFF,
                          ON,
                          ON_V1,
                          ON_V2;

extern const unsigned int WEAK_ACCEPTANCE_COND;

extern vector<unsigned int> OPT;

extern const int AUTO_PR;
extern int pr_depth;

// Functions
void startOPTDefaults();
void setOPT(unsigned int option, unsigned int value);
string getOPT(unsigned int code);


#endif // EXECUTIONOPTIONS_HH
