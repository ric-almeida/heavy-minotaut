
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Implementation file for the class Step. The construction of a lookahead     *
 *  sim. on the states of a Bottom-Up Tree Automaton (BUTA) is based on verifying   *
 *  if there is some attack from a state p against which a state q cannot defend.   *
 *  An attack is a tree of transition rules with root at p.							*
 * 	Each transition rule (<p1,p2,...,pn>,s,p) is here represented as 				*
 * 	a Step(p,s,[Step c1,Step c2,...,Step cn]), where each Step ci represents the	*
 * 	transition from pi considered in the attack. A leaf-rule (<>,s,p) 				*
 * 	(i.e., a rule with no departing states) is represented as Step(p,s,[stepPsi]),	*
 *  where stepPsi is the (unique) initial state of a BUTA made explicit.            *
 * 																					*
 ************************************************************************************/

#include "step.hh"


Step::Step() : next(0)
{
    if (dbg) std::cout << "Empty object is being created\n";
}

Step::Step(state p) : next(0)
{
    this->p          =  p;

    if (dbg) std::cout << "Object " << std::to_string(this->p) << " has been created\n";
}

Step::Step(state p, node_no node) : next(0)
{
    this->p     =  p;
    this->node  =  node;

    if (dbg) std::cout << "Object " << std::to_string(this->p) << " has been created\n";
}

Step::Step(state p, bool firstStep) : next(0)
{
    this->p          =  p;
    this->firstStep  =  firstStep;
    if (firstStep) this->node = "0";

    if (dbg) std::cout << "Object " << std::to_string(this->p) << " has been created\n";
}

Step::Step(state p, symbol s, typerank rank, bool firstStep) : next(rank)
{
    this->p          =  p;
    this->s    		 =  s;
    this->firstStep  =  firstStep;
    if (firstStep) this->node = "0";

    if (dbg) std::cout << "Object " << std::to_string(this->p) << " has been created\n";
}

Step::Step(state p, symbol s, typerank rank, const vector<state>& children, bool firstStep) : next(rank)
{
    this->p          =  p;
    this->s    		 =  s;
    this->setChildren(children);
    this->firstStep  =  firstStep;
    if (firstStep) this->node = "0";

    if (dbg) std::cout << "Object " << std::to_string(this->p) << " has been created\n";
}

Step::~Step()
{
    if (dbg) cout << "Object " << std::to_string(this->p) << " is being deleted" << endl;
}

node_no Step::getNode()
{
    return this->node;
}

state Step::getState() const
{
    //if (this->p == NO_STATE)
      //  exit_with_error("Step::getState() called on a Step with no state.");

    return this->p;
}

symbol Step::getSymbol() const
{
    return this->s;
}

std::vector<Step> Step::getNext() const
{
    return this->next;
}

std::vector<Step>& Step::getChildren()
{
    return this->next;
}

Step* Step::getChildAddr(int pos)
{
    try {
        return &(this->next.at(pos));
    }
    catch (const std::out_of_range& oor) {
        std::cerr << "Out of Range error: " << oor.what();
        exit_with_error(" when acessing this->next.");
    }

    return NULL;	/* Just to make the -Wreturn-type g++ option happy. */
}

Step& Step::getParent()
{
    if (this->parent == NULL)
        exit_with_error("Error trying to call getParent() on a Step with parent=NULL.");

    return *(this->parent);
}

int Step::getIndex()
{
    if (this->index == NONE)
        exit_with_error("Step::getIndex() called on a Step with index=NONE.");

    return this->index;
}

/* Sets the symbol of this step to 's' and sets next to an array of pointers to
 * steps for each of the states in 'states', and returns next. */
void Step::setSymbol(symbol s, typerank r)
{
    this->s  =  s;
    this->next.resize(r);
}

void Step::setNode(node_no node)
{
    this->node = node;
}

void Step::setChild(state p, int pos)
{
    try {
        (this->next).at(pos) = Step(p, this->getNode() + std::to_string(pos));
    }
    catch (const std::out_of_range& oor) {
        std::cerr << "Out of Range error: " << oor.what()
                  << "when trying to create a Step(" << p
                  << ") at the position " << pos
                  << " of this->next, which has size "
                  << this->next.size();
        exit_with_error("\n");
    }
}

