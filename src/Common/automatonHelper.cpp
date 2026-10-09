
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Implementation of a collection of functions that aid in the manipulation    *
 *  and analysis of automata.                                                       *
 * 																					*
 ************************************************************************************/

#include "automatonHelper.hh"


Automaton addInitialState(const Automaton& old_aut)
{
    Automaton new_aut = copyAutWithoutTrans(old_aut);

    vector<state> children;
    bool hasLeafRules = false;
    state initialState = getGreatestUsedState(old_aut)+1;

    for (const lv_transition& trans : old_aut) {
        if (!trans.GetChildren().empty())
            new_aut.AddTransition(trans.GetChildren(), trans.GetSymbol(), trans.GetParent());
        else
        {
            hasLeafRules = true;
            children = {initialState};
            new_aut.AddTransition(children, trans.GetSymbol(), trans.GetParent());
        }
    }

    if (!hasLeafRules)
    {
        Automaton empty;
        return empty;
    }

    return new_aut;
}

Automaton removeInitialState(const Automaton& old_aut)
{
    state initialState = getInitialState(old_aut);
    // This assumes that the explicit initial state has been added to the automaton,
    // which necessarily happens except when the input automaton had no leaf-rules
    // (in such a case, discarding the automaton is advisable).

    Automaton new_aut = copyAutWithoutTrans(old_aut);
    vector<state> empty_children(0), children, initial_state_children = {initialState};

    for (const lv_transition& trans : old_aut)
    {
        children = trans.GetChildren();
        if (children == initial_state_children)
            new_aut.AddTransition(empty_children, trans.GetSymbol(), trans.GetParent());
        else
            new_aut.AddTransition(children, trans.GetSymbol(), trans.GetParent());
    }

    return new_aut;
}

/* Reindex the states if they are not continuous, e.g.: states = {0,2,5} becomes {0,1,2}.
 * This function was implemented with the aim of being used in between steps that delete states in the automaton,
 * namely Quotienting and Pruning techniques. However, calling it then turned out to make the overall execution slower. */
AutData reindexStates(const AutData& oldAutData)
{

    unsigned int greatestState = getGreatestUsedState(oldAutData);
    if (getNumbUsedStates(oldAutData) == greatestState+1)
        return oldAutData;     /* The states are already continuous. */

    Automaton aut = removeInitialState(getAut(oldAutData));

    state stateCnt = 0;
    stateToStateMap stateMap;
    stateToStateTranslWeak stateTransl(stateMap,
                                      [&stateCnt](const state&){return stateCnt++;});

    aut = aut.ReindexStates(stateTransl);

    aut = addInitialState(aut);

    AutData newAutData = wrapAutData(aut, getRanks(oldAutData));

    return newAutData;
}

AutData removeUselessStates(const AutData& old_autData)
{
    Automaton old_aut = getAut(old_autData);
    Automaton new_aut = removeInitialState(old_aut);
    new_aut = new_aut.RemoveUselessStates();
    new_aut = addInitialState(new_aut);

    AutData new_autData = wrapAutData(new_aut,getRanks(old_autData));

    return new_autData;
}

string removeAlphabetLine(string autStr)
{
    string prefix = "Ops\n";
    string rest = autStr.substr(autStr.find("Automaton"), autStr.length());

    return prefix + rest;
}

set<symbol> getSymbolsFrom(const Automaton& aut, state p, bool ignoreLeafSymbols)
{
    set<symbol> symbols;

    for (const lv_transition& trans : aut[p])
    {
        if (isALeafTransition(trans, aut) && ignoreLeafSymbols)
            continue;

        symbols.insert(trans.GetSymbol());
    }

    return symbols;
}

set<symbol> getUsedSymbols(const Automaton& aut, bool ignoreLeafSymbols)
{
    set<symbol> symbols;
    stateSet states = getUsedStates(aut);

    for (state p : states)
    {
        set<symbol> symbols_from_p = getSymbolsFrom(aut, p, ignoreLeafSymbols);
        symbols.insert(symbols_from_p.begin(), symbols_from_p.end());
    }

    return symbols;
}

/* This function is not completely reliable: it should only be used immediately after the
 * automaton has been parsed. */
