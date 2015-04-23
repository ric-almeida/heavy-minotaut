
#include "simulationHelper.hh"
#include <utility>

vector<unsigned int> OPT;

defence success = 20;
defence fail = 15;
defence weak_fail = 10, strong_fail = 0;

MaybeTransition::MaybeTransition(const transition trans) : trans(trans)
{
    this->isATrans = true;
}

MaybeTransition::MaybeTransition() : trans()
{	
}

bool MaybeTransition::isATransition()
{
	return this->isATrans;
}

transition MaybeTransition::getTransition()
{
	return this->trans;
}

typerank MaybeTransition::getRank(const vector<typerank>& ranks)
{
    if (this->isATrans) {
        try {
            return ranks.at((this->trans).GetSymbol());
        }
        catch (const std::out_of_range& oor) {
            std::cerr << "Out of Range error: " << oor.what()
                      << " when accessing the position " << (this->trans).GetSymbol()
                      << " of vector ranks.";
            exit_with_error("\n");
        }
    }
    else
        exit_with_error("MaybeTransition::getRanks(ranks) called on an object of type MaybeTransition which is not a transition.\n");

    return ranks.at((this->trans).GetSymbol());     // Never executed - just to make the compiler happy.
}

bool isSuccess(defence def)
{
    if (def == success)
        return true;
    else
        return false;
   /*
   if (OPT[THREE_VALUES_LOGIC] == ON && boost::get<unsigned int>(def) == success def == success)
        return true;
   if (OPT[THREE_VALUES_LOGIC] == OFF && boost::get<unsigned int>(def) == true de)
        return true;
   else
        return false;*/
}

bool isStrongFail(defence def)
{
    if (OPT[THREE_VALUES_LOGIC] == OFF)
        exit_with_error("isStrongFail called with the THREE_VALUES_LOGIC disabled.");

    if ((OPT[THREE_VALUES_LOGIC] == ON_V1 || OPT[THREE_VALUES_LOGIC] == ON_V2) && def == strong_fail)
        return true;

    return false;
}

bool isWeakFail(defence def)
{
    if (OPT[THREE_VALUES_LOGIC] == OFF)
        exit_with_error("isWeakFail called with the THREE_VALUES_LOGIC disabled.");

    if ((OPT[THREE_VALUES_LOGIC] == ON_V1 || OPT[THREE_VALUES_LOGIC] == ON_V2) && def == weak_fail)
        return true;

    return false;
}

bool isFail(defence def)
{
    if (OPT[THREE_VALUES_LOGIC] == ON_V1 || OPT[THREE_VALUES_LOGIC] == ON_V2)
        exit_with_error("isFail called with the THREE_VALUES_LOGIC disabled.");

    return def==fail;
}

bool isBetter(defence def1, defence def2)
{
    return def1 > def2;
}

bool isWorse(defence def1, defence def2)
{
    return def2 > def1;
}

/* A function that, given a vector containing the transitions per state, and a vector containing at each position a pointer
 * ptr_i to a Step, returns a vector containing at each position a vector with all the 
 * transitions from ptr_i->getState(). */
transitions mapGetTrans(const Automaton& aut, const vector<Step*>& steps) {
	unsigned int size = steps.size();
	
    transitions result(size);
    state p, initialState = getInitialState(aut);
	bool isEmpty = true;
	for(unsigned int i=0; i<size; i++) {
		Step* step = vectorStepPtrsAt(steps,i,"steps");
		p = step->getState();
		
        if (p==initialState) continue;

        for (const transition t : aut[p]) {
            result.at(i).push_back(t);
			isEmpty = false;
		}
	}
	
	if (isEmpty) result.resize(0);
	
	return result;
}

vector<state> mapGetState(vector<Step>& vec) {
    unsigned int size = vec.size();
    vector<state> vec_res(size);

    for (unsigned int i=0; i<size; i++)
        vec_res.at(i) = vec.at(i).getState();

    return vec_res;
}