void Step::setChild(Step& child, int pos)
{
     try {
        child.setNode(this->getNode() + std::to_string(pos));
        (this->next).at(pos) = child;
    }
    catch (const std::out_of_range& oor) {
        std::cerr << "Out of Range error: " << oor.what()
                  << "when trying to place a Step"
                  << " at the position " << pos
                  << " of this->next, which has size "
                  << this->next.size();
        exit_with_error("\n");
    }
}

void Step::setChildren(const vector<state>& children)
{
    for (unsigned int i=0; i < children.size(); i++)
        this->setChild(children.at(i), i);
}

void Step::setParent(Step* parent)
{
    this->parent = parent;
}

void Step::setIndex(size_t index)
{
    this->index = index;
}

void Step::set(symbol s, typerank r, const vector<state>& children)
{
    if (r==0)
    {
        exit_with_error("Step->set received r=0");
    }
    else {
        this->setSymbol(s, r);
        this->setChildren(children);
    }
}

unsigned int Step::numbChildren(){
    return this->next.size();
}

bool Step::isFirstStep()
{
    return this->firstStep;
}

bool Step::isALeafStep()    /* Applicable in a dw simulation context. */
{
    return (this->next).capacity() == 0;
}

bool Step::isLastStep()     /* Applicable in a up simulation context. */
{
    return (this->parent) == NULL;
}

string Step::toString()
{
    if (this == NULL)
        return "";

    string str = "Step(p=" + std::to_string(this->p) + ", s=" + std::to_string(this->s)
                 + ", next=<";

    //symbol s = this->getSymbol();
    if (/*s == NO_SYMBOL*/ this->next.empty())
        return str + ">)";

    unsigned int size = this->numbChildren();
    for(unsigned int i=0; i<size; i++)
    {
        state pi = this->next[i].getState();
        str += std::to_string(pi);
        if (i<size-1) str += ",";
    }

    return str + ">)";
}

void Step::eraseCode(){
    this->c = EMPTY_CODE;
}

code Step::getEmptyCode()
{
    return EMPTY_CODE;
}

Step& firstStepOf(vector<vector<Step*> >& matrix) {

    vector<Step*> row;
    try {
        row = matrix.at(0);
    }
    catch (const std::out_of_range& oor) {
        std::cerr << "Out of Range error: " << oor.what();
        exit_with_error(" when acessing matrix.");
    }

    try {
        return *(row.at(0));
    }
    catch (const std::out_of_range& oor) {
        std::cerr << "Out of Range error: " << oor.what();
        exit_with_error(" when acessing row.");
    }

    return *(row.at(0));
}

state firstStateOf(vector<vector<Step*> >& matrix) {

    return firstStepOf(matrix).getState();
}

/* Functions that try to return the element at a certain position of a vector
 * and throw an exception if a out_of_range error occurs. */

Step& vectorStepsAt(vector<Step>& vec, const unsigned int pos, const char* strct_name){

    try
    {
        return vec.at(pos);
    }
    catch (const std::out_of_range& oor) {
        std::cerr << "Out of Range error: " << oor.what();
        std::cerr << " when acessing ";
        exit_with_error(strct_name);
    }

    return vec.at(0);       // Just to make the compiler happy.
}

Step* vectorStepPtrsAt(const vector<Step*>& vec, const unsigned int pos, const string strct_name){

    try {
        return vec.at(pos);
    }
    catch (const std::out_of_range& oor) {
        std::cerr << "Out of Range error: " << oor.what();
        std::cerr << " when acessing the position " << std::to_string(pos) << " of "
        << strct_name << " which has size " << vec.size();
        exit_with_error("\n");
    }

    return vec.at(pos);
}

vector<Step*>& vectorVectorStepPtrsAt(vector<vector<Step*> >& vec, const unsigned int pos, const string strct_name){

    try {
        return vec.at(pos);
    }
    catch (const std::out_of_range& oor) {
        std::cerr << "Out of Range error: " << oor.what();
        std::cerr << " when acessing the position " << std::to_string(pos) <<
        " from " << strct_name << " which has size " << std::to_string(vec.size());
        exit_with_error("\n");
    }

    return vec.at(0);       // Just to make the compiler happy.
}