vector<typerank> getRanks(const Automaton& aut)
{
    set<symbol> symbols = getUsedSymbols(aut);
    unsigned int greatest_symbol = getGreatestUsedSymbol(aut);  // Could be improved.
    vector<typerank> ranks(greatest_symbol+1);  /* This alloc guarantees we keep the ranks for all symbs, */
                                                /* even if their int representation is not continuous.    */

    // Get a pointer to the alphabet of type onTheFlyAlphabet which is being used already
    // by 'aut'.
    std::shared_ptr<onTheFlyAlphabet> alph;
    if (!(alph = std::dynamic_pointer_cast<onTheFlyAlphabet>(aut.GetAlphabet())))
    {	// The alphabet should be ExplicitTreeAut::onTheFlyAlphabet,
        // if it isn't then throw 'false'.
        assert(false);
    }

    symbol s;
    typerank rank;
    for (auto stringSymbolPair : alph->GetSymbolDict())
    {
        s = stringSymbolPair.second;
        if (symbols.find(s) == symbols.end())
            continue;
        rank = stringSymbolPair.first.rank;
        if (rank==0) rank = 1;      /* Since we make the initial state explicit. */
        ranks.at(s) = rank;
        if (s > greatest_symbol)
            exit_with_error("Found a symbol greatest than the greatest symbol!");
    }

    return ranks;
}


AutData parseFromString(string autStr, stateDict& sDict)
{
    /* Parsing the automaton. */
    std::unique_ptr<VATA::Parsing::AbstrParser> parser(new VATA::Parsing::TimbukParser());

    Automaton aut;
    aut.LoadFromString(*parser, autStr, sDict);

    aut = addInitialState(aut);

    /* Getting the ranks of the symbols. */
    vector<typerank> ranks = getRanks(aut);

    AutData autData = AutData (aut,ranks);

    return autData;
}

Automaton parseFromString2(string autStr, stateDict& stateDict, vector<typerank>& ranks)
{
    /* Parsing the automaton. */
    std::unique_ptr<VATA::Parsing::AbstrParser> parser(new VATA::Parsing::TimbukParser());

    Automaton aut;
    aut.LoadFromString(*parser, autStr, stateDict);

    aut = addInitialState(aut);

    /* Getting the ranks of the symbols. */
    ranks = getRanks(aut);

    return aut;
}

AutData parseFromFile(string filename, stateDict& sDict)
{
    return parseFromString(VATA::Util::ReadFile(filename), sDict);
}

/* Caution: an automaton parsed with this function cannot be printed using printAut
 * since the stateDict parameter is missing. */
AutData parseFromFile(string filename)
{
    stateDict sDict;
    return parseFromFile(filename, sDict);
}

string autToStringTimbuk(const Automaton& aut, stateDict& sDict)
{
    VATA::Serialization::AbstrSerializer* serializer =
        new VATA::Serialization::TimbukSerializer();
    // dump the automaton
    string result = aut.DumpToString(*serializer, sDict);

    return result;
}

string autToStringTimbuk(const Automaton& aut)
{
    string result;

    VATA::Serialization::AbstrSerializer* serializer =
        new VATA::Serialization::TimbukSerializer();
    // dump the automaton
    result = aut.DumpToString(*serializer);

    return result;
}

/* Note that this function REWRITES the output automaton if a file with the same same already exists! */
void saveAutToFile(const Automaton& aut, stateDict& sDict, string filename, bool removeInitialSt)
{
    Automaton aut_;
    if (removeInitialSt)
        aut_ = removeInitialState(aut);
    else
        aut_ = aut;

    writeToFile(filename, autToStringTimbuk(aut_,sDict), true);
}

void saveAutToFile(const AutData& autData, stateDict& sDict, string filename, bool removeInitialSt)
{
    saveAutToFile(getAut(autData), sDict, filename, removeInitialSt);
}

void saveAutToFile(const Automaton& aut, string filename, bool removeInitialSt)
{
    Automaton aut_;
    if (removeInitialSt)
        aut_ = removeInitialState(aut);
    else
        aut_ = aut;

    string str_aut = autToStringTimbuk(aut_);

    writeToFile(filename, str_aut, true);
}

void saveAutToFile(const AutData& autData, string filename, bool removeInitialSt)
{
    saveAutToFile(getAut(autData), filename, removeInitialSt);
}

tuple<AutData,stateDict> dumpAndLoadAut(const Automaton& aut)
{
    string temp_filename = "aut_" + std::to_string(unique_id()) + ".timbuk";
    saveAutToFile(aut, temp_filename, true);

    stateDict new_sDict;
    AutData autData_f = parseFromFile(temp_filename, new_sDict);
    deleteFile(temp_filename);

    tuple<AutData,stateDict> result = make_tuple(autData_f,new_sDict);

    return result;
}

tuple<AutData,stateDict> dumpAndLoadAut(const AutData &autData_i)
{
    Automaton aut = getAut(autData_i);
    const vector<typerank> ranks = getRanks(autData_i);

    tuple<AutData,stateDict> tup = dumpAndLoadAut(aut);

    return tup;
}

const Automaton &getAut(const AutData &autData)
{
    const Automaton& aut = std::get<0>(autData);
    return aut;
}

const vector<typerank>& getRanks(const AutData &autData)
{
    return std::get<1>(autData);
}

AutData wrapAutData(const Automaton& aut, const vector<typerank>& ranks)
{
    return AutData (aut,ranks);
}

