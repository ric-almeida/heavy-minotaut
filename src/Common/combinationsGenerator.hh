
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Header file of a class for generating all possible combinations of          *
 *  transitions from a set of states. It works as an auxiliary class for the        *
 *  computation of the extension of an attack during a simulation computation.      *
 *  Currently not being needed.                                                     *
 * 																					*
 ************************************************************************************/

#ifndef _COMBINATIONSGENERATOR_HH_
#define _COMBINATIONSGENERATOR_HH_


#include "automatonHelper.hh"
#include "simulationHelper.hh"
#include "common.hh"
#include "debugging.hh"
#include "common.hh"


extern vector<vector<vector<MaybeTransition> > > allCombs;

class Tree
{
    public:
        int getNode();
        Tree& getChild(const unsigned int);
        vector<Tree>& getChildren();
        unsigned int numbChildren();
        bool isRoot();
        bool isALeaf();
        void setChildren(int node, int num_children);
        void setChild(unsigned int pos, Tree child);
        string toString();
        Tree();
        Tree(int node);
        Tree(int node, int num_children);
        ~Tree();
        const static int NONE = -1;     /* The type decl. of NONE allows its use outside the class. */
        const static int ROOT_NODE = -2;

    private:
        int node = NONE;
        vector<Tree> children;
};

Tree& vectorTreesAt(vector<Tree>& vec, const unsigned int pos, const char* strct_name);
vector<vector<MaybeTransition> > genAllTransCombinations(const vector<vector<transition> >&);
vector<vector<MaybeTransition> > genAllTransCombinations_sortedByRanks(const vector<vector<transition> >& all_trans, const vector<typerank>& ranks);
vector<vector<MaybeTransition> > genAllTransCombinations_sortedByNumbFinalStates(const vector<vector<transition> >& all_trans, const vector<bool>& isFinal);

#endif

