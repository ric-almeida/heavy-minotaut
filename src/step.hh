
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Header file for the class Step. The construction of a lookahead simulation	*
 *  on the states of a Bottom-Up Tree Automaton (BUTA) is based on verifying if 	*
 *  there is some attack from a state p against which a state q cannot defend. An 	*
 *  attack is a tree of transition rules with root at p.							*
 * 	Each transition rule (<p1,p2,...,pn>,s,p) is here represented as 				*
 * 	a Step(p,s,[Step c1,Step c2,...,Step cn]), where each Step ci represents the	*
 * 	transition from pi considered in the attack. A leaf-rule (<>,s,p) 				*
 * 	(i.e., a rule with no departing states) is represented as Step(p,s,[stepPsi]),	*
 *  where stepPsi is the (unique) initial state of a BUTA made explicit.            *
 * 																					*
 ************************************************************************************/

#ifndef _STEP_HH_
#define _STEP_HH_

#include "Common/debugging.hh"
#include "Common/automatonHelper.hh"

using namespace std;
using std::vector;
typedef string code;
typedef string node_no;

extern code CODE_DEL, CODE_DEL_POW;

const static string code_delimiter;
const static code EMPTY_CODE = "";
// 'static' so that the several imports of this header by other files does not originate the multiple definitions of the variable.

class Step
{
    public:
        const static int NONE = -1;
        node_no getNode();          // get the node number
        state getState() const;
        symbol getSymbol() const;
        vector<Step> getNext() const;
        vector<Step> &getChildren();
        Step* getChildAddr(int pos);
        Step &getParent();
        int getIndex();
        code getCode();
        code getEmptyCode();
        code getHeadlessCode();
        code getSetCode();
        bool isFirstStep();
        bool isLastStep();
        bool isALeafStep();						/* An attack is a tree and its nodes are Steps, */
        void setNode(node_no node);
        void setSymbol(symbol s, typerank r);	/* therefore every attack has possibly many leaf-steps. */
        void setChild(state p, int pos);
        void setChild(Step &child, int pos);
        void setChildren(const vector<state> &vec);
        void setParent(Step*);
        void setIndex(size_t index);
        void set(symbol s, typerank r, const vector<state> &vec);
        void eraseCode();
        unsigned int numbChildren();
        string toString();
        Step();
        Step(state p);
        Step(state p, node_no node);
        Step(state p, bool firstStep);
        Step(state p, symbol s, typerank r, bool firstStep);
        Step(state, symbol, typerank, const vector<state>&, bool);
        ~Step();

    private:
        node_no EMPTY_NODE = "";
        node_no node = EMPTY_NODE;
        code c = EMPTY_CODE;
        state p = /*NO_STATE*/ 0;
        symbol s = /*NO_SYMBOL*/ 0;
        vector<Step> next;
        Step* parent = NULL;
        int index = NONE;           /* Only useful in up simulations: if parent!=NULL, index will be the    */
        bool firstStep = false;     /* position at which the current Step occurs in the children of parent. */
        /* 'firstStep' indicates if the current object is the root Step in the attack tree,
           i.e., if this->p is the state being tested for simulation. */
};

Step& firstStepOf(vector<vector<Step*> >&);
state firstStateOf(vector<vector<Step*> >&);

Step& vectorStepsAt(vector<Step>&, const unsigned int, const char*);
Step* vectorStepPtrsAt(const vector<Step *> &, const unsigned int, const string);
vector<Step*>& vectorVectorStepPtrsAt(vector<vector<Step*> >&, const unsigned int, const char*);
code concatenateCodes(code x, code y);

/* Below is the overloading of the comparison operators for the class Step. According to the rules of thumb,
 * the binary infix comparison operators should be implemented as non-member functions.
 * For efficiency reasons, these definitions are made inline (since they are small functions and, at least
 * in the case of the less-than operator, called very often). The 'one definition rule' for inline
 * functions justifies breaking the general rule that only function declarations go in the header files.
 * The less-than operator is used by std::set<Step>. The remaining ones may not be needed but are defined
 * anyway for consistency purposes. */

inline bool operator<(const Step& lstep, const Step& rstep){
    if (lstep.getState() < rstep.getState())
        return true;
    if (lstep.getState() == rstep.getState()) {
        if (lstep.getSymbol() < rstep.getSymbol())
            return true;
        if (lstep.getSymbol() == rstep.getSymbol()) {
            vector<Step> lchildren = lstep.getNext();
            vector<Step> rchildren = rstep.getNext();
            unsigned int size = lchildren.size();
            for (unsigned int i=0; i<size; i++) {
                if (lchildren.at(i) < rchildren.at(i))
                    return true;
                if (rchildren.at(i) < lchildren.at(i))
                    return false;
            }
         }
    }
    return false;
}

inline bool operator==(const Step& lstep, const Step& rstep){
    if (lstep.getState() == rstep.getState())
        if (lstep.getSymbol() == rstep.getSymbol()) {
            vector<Step> lchildren = lstep.getNext();
            vector<Step> rchildren = rstep.getNext();
            unsigned int size = lchildren.size();
            bool ok = true;
            for (unsigned int i=0; i<size; i++) {
                if (!(lchildren.at(i) == rchildren.at(i)))
                    ok = false;
            }
            if (ok) return true;
         }
    return false;
}

inline bool operator!=(const Step& lstep, const Step& rstep){
    return !(lstep==rstep);
}

inline bool operator>(const Step& lstep, const Step& rstep){
    return !(lstep<rstep) && !(lstep==rstep);
}

/* Returns the code of a step (its state concatenated with its symbol and concatenated
* with the codes of each of its children). */
inline code Step::getCode()  {

    code c = std::to_string(this->getState());
    c += code_delimiter;
    c += std::to_string(this->getSymbol());
    vector<Step>& children = this->getChildren();
    for (Step& child : children)
    {
        c += code_delimiter;
        c += child.getCode();
    }

    return c;
}


/* Returns the code of a step (its state concatenated with its symbol and concatenated
*  with the codes of each of its children) but first generates it and sets it first if it
*  hasn't been set yet. */
inline code Step::getSetCode()  {
    code result;

    if (this->c == EMPTY_CODE)
    {
        code c = std::to_string(this->getState());
        c += code_delimiter;
        c += std::to_string(this->getSymbol());
        vector<Step>& children = this->getChildren();
        for (Step& child : children)
        {
            c += code_delimiter;
            c += child.getSetCode();
        }

        result = c;
        this->c = c;
    }
    else
        result = this->c;

    return result;
}


/* Similar to getCode(), but ignores the state of the first step. Useful in a case where all
   codes will be stored in a structure according to that state.   */
inline code Step::getHeadlessCode()
{
    code result = std::to_string(this->getSymbol());
    vector<Step>& children = this->getChildren();
    for (Step& child : children)
    {
        result += code_delimiter;
        result += child.getCode();
    }

    return result;
}

#endif