vector<unsigned int> mapGetNumbTrans(const vector<vector<transition> >& vec) {
    unsigned int size = vec.size();

    vector<unsigned int> result(size);
    for(unsigned int i=0; i<size; i++)
        result.at(i) = vec.at(i).size();

    return result;
}

/* Gets all the used states (i.e., states which appear in transitions
 * as parents or children) of the given automaton or are final states. */
stateSet getUsedStates(const Automaton& aut)
{
    // Create an unordered set called states.
    std::unordered_set<state> states;

    for (const lv_transition trans : aut)
    {
        // For each transition in 'aut', insert its parent in 'states'.
        states.insert(trans.GetParent());

        for (const state s : trans.GetChildren())
        {
            // Now for each children in that same transition,
            // insert it in 'states'.
            states.insert(s);
        }
    }

    finalStateSet f = aut.GetFinalStates();
    for (const state s : f)
    {
        states.insert(s);
    }

    return states;
}

/* All our measures count the special initial state as a state and the initial transitions
 * as ordinary transitions, therefore the default value for ignoreInitislSt is false. */
unsigned int getNumbUsedStates(const Automaton& aut, bool ignoreInitialSt)
{
    if (ignoreInitialSt && getNumbUsedStates(aut) > 0)
        return getUsedStates(aut).size()-1;
    else
        return getUsedStates(aut).size();
}

unsigned int getNumbUsedStates(const AutData& autData, bool ignoreInitialSt)
{
    return getNumbUsedStates(getAut(autData), ignoreInitialSt);
}

string getNumbUsedStates_str(const AutData& autData, bool ignoreInitialSt)
{
    return std::to_string(getNumbUsedStates(autData, ignoreInitialSt));
}

string getNumbUsedStates_str(const Automaton& aut, bool ignoreInitialSt)
{
    return std::to_string(getNumbUsedStates(aut, ignoreInitialSt));
}

float getTransitionDensity(const Automaton& aut, bool ignoreLeafRules)
{
    float transDens = 0;

    unsigned int numb_states = getNumbUsedStates(aut, ignoreLeafRules);
    unsigned int numb_transitions = getNumbTransitions(aut, ignoreLeafRules);
    unsigned int numb_symbols = getNumbSymbols(aut, ignoreLeafRules);

    if (numb_states == 0 || numb_symbols == 0)
        transDens = 0;
    else
        transDens = (float) numb_transitions / ((float) numb_states * (float) numb_symbols);

    return transDens;
}

float getTransitionDensity(const AutData& autData, bool ignoreLeafRules)
{
    return getTransitionDensity(getAut(autData), ignoreLeafRules);
}

float getTransitionDensity(unsigned int numb_states, unsigned numb_symbols,
                           unsigned int numb_transitions)
{
    float transDens = 0;

    if (numb_states == 0 || numb_symbols == 0)
        transDens = 0;
    else
        transDens = (float) numb_transitions / ((float) numb_states * (float) numb_symbols);

    return transDens;
}

string getTransitionDensity_str(const Automaton& aut, bool ignoreLeafRules)
{
    return std::to_string(getTransitionDensity(aut, ignoreLeafRules));
}

vector<bool> getIsFinal(const Automaton& aut, const unsigned int n) {
    vector<bool> isFinal(n);

    finalStateSet f = aut.GetFinalStates();

    for(unsigned int i=0; i<n; i++)
        isFinal[i] = f.count(i);	/* count returns '1' if i is found in f and '0' otherwise. */

    return isFinal;
}

bool* getIsFinal2(const Automaton& aut, const unsigned int n) {
    bool* isFinal = new bool[n];

    finalStateSet f = aut.GetFinalStates();

    for(unsigned int i=0; i<n; i++)
        isFinal[i] = f.count(i);	/* count returns '1' if i is found in f and '0' otherwise. */

    return isFinal;
}

unsigned int getNumbLeafSymbols(const Automaton& aut)
{
    return (getLeafSymbols(aut)).size();
}

/* Returns the number of symbols in the alphabet of the automaton.
 * This function appears to not always work correctly. */
unsigned int getNumbSymbols(const Automaton& aut, bool ignoreLeafSymbols)
{
    unordered_set<symbol> symb;
    for (const transition& trans : aut)
        symb.insert(trans.GetSymbol());

    unsigned int numb = symb.size();

    if (ignoreLeafSymbols)
        return numb - getNumbLeafSymbols(aut);
    else
        return numb;
}

unsigned int getNumbSymbols(const AutData& autData, bool ignoreLeafSymbols)
{
    return getNumbSymbols(getAut(autData), ignoreLeafSymbols);
}