/* We use a structure called 'steps_matrix' which allows us to, at any point of the execution
 * of the program, quickly access all the Steps with a certain depth. This structure has
 * type vector<vector<Step*> >, a vector that at each index i stores a vector with pointers
 * to all the Steps with depth i. */
vector<vector<Step*> >* initializeStepsMatrix(vector<vector<Step*> >& steps_matrix, const int la, Step* first)
{	
	steps_matrix.reserve(la+1);
	
	try{
		vector<Step*> first_row(1);
		first_row.at(0) = first;
		steps_matrix.push_back(first_row);
	}
	catch (const std::out_of_range& oor) {
		std::cerr << "Out of Range error: " << oor.what();
		exit_with_error(" when acessing steps_matrix.");
	}	
	
	return &steps_matrix;
}

string stepsMatrix2String(const vector<vector<Step*>>& steps_matrix)
{
	string str;
	vector<Step*> row;
	
	for (unsigned int i=0; i<steps_matrix.size(); i++)
	{
		str += "\nSteps of depth " + std::to_string(i) + ": ";
		
		try {
			row = steps_matrix.at(i);
		}
		catch (const std::out_of_range& oor) {
			std::cerr << "Out of Range error: " << oor.what();
			exit_with_error(" when acessing steps_matrix.");
		}
		
		for (unsigned int j=0; j<row.size(); j++)
		{
			str += row.at(j)->toString() + " ";
		}
		str += "\n";
	}
	
	return str;
}

/* Loops through the given matrix, erasing the code recorded in each step. */
void eraseCodeAllSteps(vector<vector<Step*> >& steps)
{
    for (vector<Step*>& vec : steps)
        for (Step* step : vec)
            step->eraseCode();

}

/* Extends the attack 'steps'. */
void extendAttack(vector<vector<Step*> >& steps, const unsigned int depth, vector<MaybeTransition>& transitions, const vector<typerank>& ranks) {
	if (steps.size()-1 < depth+1)
		steps.resize(steps.size()+1);
	
    /* First get the row of steps corresponding to the leaves of the current attack */
    vector<Step*> row = steps.at(depth);        // returns a reference.
		
    for (unsigned int i=0; i < transitions.size(); i++) {   // I could be taking each transition by reference
		if (transitions.at(i).isATransition()) {
			transition trans = transitions.at(i).getTransition();
            symbol s = trans.GetSymbol();
            vector<state> children = trans.GetChildren();
			
            /* Each of those steps is then extended with a transition (if it exists) */
			Step* step = row.at(i);
            typerank r = ranks[s];
            step->set(s, r, children);

			for(unsigned int j=0; j < children.size(); j++) {
                /* We now have a new row of (ptrs of) steps in the attack, which has just been extended.
                 * So save this row in the matrix steps. */
				try {
					steps.at(depth+1).push_back(step->getChildAddr(j));  
				}
				catch (const std::out_of_range& oor) {
					std::cerr << "Out of Range error: " << oor.what() << 
					"when acessing the position " << depth+1 << 
					" of steps which has size " << steps.size();
					exit_with_error("");
				}
			}
		}		
	}

    if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS]==GLOBAL_V2 || OPT[TYPE_OF_HISTORY_OF_BAD_ATKS]==GLOBAL_V2)
    {
        eraseCodeAllSteps(steps);
    }

}

/* String and IO functions */

/*string vectorVectorMaybeTransitionsToString(vector<vector<MaybeTransition> > vec)
{
	string str;
	
	for (unsigned int i=0; i<vec.size(); i++){
		vector<MaybeTransition> subvec = vec.at(i);
		for (unsigned int j=0; j<subvec.size(); j++)
			if (subvec.at(j).isATransition())
				str += transitionToString(subvec.at(j).getTransition()) + ",";
			else
				str += "<no trans>,";
		str += "\n";
	}
	
	return str;
}

void printVectorVectorMaybeTransitions(vector<vector<MaybeTransition> > vec)
{
	std::cout << vectorVectorMaybeTransitionsToString(vec) << "\n";
}*/

/* Test Functions */

