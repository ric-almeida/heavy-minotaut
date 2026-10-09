
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Implementation file for a class for generating all possible combinations of *
 *  transitions from a set of states. It works as an auxiliary class for the        *
 *  computation of the extension of an attack during a simulation computation.      *
 *  Currently not being needed.                                                     *
 * 																					*
 ************************************************************************************/

#include "combinationsGenerator.hh"

Tree::Tree() : children(0)
{	
	if (dbg) std::cout << "Empty object is being created\n";
}

Tree::Tree(int node) : children(0)
{	
	this->node          =  node;
	
	if (dbg) std::cout << "Object " << std::to_string(this->node) << " has been created\n";
}

Tree::Tree(int node, int num_children) : children(num_children)
{	
	this->node          =  node;
	this->children.resize(num_children);
	
	if (dbg) std::cout << "Object " << std::to_string(this->node) << " has been created\n";
}

Tree::~Tree()
{
    if (dbg) cout << "Object " << std::to_string(this->node) << " is being deleted" << endl;
}

int Tree::getNode()
{
	return this->node;
}

unsigned int Tree::numbChildren()
{
	return this->children.size();
}

vector<Tree>& Tree::getChildren()
{
	return this->children;
}

Tree& Tree::getChild(const unsigned int pos)
{
	return vectorTreesAt(this->children, pos, "getChild of a tree");
}

bool Tree::isRoot()
{
	return (this->node == ROOT_NODE);
}

bool Tree::isALeaf()
{
	return this->numbChildren() == 0;
}

void Tree::setChild(unsigned int pos, Tree child)
{
	if (this->children.size() < (pos+1))
		this->children.resize(pos+1);
	
	try {
		this->children.at(pos) = child;
	}
	catch (const std::out_of_range& oor) {
		std::cerr << "Out of Range error: " << oor.what();
        std::cerr << " when changing the value at the position " << std::to_string(pos)
                  << " of this->children which has size " << std::to_string(this->children.size());
		exit_with_error("");
	}
}

string Tree::toString()
{
	if (this == NULL) 
		return "";
	
	string str = "Tree(node=" + std::to_string(this->node) + ", children=<";
	
	unsigned int size = this->getChildren().size();
	if (size == 0)
		return str + ">)";
		
	for(unsigned int i=0; i<size; i++)
	{
		Tree chi = this->getChild(i);
		str += chi.toString();
		if (i<size-1) str += ",";
	}
	
	return str + ">)";
}

Tree createTree(const int root, const vector<unsigned int>& vec, const unsigned int depth, const unsigned int size) {
	if (depth == size) 
		return Tree(root);
	else {
		Tree t = Tree(root);
		unsigned int vec_at_depth = vectorUIntsAt(vec,depth, "vec");
		if (vec_at_depth == 0) {
            t.setChild(0, createTree(Tree::NONE, vec, depth+1, size));
			return t;
		}
		else
			for (unsigned int i=0; i<vec_at_depth; i++) 
				t.setChild(i, createTree(i, vec, depth+1, size));
			return t;
    }
}

vector<vector<int> > getAllRuns(Tree& t){
	vector<vector<int> > all_runs;
	
	if(t.isRoot()) {
		for(unsigned int i=0; i < t.numbChildren(); i++){
			vector<vector<int> > all_runs_i = getAllRuns(t.getChild(i));
			for(unsigned int j=0; j < all_runs_i.size(); j++) 
				all_runs.push_back(all_runs_i.at(j));
		}
	}
	else
	if (t.isALeaf()) {
		all_runs.resize(1);
		vector<int> vec = {t.getNode()};
		all_runs.at(0) = vec;
	}
	else{
		for(unsigned int i=0; i < t.numbChildren(); i++){
			vector<vector<int> > all_runs_i = getAllRuns(t.getChild(i));
			for(unsigned int j=0; j < all_runs_i.size(); j++) {
				all_runs_i.at(j).insert(all_runs_i.at(j).begin(), t.getNode());
				all_runs.push_back(all_runs_i.at(j));		
			}
		}
	}
	
	return all_runs;
}

vector<vector<int> > generateCombinations(const vector<unsigned int>& vec) {
    Tree t = createTree(Tree::ROOT_NODE, vec, 0, vec.size());
	return getAllRuns(t);
}	

vector<vector<MaybeTransition> > translateToTransitions(const vector<vector<transition> >& trans, const vector<vector<int> >& indexCombs) {
	vector<MaybeTransition> row;
	row.reserve(trans.size());
	vector<vector<MaybeTransition> > result (indexCombs.size(), row);
	
	for (unsigned int i=0; i < result.size(); i++) {
		vector<int> comb = indexCombs.at(i);
		for (unsigned int j=0; j < comb.size(); j++) {
			int index = comb.at(j);
            if (index == Tree::NONE) {
				MaybeTransition noTransition = MaybeTransition();
				result.at(i).push_back(noTransition);
			}
			else {
				transition t = trans.at(j).at(index);
				MaybeTransition yesTransition = MaybeTransition(t);
				result.at(i).push_back(yesTransition);
			}
		}
	}
	
	return result;	
}

vector<vector<MaybeTransition> > genAllTransCombinations(const vector<vector<transition> >& all_trans) {
	vector<unsigned int> numbTrans = mapGetNumbTrans(all_trans);
	
	vector<vector<int> > indexCombs = generateCombinations(numbTrans);
	
	return translateToTransitions(all_trans,indexCombs);
}