float getAvgRank(const Automaton& aut)
{
    // Get a pointer to the alphabet of type onTheFlyAlphabet which is being used already
    // by 'aut'.
    std::shared_ptr<onTheFlyAlphabet> alph;
    if (!(alph = std::dynamic_pointer_cast<onTheFlyAlphabet>(aut.GetAlphabet())))
    {	// The alphabet should be ExplicitTreeAut::onTheFlyAlphabet,
        // if it isn't then throw 'false'.
        assert(false);
    }

    typerank ranks = 0;
    size_t c = 0;
    for (const auto stringSymbolPair : alph->GetSymbolDict())
    {
        c++;
        ranks += stringSymbolPair.first.rank;
    }

    if (c==0)
        return 0;
    else
        return (float) ranks / (float) c;

}

float getAvgRank(const AutData& autData)
{
    return getAvgRank(getAut(autData));
}

state getGreatestUsedState(const Automaton& aut)
{
    state p=0;

    for (const lv_transition& trans : aut)
    {
        if (trans.GetParent() > p)
            p = trans.GetParent();

        for (const state& s : trans.GetChildren())
        {
            if (s > p)
                p = s;
        }
    }

    return p;
}

state getGreatestUsedState(const AutData& aut)
{

    return getGreatestUsedState(getAut(aut));

}

symbol getGreatestUsedSymbol(const Automaton& aut)
{
    symbol s=0;

    for (const lv_transition& trans : aut)
    {
        if (trans.GetSymbol() > s)
            s = trans.GetSymbol();
    }

    return s;
}

symbol getGreatestUsedSymbol(const AutData& autData)
{
    return getGreatestUsedSymbol(getAut(autData));
}

/* Returns a vector that at each index i contains a vector of pairs:
 * the first element is a transition (bottom-up) of aut in which i is a child and the second
 * element is the position at each i appears among the children of that transition. */
vector<vector<pair<transition,size_t>> > obtainTransBotUp(const Automaton& aut, unsigned int numbStates) {
    vector<vector<pair<transition,size_t>> > trans_botup(numbStates);

    for (const lv_transition trans : aut) {
        vector<state> children = trans.GetChildren();
        unsigned int size = children.size();

        for (unsigned int i=0; i<size; i++)
            trans_botup.at(children.at(i)).push_back(std::make_pair(trans,i));

    }

    return trans_botup;
}

bool equiv(const Automaton& aut1, const Automaton& aut2)
{
    Automaton aut1_ = removeInitialState(aut1);
    Automaton aut2_ = removeInitialState(aut2);

    bool result1 = true;
    result1 = VATA::ExplicitTreeAut::CheckInclusion(aut1_, aut2_);
    bool result2 = false;
    result2 = VATA::ExplicitTreeAut::CheckInclusion(aut2_, aut1_);

    return (result1 && result2);
}

bool equiv(const AutData& autData1, const AutData& autData2)
{

    return equiv(getAut(autData1),getAut(autData2));

}

bool langIsEmpty(const Automaton& aut)
{
    Automaton empty;
    return equiv(empty,aut);
}

bool langIsEmpty(const AutData& autData)
{
    return langIsEmpty(getAut(autData));
}

tuple<AutData,stateDict> complement(const Automaton& aut1, float& time_c)
{
    Automaton aut1_ = removeInitialState(aut1);

    auto start_c = startTimer();
    Automaton aut2  = aut1_.Complement();
    time_c = elapsedSec(start_c);

    aut2 = addInitialState(aut2);

    tuple<AutData,stateDict> tup = dumpAndLoadAut(aut2); // due to a bug in libvata's complement operation, which erases the dictionary during the process

    return tup;
}

tuple<AutData,stateDict> complement(const Automaton& aut1)
{
    float time_c_dump;
    return complement(aut1,time_c_dump);
}

tuple<AutData,stateDict> complement(const AutData& autData1, float& time_c)
{
    Automaton aut1 = getAut(autData1);

    tuple<AutData,stateDict> tup = complement(aut1,time_c);

    return std::make_tuple(std::get<0>(tup), std::get<1>(tup));
}

tuple<AutData,stateDict> complement(const AutData& autData1)
{
    float time_c_dump;
    return complement(autData1,time_c_dump);
}

state getInitialState(const Automaton& aut)
{
    return getGreatestUsedState(aut);
}

/* String and IO functions */

string transitionToString(const lv_transition& trans)
{
    return "<state=" + std::to_string(trans.GetParent()) + ", symbol="
    + std::to_string(trans.GetSymbol()) + ", children="
    + Convert::ToString(trans.GetChildren()) + ">";
}

/* Given a symbol in its internal representation, the function returns
 * its external representation. */
symbolExt translateSymbol(const Automaton& aut, symbol symb_int){

    symbolExt symb_ext = aut.GetAlphabet()->GetSymbolBackTransl()->operator()(symb_int);

    return symb_ext;
}

/* Given a state in its internal representation, the function returns
 * its external representation. */