state* createNStates(state* states, int n1, int n2, int n)
{
	string pref = std::to_string(n1) + std::to_string(n2);
	string name;
	
	for(int i=1; i<=n; i++){
		name = pref + std::to_string(i);
		states[i-1] = std::stoi(name);
	}
	 
	return states;
}		

/*
void test_extendAttack(vector<vector<Step*> >& steps_matrix, unsigned int depth, int n)
{
	state* states = new state[n];
	
	if (depth < steps_matrix.capacity()-1){
		vector<Step*> steps = steps_matrix.at(depth);
		for (unsigned int i=0; i<steps.size(); i++)
		{
			states = createNStates(states,depth+1,i+1,n);
			Step* step = steps.at(i);

			int rank = getRank(n);
			step->setSymbol(n,rank);
			
			if (steps_matrix.size()-1 < depth+1) 
				steps_matrix.resize(steps_matrix.size()+1);
			
			for (int j=0; j<rank; j++)
			{				
				step->setChild(states[j],j);
				
				try {
                   steps_matrix.at(depth+1).push_back(step->getChildAddr(j));
				}
				catch (const std::out_of_range& oor) {
					std::cerr << "Out of Range error: " << oor.what();
					exit_with_error(" when acessing steps_matrix.");
				}
			}
		}
		test_extendAttack(steps_matrix,depth+1,n);
	}
	else
		std::cout << "The matrix has been updated to " << stepsMatrix2String(steps_matrix) << "\n";
}
*/

void initializeW(const Automaton& aut, vector<vector<bool> >& W,
                 const unsigned int n, const bool strict)
{
    stateSet usedStates = getUsedStates(aut);

    vector<bool> column (n,false);
    W.assign(n,column);

    for (state p : usedStates)
        for (state q : usedStates)
        {
            if (strict && p==q)
                W.at(p).at(q) = false;
            else
                W.at(p).at(q) = true;
        }

//    for (unsigned int i=0; i<n; i++)
//        for (unsigned int j=0; j<n; j++)
//        {
//            if (strict && i==j)
//                W.at(i).at(j) = false;
//            else
//                W.at(i).at(j) = true;
//        }
			
}

/* Computes the transitive closure of a binary relation represented by the
 * boolean nxn matrix W. It uses Warshall's algorithm.
 * Returns a copy of W, so that the matrix before the transClosure call is still available. */
vector<vector<bool> > transClosure(vector<vector<bool> > W, const unsigned int n) {
    for (unsigned int p=0; p<n; p++)
        for (unsigned int q=0; q<n; q++)
            if (p!=q && W.at(q).at(p))
                for (unsigned int r=0; r<n; r++)
                    if (W.at(p).at(r))
                        W.at(q).at(r) = true;

    return W;
}

/* Computes the asymetric restriction of the transitive closure of W. */
vector<vector<bool> > asymTransClosure(vector<vector<bool> >& W, const unsigned int n) {
    vector<vector<bool> > W_trans = transClosure(W,n);

    for (unsigned int i=0; i<n; i++)
        for (unsigned int j=0; j <= i; j++)
            if (W_trans.at(i).at(j) && W_trans.at(j).at(i))
                W_trans.at(j).at(i) = false;

    return W_trans;
}

/* Extracts the strict subrelation of R. The function keeps in R only
 * those (x,y) s.t. xRy but not yRx.
 * Note: This is not the same as the asymmetric restriction of R. */
void extractStrictRelation(vector<vector<bool> >& R, const unsigned int n) {
    for (unsigned int i=0; i<n; i++)
        for (unsigned int j=0; j<=i; j++)
            if (R.at(i).at(j) && R.at(j).at(i)) {
                R.at(i).at(j) = false;
                R.at(j).at(i) = false;
            }
}

bool areInRel(const state p, const state q, const vector<vector<bool> >& W) {

    try {
        return W.at(p).at(q);
    }
    catch (const std::out_of_range& oor) {
        std::cerr << "Out of Range error: " << oor.what()
                  << "when acessing the states-pair (" << p
                  << "," << q << ") in a boolean matrix.";
        exit_with_error("");
    }

    return W.at(p).at(q);
}