void genAllTransCombinations2(const unsigned int index, const vector<vector<transition> >& all_trans, vector<MaybeTransition>& combination, vector<vector<MaybeTransition> >* all_combs) {
    if (index == all_trans.size()) {
        (*all_combs).push_back(combination);
    }
    else {
        if (all_trans.at(index).empty()) {
            combination.at(index) = MaybeTransition();
            genAllTransCombinations2(index+1, all_trans, combination, all_combs);
        }
        else {
            for (const transition trans : all_trans.at(index)) {
                combination.at(index) = MaybeTransition(trans);
                genAllTransCombinations2(index+1, all_trans, combination, all_combs);
            }
        }
    }
}

/* Auxiliary function used when ordering a vectors of transitions by the sum of the rankings (of the symbols) of all transitions
 * in each vector. The ordering is done descendly. */
bool orderingFun_byTotalRanks(const pair<vector<MaybeTransition>,typerank>& pair1, const pair<vector<MaybeTransition>,typerank>& pair2) {
    return pair2.second < pair1.second;
}

typerank getTotalRank(const vector<MaybeTransition>& vec, const vector<typerank>& ranks) {
    typerank totalRank = 0;

    for ( MaybeTransition mt : vec) {
        if (mt.isATransition())
            totalRank += mt.getRank(ranks);
    }

    return totalRank;
}

vector<pair<vector<MaybeTransition>,typerank> > attachTotalRanks(const vector<vector<MaybeTransition>>& vecs, const vector<typerank>& ranks) {
    vector<pair<vector<MaybeTransition>,typerank> > vecs_with_ranks;
    vecs_with_ranks.reserve(vecs.size());

    for (const vector<MaybeTransition> vec : vecs) {
        typerank total_rank = getTotalRank(vec, ranks);
        vecs_with_ranks.push_back(std::make_pair(vec,total_rank));
    }

    return vecs_with_ranks;
}

vector<vector<MaybeTransition> > dettachTotalRanks(const vector<pair<vector<MaybeTransition>,typerank> >& vecs) {
    vector<vector<MaybeTransition> > vecs_without_ranks;
    vecs_without_ranks.reserve(vecs.size());

    for (const pair<vector<MaybeTransition>,typerank>& pair_ : vecs) {
        vecs_without_ranks.push_back(pair_.first);
    }

    return vecs_without_ranks;
}

vector<vector<MaybeTransition> > genAllTransCombinations_sortedByRanks(const vector<vector<transition> >& all_trans, const vector<typerank>& ranks) {
    const vector<vector<MaybeTransition> > combs = genAllTransCombinations(all_trans);    // No particular order.

    vector<pair<vector<MaybeTransition>,typerank> > combs_with_ranks = attachTotalRanks(combs, ranks);
    std::sort(combs_with_ranks.begin(), combs_with_ranks.end(), orderingFun_byTotalRanks);

    return dettachTotalRanks(combs_with_ranks);
}

/* Auxiliary function used when ordering a vectors of transitions by the number of destination states which are final.
 * The ordering is done descendly. */
bool orderingFun_byNumbFinalStates(const pair<vector<MaybeTransition>,unsigned int>& pair1, const pair<vector<MaybeTransition>,unsigned int>& pair2) {
    return pair2.second < pair1.second;
}

unsigned int getNumbFinalStates(const vector<MaybeTransition>& vec, const vector<bool>& isFinal) {
    unsigned int numb_final_states = 0;

    for ( MaybeTransition mt : vec ) {
        if (mt.isATransition()) {
            transition trans = mt.getTransition();
            for (const state child : trans.GetChildren())
                if (isFinal.at(child))
                    numb_final_states++;
        }

    }

    return numb_final_states;
}

vector<pair<vector<MaybeTransition>,unsigned int> > attachNumbFinalStates(const vector<vector<MaybeTransition>>& vecs, const vector<bool>& isFinal) {
    vector<pair<vector<MaybeTransition>,unsigned int> > vecs_with_numbs;
    vecs_with_numbs.reserve(vecs.size());

    for (const vector<MaybeTransition> vec : vecs) {
        unsigned int numb_final_states = getNumbFinalStates(vec, isFinal);
        vecs_with_numbs.push_back(std::make_pair(vec,numb_final_states));
    }

    return vecs_with_numbs;
}

vector<vector<MaybeTransition> > dettachNumbFinalStates(const vector<pair<vector<MaybeTransition>,unsigned int> >& vecs) {
    vector<vector<MaybeTransition> > vecs_without_numbs;
    vecs_without_numbs.reserve(vecs.size());

    for (const pair<vector<MaybeTransition>,unsigned int> vec : vecs) {
        vecs_without_numbs.push_back(vec.first);
    }

    return vecs_without_numbs;
}

vector<vector<MaybeTransition> > genAllTransCombinations_sortedByNumbFinalStates(const vector<vector<transition> >& all_trans, const vector<bool>& isFinal) {
    vector<vector<MaybeTransition> > all_combs;
    vector<MaybeTransition> combination(all_trans.size());
    genAllTransCombinations2(0, all_trans, combination, &all_combs);    // No particular order.

    vector<pair<vector<MaybeTransition>,unsigned int> > combs_with_numbs = attachNumbFinalStates(all_combs, isFinal);
    std::sort(combs_with_numbs.begin(), combs_with_numbs.end(), orderingFun_byNumbFinalStates);

    return dettachNumbFinalStates(combs_with_numbs);
}

Tree& vectorTreesAt(vector<Tree>& vec, const unsigned int pos, const char* strct_name){

	try {
        return vec.at(pos);
	}
	catch (const std::out_of_range& oor) {
		std::cerr << "Out of Range error: " << oor.what();
		std::cerr << " when acessing ";
		exit_with_error(strct_name);
	}
	
    return vec.at(0);   // Just to make the compiler happy.
}