stateExt translateState(const Automaton& aut, const stateDict& dict, const state s_int)
{

    std::basic_string<char> s_ext;

    state initialState = getInitialState(aut);
    if (s_int == initialState)
        s_ext = "ψ";
    else
        s_ext = dict.TranslateBwd(s_int);

    return s_ext;
}

/* Returns the number of transitions in the given automaton.
 * All our measures count the initial transitions
 * as ordinary transitions, therefore the default value for ignoreInitislSt is false. */
unsigned int getNumbTransitions(const Automaton& aut, bool ignoreLeafRules)
{
    unsigned int c = 0;
    for (const lv_transition& trans : aut)
    {
        if (!isALeafTransition(trans, aut))
            c++;
        else
            if (!ignoreLeafRules)
                c++;
    }

    return c;
}

/* This function has the same result as calling getNumbTransitions with flag ignoreLeafRules=false */
unsigned int getNumbTransitions(const Automaton& aut)
{
    unsigned int c = 0;
    for (const lv_transition& trans : aut)
        c++;

    return c;
}

unsigned int getNumbTransitions(const AutData& autData)
{
    return getNumbTransitions(getAut(autData));
}

unsigned int getNumbTransitions(const AutData& autData, bool ignoreLeafRules)
{
    return getNumbTransitions(getAut(autData), ignoreLeafRules);
}

string getNumbTransitions_str(const AutData& autData, bool ignoreLeafRules)
{
    return std::to_string(getNumbTransitions(autData,ignoreLeafRules));
}

string getNumbTransitions_str(const Automaton& aut, bool ignoreLeafRules)
{
    return std::to_string(getNumbTransitions(aut,ignoreLeafRules));
}

bool isALeafTransition(const lv_transition& trans, const Automaton& aut)
{
    state initialState = /*(initialSt < 0) ?*/ getInitialState(aut) /*: initialSt*/;
    vector<state> initial_state_children = {initialState};

    return (trans.GetChildren() == initial_state_children);
}

unsigned int getNumbLeafTransitions(const Automaton& aut)
{
    unsigned int c = 0;
    // This assumes that the explicit initial state has been added to the automaton,
    // which necessarily happens except when the input automaton had no leaf-rules
    // (in such a case, discarding the automaton is advisable).
    for (const lv_transition& trans : aut)
    {
        if (isALeafTransition(trans, aut))
            c++;
    }

    return c;
}

vector<symbol> getLeafSymbols(const Automaton& aut)
{
    vector<symbol> leaf_symbols;
    state initialState = getInitialState(aut);
    // This assumes that the explicit initial state has been added to the automaton,
    // which necessarily happens except when the input automaton had no leaf-rules
    // (in such a case, discarding the automaton is advisable).
    vector<state> initial_state_children = {initialState};
    for (const lv_transition& trans : aut)
    {
        if (trans.GetChildren() == initial_state_children)
            leaf_symbols.push_back(trans.GetSymbol());
    }

    return removeDuplicates(leaf_symbols);
}

/* Computes a copy of the given automaton. This will be a full copy if
 * deep_copy=true (the default). If it is false, it will possibly be a shallow copy (the copy
 * constructor for Automaton will be used if defined). */
Automaton copyAut(const Automaton& old_aut, bool deep_copy)
{
    Automaton new_aut;

    if (!deep_copy)     // Then use the user-defined object copy constructor.
        new_aut = old_aut;
    else
    {
        finalStateSet f = old_aut.GetFinalStates();
        for (const state& s : f)
        {
            new_aut.SetStateFinal(s);
        }

        const abstrAlphabet& alpha = old_aut.GetAlphabet();
        abstrAlphabet newAlpha = abstrAlphabet(new onTheFlyAlphabet(static_cast<const onTheFlyAlphabet&>(*alpha)));
        new_aut.SetAlphabet(newAlpha);

        for (const lv_transition& trans : old_aut)
            new_aut.AddTransition(trans.GetChildren(), trans.GetSymbol(), trans.GetParent());
    }

    return new_aut;
}

AutData copyAut(const AutData& old_autData, bool deep_copy)
{

    Automaton new_aut = copyAut(getAut(old_autData), deep_copy);

    return wrapAutData(new_aut,getRanks(old_autData));

}

/* Returns a copy of the given automaton but without any transition. */
Automaton copyAutWithoutTrans(const Automaton& old_aut)
{
    Automaton new_aut/*, old_aut = getAut(old_autData)*/;

    finalStateSet f = old_aut.GetFinalStates();
    for (const state& s : f)
    {
        new_aut.SetStateFinal(s);
    }

    const abstrAlphabet& alpha = old_aut.GetAlphabet();
    abstrAlphabet newAlpha = abstrAlphabet(new onTheFlyAlphabet(static_cast<const onTheFlyAlphabet&>(*alpha)));
    new_aut.SetAlphabet(newAlpha);

    return new_aut;
}