void printStateBinRelation(const stateDiscontBinaryRelation& binRel, const unsigned int numb_states, const stateDict& dict) {
    /*std::cout << "{";

    for (unsigned int p=0; p<binRel.size_; p++)
        for (unsigned int q=0; q<binRel.size_; q++) {
            if (binRel.get(p,q))
                std::cout << "(" << translateState(dict,p) << "," << translateState(dict,q) << ") ";
        }*/

/*    bool placeComma = false;
    for (auto firstStateToMappedPair : dict_)
        {	// iterate over all pairs
            for (auto secondStateToMappedPair : dict_)
            {
                if (this->get(firstStateToMappedPair.first, secondStateToMappedPair.first))
                {	// if the pair is in the relation print
                    str << (placeComma? ", " : "");
                    str << "(" << firstStateToMappedPair.first << ", " <<
                        secondStateToMappedPair.first << ")";
                    placeComma = true;
                }
            }
        }


    std::cout << "}\n"; */

    std::cout << binRel << "\n";
}

/* Converts libvata's stateDiscontBinaryRelation type to the local vector<vector<bool> >
   type. Note that it ignores those states which are not in any transition of the
   automaton. */
void convertBinaryRelToBoolMatrix(const Automaton& aut,
                                  const stateDiscontBinaryRelation& binRel,
                                  vector<vector<bool> >& W)
{    
    VATA::Util::BinaryRelation matrix = binRel.getMatrix();

    stateSet usedStates = getUsedStates(aut);
    state initialState  = getInitialState(aut);

    for (const state& p : usedStates)
        for (const state& q : usedStates)
            if (p == initialState)
            {
                if (q == initialState)
                    W.at(p).at(q) = true;
                else W.at(p).at(q) = false;
            }
            else
                W.at(p).at(q) =
                        matrix.data_[binRel.getDict().TranslateFwd(p)*matrix.rowSize_+binRel.getDict().TranslateFwd(q)];

    /*
    vector<bool> column(numb_states,false);
    vector<vector<bool> > matrix(numb_states,column);
    unsigned int size = binRel.size();
    for (unsigned int p=0; p<binRel.size(); p++)
        for (unsigned int q=0; q<binRel.size(); q++)
            if (binRel.get(p,q))
                matrix[p][q] = true; */

    /*for (auto firstStateToMappedPair : binRel.dict_)
        {	// iterate over all pairs
            for (auto secondStateToMappedPair : binRel.dict_)
            {
                if (this->get(firstStateToMappedPair.first, secondStateToMappedPair.first))
                {	// if the pair is in the relation print
                    matrix[firstStateToMappedPair.first][secondStateToMappedPair.first] = true;
                }
            }
        }*/

    //return matrix;
}

vector<transition> moveInitialTransitionsToBeginning(vector<transition>& vec) {
    vector<transition> newVec;
    newVec.reserve(vec.size());
    for (vector<transition>::iterator it=vec.begin(); it!=vec.end(); it++)
    {
        if ((*it).GetChildren().empty())
        {
            newVec.push_back(*it);
            vec.erase(it);
        }
    }

    newVec.insert(newVec.end(), vec.begin(), vec.end());

    return newVec;
}

vector<pair<transition,typerank> > attachRank(const vector<transition>& vec, const vector<typerank>& ranks) {
    vector<pair<transition,typerank> > vec_with_ranks;
    vec_with_ranks.reserve(vec.size());

    for (const transition& trans : vec) {
        vec_with_ranks.push_back(std::make_pair(trans, ranks.at(trans.GetSymbol())));
    }

    return vec_with_ranks;
}

vector<transition> dettachRanks(const vector<pair<transition,typerank> >& vec) {
    vector<transition> vec_without_ranks;
    vec_without_ranks.reserve(vec.size());

    for (const pair<transition,typerank>& pair_ : vec) {
        vec_without_ranks.push_back(pair_.first);
    }

    return vec_without_ranks;
}