AutData copyAutWithoutTrans(const AutData& old_autData)
{

    Automaton new_aut = copyAutWithoutTrans(getAut(old_autData));

    return wrapAutData(new_aut,getRanks(old_autData));
}

typerank getGreatestRank(const vector<typerank>& ranks)
{
    return *std::max_element(ranks.begin(),ranks.end());
}

stateDiscontBinaryRelation computeLVDwSimulation(const Automaton& aut, const unsigned int numb_states) {
    simParam sp;
    sp.SetRelation(simParam::e_sim_relation::TA_DOWNWARD);
    sp.SetNumStates(numb_states);
    stateDiscontBinaryRelation sim = aut.ComputeSimulation(sp);

    return sim;
}

unsigned int getSizeLVStateBinRelation(stateDiscontBinaryRelation& binRel, unsigned int numb_states) {
    unsigned int c = 0;

    for (unsigned int p=0; p<numb_states; p++)
        for (unsigned int q=0; q<numb_states; q++) {
            if (binRel.get(p,q))
                c++;
        }

    return c;
}

void measureSizeTA(string filename, bool countInitialState, bool countInitialTransitions, bool output_human)
{
    AutData autData = parseFromFile(filename);
    unsigned int numb_states = getNumbUsedStates(autData, !countInitialState);
    unsigned int numb_trans = getNumbTransitions(autData, !countInitialTransitions);
    unsigned int numb_symb = getNumbSymbols(autData, !countInitialTransitions);
    float transDens = getTransitionDensity(numb_states, numb_symb, numb_trans);

    if (output_human)
        outputText("Numb. states: " + std::to_string(numb_states) + "\tNumb. symbols: "
                   + std::to_string(numb_symb) + "\tNumb. transitions: " + std::to_string(numb_trans)
                   + "\tTrans. density: " + convert2DecStr(transDens) + "\n");
    else
        outputText(std::to_string(numb_states) + "\t" + std::to_string(numb_symb) + "\t"
                   + std::to_string(numb_trans) + "\t" + convert2DecStr(transDens) + "\t");
}

bool statesIntersect(const vector<state>& vec1, const vector<state>& vec2)
{
    return (std::find_first_of(vec1.begin(), vec1.end(), vec2.begin(), vec2.end()) != vec1.end());
}

vector<transition> getTransFromStateBy(const state& p, const symbol& s, const Automaton& aut)
{
    vector<transition> trans_from_p_by_s;

    for (const transition& trans : aut[p])
    {
        if (trans.GetSymbol() == s)
            trans_from_p_by_s.push_back(trans);
    }

    return trans_from_p_by_s;
}

/* Returns the number of transitions from a state that share the symbol with at least another
 * transition also from that state. */
unsigned int getNumbNonDetTransFrom(const state& p, const Automaton& aut, bool ignoreLeafRules)
{
    unsigned int n_trans = 0;

    set<symbol> symbols_from_p = getSymbolsFrom(aut, p, ignoreLeafRules);
    for (const symbol& s : symbols_from_p)
    {
        unsigned int n_trans_from_p_by_s = (getTransFromStateBy(p, s, aut)).size();
        if (n_trans_from_p_by_s >= 2)
            n_trans += n_trans_from_p_by_s;
    }

    return n_trans;
}

/* Returns a tuple containing two measures of the non-determinism of the given automaton:
 * 1) the number of states (%) from which there are non-determisitic transitions (top-down),
 *    i.e., at least 2 transitions with the same symbol.
 * 2) the number of transitions (%) for which there is another transition from the same state
 *    and with the same symbol.  */
tuple<float, float> measureNonDeterminism(const Automaton& aut, unsigned int numb_transitions)
{
    unsigned int n_states = 0;
    stateSet states = getUsedStates(aut);
    unsigned int n_states_total = /*getNumbUsedStates(aut, true)*/ states.size() - 1;
    unsigned int n_trans = 0;
    unsigned int n_trans_total = (numb_transitions==0) ? getNumbTransitions(aut, true) : numb_transitions;

    for (const state& s : states)
    {
        unsigned int n_nonDetTrans = getNumbNonDetTransFrom(s, aut);

        if (n_nonDetTrans >= 2)
            n_states++;

        n_trans += n_nonDetTrans;
    }

    float n_states_perc = (n_states_total == 0)? 0.0 : ((float) n_states / (float) n_states_total) * 100.0;
    float n_trans_perc  = (n_trans_total == 0)?  0.0 : ((float) n_trans / (float) n_trans_total) * 100.0;

    return std::make_tuple(n_states_perc, n_trans_perc);
}

tuple<float, float> measureNonDeterminism(const AutData& autData, unsigned int numb_transitions)
{
    return measureNonDeterminism(getAut(autData), numb_transitions);
}