bool orderingFun_byRank(const pair<transition,typerank>& pair1, const pair<transition,typerank>& pair2)
{
    return pair2.second < pair1.second;
}

vector<transition> orderTransBySymbolsRankings(vector<transition>& vec, const vector<typerank>& ranks) {
    vector<pair<transition,typerank> > vec_with_ranks = attachRank(vec, ranks);

    std::sort(vec_with_ranks.begin(), vec_with_ranks.end(), orderingFun_byRank);

    return dettachRanks(vec_with_ranks);
}

/* This function checks that states1[i] W states2[i] is true for any i.
 * If so, states1 and states2 are in the relation W lifted to vectors of states.
 * In case flag_asym=true, it additionally requires that states1[i] != states2[i]
 * for at least one i. If it should be the case, then W lifted to vectors is strict. */
bool areInRelIter(const vector<state>& states1, const vector<state>& states2, const vector<vector<bool> >& W, const bool flag_strict) {
    unsigned int n = states1.size();
    if (n != states2.size())
        exit_with_error("function areInRelIter received two vectors of states of different size.");

    state p,q;
    bool ok_inRel=true, ok_strictness=false;
    for (unsigned int i=0; i<n; i++) {
        p = states1.at(i);
        q = states2.at(i);

        if (!areInRel(p, q, W)) {
            ok_inRel = false;
            break;
        }

        if (flag_strict)
            if (!areInRel(q, p, W))
                ok_strictness = true;

    }

    if (flag_strict)
        return ok_inRel&&ok_strictness;
    else
        return ok_inRel;
}

vector<vector<bool> > generateIdRelation(const unsigned int n) {
    vector<bool> column(n);
    vector<vector<bool> > W(n, column);

    for (unsigned int i=0; i<n; i++)
        for (unsigned int j=0; j<n; j++) {
            if (i==j)
                W.at(i).at(j) = true;
            else
                W.at(i).at(j) = false;
        }

    return W;
}

unsigned int getSizeOfRel(const vector<vector<bool> >& W, const unsigned int n) {
    unsigned int c = 0;

    for (unsigned int i=0; i<n; i++)
        for (unsigned int j=0; j<n; j++)
            if (W.at(i).at(j)) c++;

    return c;
}

vector<pair<transition,size_t> >& vectorVectorPairTransitionIntAt(vector<vector<pair<transition,size_t>> >& vector, const state i) {

    //std::vector<pair<transition,size_t>> answer;

    try {
        return vector.at(i);
    }
    catch (const std::out_of_range& oor) {
        std::cerr << "Out of Range error: " << oor.what()
                  << "when trying to access position " << i
                  << "of a vector of vectors of pairs of type (transition,int), which has size "
                  << vector.size();
        exit_with_error("\n");
    }

    return vector.at(i);     // Just to make the compiler happy.
}

/* IO and String conversion functions */

string w2String(const Automaton& aut, const vector<vector<bool> >& W, const unsigned int n, const stateDict *dict){
    string str = "{";
	
	for (unsigned int i=0; i<n; i++)
		for (unsigned int j=0; j<n; j++)
			if (W.at(i).at(j))
                str += "(" + (dict==NULL ? std::to_string(i) : translateState(aut,*dict,i)) + "," +
                             (dict==NULL ? std::to_string(j) : translateState(aut,*dict,j)) + ") ";
	
    return str + "}";
}

void printW(const Automaton& aut, const vector<vector<bool> >& W, const unsigned int n, const stateDict *dict){
    std::cout << w2String(aut, W, n, dict);
}

void printAttack(const Automaton& aut, Step& step) {
    std::cout << "(" << step.getState() << "," << step.getSymbol() << ",<";
    for (unsigned int i=0; i<step.getChildren().size(); i++){
        printAttack(aut, step.getChildren().at(i));
        if (i<step.getChildren().size()-1)
            std::cout << ",";
    }
    std::cout << ">)";
}