void measureNonDeterminism_ui(string filename, bool output_human)
{
    AutData autData = parseFromFile(filename);
    tuple<float,float> nd_tup = measureNonDeterminism(autData);

    if (output_human)
        outputText("ND_states: " + convert2DecStr(std::get<0>(nd_tup))
                   + "%\tND_trans: " + convert2DecStr(std::get<1>(nd_tup)) + "%\n");
    else
        outputText(convert2DecStr(std::get<0>(nd_tup)) + "\t" + convert2DecStr(std::get<1>(nd_tup)) + "\t");
}

bool measureTO_aux(const vector<state>& vec1, const vector<state>& vec2)
{
    if (vec1.size() != vec2.size())
        exit_with_error("Function foo received two vectors of different size.\n");

    for (unsigned int i=0; i<vec1.size(); i++)
        if (vec1.at(i) == vec2.at(i)) return true;

    return false;
}

/* Measures the overlap of transitions in the given automaton and returns a tuple as follows:
 * - the first element is the number of transitions (%) for which at least one of their children states
 * is also part of another transition with the same parent and symbol and at that same position;
 * - the second element is an average of how many (%) children states of a transition that overlaps
 * are also children states of a different transition with the same parent and symbol (maintaining their positions).
 * - the third element gives an average of, for each transition that overlaps, how many times each of its children
 * states appears at that same position in an additional transition with the same parent and symbol.
*/
vector<float> measureTransOverlaps(const Automaton& aut)
{
    std::set<transition> transWhichOverlap;     // Using a set forces that each transition shall be counted only once.

    for (const transition& trans1 : aut)
    {
        for (const transition& trans2 : aut)
        {
            if (trans2==trans1)
                continue;
            if (trans1.GetParent()==trans2.GetParent() && trans1.GetSymbol()==trans2.GetSymbol() && measureTO_aux(trans1.GetChildren(), trans2.GetChildren()))
            {
                transWhichOverlap.insert(trans2);
            }
        }
    }

    unsigned int numb_transitions = getNumbTransitions(aut, true);
    float avg_transWhichOverlap = ((float) transWhichOverlap.size() / (float) numb_transitions) * (float) 100;

    float avg = 0, n = 0;
    for (std::set<transition>::iterator it1=transWhichOverlap.begin(); it1!=transWhichOverlap.end(); ++it1)
    {
        size_t numb_children = (*it1).GetChildren().size(), c = 0;
        if (numb_children==0) continue;

        for (unsigned int i=0; i<numb_children; i++)
        {
            for (std::set<transition>::iterator it2=transWhichOverlap.begin(); it2!=transWhichOverlap.end(); ++it2)
            {
                if (it2==it1)
                    continue;
                if ((*it1).GetParent()==(*it2).GetParent() && (*it1).GetSymbol()==(*it2).GetSymbol() && (*it1).GetChildren().at(i)==(*it2).GetChildren().at(i))
                {
                    c++;
                    break;
                }
            }
        }

        avg += (float) c / (float) numb_children;
        n++;
    }
    if (n > 0)
        avg = (avg / (float) n) * (float) 100;
    else
        avg = 0;
    float avg1 = avg;

    avg = 0;
    n = 0;
    for (std::set<transition>::iterator it1=transWhichOverlap.begin(); it1!=transWhichOverlap.end(); ++it1)
    {
        size_t numb_children = (*it1).GetChildren().size(), c = 0;
        if (numb_children==0) continue;

        for (unsigned int i=0; i<numb_children; i++)
        {
            for (std::set<transition>::iterator it2=transWhichOverlap.begin(); it2!=transWhichOverlap.end(); ++it2)
            {
                if (it2==it1)
                    continue;
                if ((*it1).GetParent()==(*it2).GetParent() && (*it1).GetSymbol()==(*it2).GetSymbol() && (*it1).GetChildren().at(i)==(*it2).GetChildren().at(i))
                {
                    c++;
                }
            }
        }
        avg += (float) c / (float) numb_children;
        n++;
    }
    if (n > 0)
        avg = (avg / (float) n);
    else
        avg = 0;
    float avg2 = avg;

    vector<float> result = {avg_transWhichOverlap, avg1, avg2};

    return result;
}

vector<float> measureTransOverlaps(const AutData& autData)
{
    return measureTransOverlaps(getAut(autData));
}

string measureTransOverlaps_str2Dec(const Automaton& aut)
{
    vector<float> overlaps = measureTransOverlaps(aut);
    return "(" + convert2DecStr(overlaps.at(0)) + ", " +
            convert2DecStr(overlaps.at(1)) + ", " +
            convert2DecStr(overlaps.at(2)) + ")";
}

string measureTransOverlaps_str2Dec(const AutData& autData)
{
    return measureTransOverlaps_str2Dec(getAut(autData));
}

void measureTransOverlaps_ui(string filename, bool output_human)
{
    AutData autData = parseFromFile(filename);
    vector<float> overlaps = measureTransOverlaps(autData);

    if (output_human)
        outputText("TOL_1: " + convert2DecStr(overlaps.at(0))
                   + "%\tTOL_2: " + convert2DecStr(overlaps.at(1))
                   + "%\tTOL_3: " + convert2DecStr(overlaps.at(2)) + "\n");
    else
        outputText(convert2DecStr(overlaps.at(0)) + "\t" +
                    convert2DecStr(overlaps.at(1)) + "\t" +
                    convert2DecStr(overlaps.at(2)) + "\t");

}

/* Returns the value of numbStates(smaller) / numbStates(larger) in %. */
float measureStatesReduction(const Automaton& smaller, const Automaton& larger, bool ignoreInitialSt)
{
    return measureStatesReduction(getNumbUsedStates(smaller,ignoreInitialSt), getNumbUsedStates(larger,ignoreInitialSt));
}

float measureStatesReduction(unsigned int q_smaller, unsigned int q_larger)
{
    float states_red;

    if (q_larger == 0)
        states_red = 100;
    else
        states_red = ((float) q_smaller / (float) q_larger) * (float) 100;

    return states_red;
}

float measureStatesReduction(const AutData& smaller, const AutData& larger, bool ignoreInitialSt)
{
    return measureStatesReduction(getAut(smaller), getAut(larger), ignoreInitialSt);
}

float measureTransitionsReduction(const Automaton& smaller, const Automaton& larger, bool ignoreInitialSt)
{
    return measureTransitionsReduction(getNumbTransitions(smaller,ignoreInitialSt), getNumbTransitions(larger,ignoreInitialSt));
}

float measureTransitionsReduction(unsigned int delta_smaller, unsigned int delta_larger)
{
    float transitions_red;

    if (delta_larger == 0)
        transitions_red = 100;
    else
        transitions_red = ((float) delta_smaller
                           / (float) delta_larger) * (float) 100;

    return transitions_red;
}

float measureTransitionsReduction(const AutData& smaller, const AutData& larger, bool ignoreInitialSt)
{
    return measureTransitionsReduction(getAut(smaller), getAut(larger),ignoreInitialSt);
}

float measureTransDensReduction(float transDens_smaller, float transDens_larger)
{
    if (transDens_larger <= 0.0)
        return 0.0;

    return (transDens_smaller / transDens_larger) * (float) 100;
}

float measureTransDensReduction(const Automaton& smaller, const Automaton& larger, bool ignoreInitialSt)
{
    float transDens_smaller = getTransitionDensity(smaller,ignoreInitialSt);
    float transDens_larger  = getTransitionDensity(larger,ignoreInitialSt);

    return measureTransDensReduction(transDens_smaller, transDens_larger);
}

float measureTransDensReduction(const AutData& smaller, const AutData& larger, bool ignoreInitialSt)
{
    return measureTransDensReduction(getAut(smaller), getAut(larger),ignoreInitialSt);
}

/* Converts a float to a string representation containing 2 decimal values. */
string convert2DecStr(float f)
{
    return (boost::format("%.2f") % f).str();
}



void printAut(const Automaton& aut, stateDict& sDict)
{
    Automaton aut2 = removeInitialState(aut);
    outputText(autToStringTimbuk(aut2,sDict));
}

void printAut(const AutData& autData, stateDict& sDict)
{
    printAut(getAut(autData), sDict);
}

void printAut(const Automaton& aut)
{
    Automaton aut2 = removeInitialState(aut);
    outputText(autToStringTimbuk(aut2));
}

void printAut(const AutData& autData)
{
    printAut(getAut(autData));
}


void printAutData(const Automaton& aut, bool printTransOverlap) {
    /* Print the used states. */
    stateSet states = getUsedStates(aut);
    std::cout << "Q contains " << states.size() << " (used) states and ";
    std::cout << "F contains " << aut.GetFinalStates().size() << " final states. ";
    std::cout << "Sigma contains " << getNumbSymbols(aut) << " symbols "
              << "and their average rank is " << getAvgRank(aut) << ". ";
    std::cout << "Delta contains " << getNumbTransitions(aut) << " transitions and the transition density is "
              << getTransitionDensity(aut) << ". ";
    if (printTransOverlap)
    {
        vector<float> transOverlaps = measureTransOverlaps(aut);
        std::cout << transOverlaps.at(0)
                  << "% of the transitions overlap with some other transition (i.e., at least one of their children states is also part, at the same position, of another transition with the same parent and symbol). On average, for each transition that overlaps, the percentage of children states that are thus shared with a different transition "
                  << transOverlaps.at(1)
                  << "%. And, on average, for each transition that overlaps, each of its children states appears, at that same position, in "
                  << transOverlaps.at(2) << " additional transitions with the same parent and symbol. ";
    }
    std::cout << "\n";
}
