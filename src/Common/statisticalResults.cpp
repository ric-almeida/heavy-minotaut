
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Objects of the classes defined in this file are used to keep record         *
 *  of measures during sequences of tests (see, for e.g., the functions under       *
 *  src/testers/). In some cases they also keep the current averages of certain     *
 *  measures.                                                                       *
 * 																					*
 ************************************************************************************/

#include "statisticalResults.hh"

MetaData emptyMetaData  =  MetaData();
TestData emptyTestData  =  TestData();
Timeout  emptyTimeout   =  Timeout();

int NO_OUT = 0;
int LOG = 1;
int STD_OUT_HUMAN = 2;
int STD_OUT_MACHINE = 3;

MetaData::MetaData() : initial_avg_transOverlap(3,0), initial_greatest_transOverlap(3,0)
{
}

void MetaData::inc()
{
    this->total++;
}

void MetaData::updateInitialAvg(unsigned int q, unsigned int delta, unsigned int sigma, float avg_ranking, float dens, vector<float> transOverlap)
{
    this->initial_avg_q       = ( this->initial_avg_q*(this->total-1) + q )             / (float) this->total;

    this->initial_avg_delta   = ( this->initial_avg_delta*(this->total-1) + delta )     / (float) this->total;

    if (sigma > 0)
        this->initial_avg_sigma   = ( this->initial_avg_sigma*(this->total-1) + sigma )     / (float) this->total;

    if (avg_ranking > 0.0)
        this->initial_avg_ranking = ( this->initial_avg_ranking*(this->total-1) + avg_ranking ) / (float) this->total;

    if (dens > 0.0)
        this->initial_avg_dens    = ( this->initial_avg_dens*(this->total-1) + dens )       / (float) this->total;

    if (!transOverlap.empty())
        for (unsigned int i=0; i<transOverlap.size(); i++)
        {
            (this->initial_avg_transOverlap).at(i)
                    = ((this->initial_avg_transOverlap).at(i)*(this->total-1) + (transOverlap).at(i) ) / (float) this->total;
        }
}

void MetaData::updateInitialAvg(const Automaton& aut)
{
    this->updateInitialAvg(getNumbUsedStates(aut), getNumbTransitions(aut),
                           getNumbSymbols(aut), getAvgRank(aut), getTransitionDensity(aut),
                           measureTransOverlaps(aut));
}

void MetaData::updateInitialAvg(const AutData& autData)
{
    this->updateInitialAvg(getAut(autData));
}

void MetaData::checkInitialGreatest(unsigned int q, unsigned int delta, unsigned int sigma, float ranking, float dens, vector<float> transOverlap)
{
    if (q > this->initial_greatest_q)
        this->initial_greatest_q = q;
    if (delta > this->initial_greatest_delta)
        this->initial_greatest_delta = delta;
    if (sigma > this->initial_greatest_sigma)
        this->initial_greatest_sigma = sigma;
    if (ranking > this->initial_greatest_ranking)
        this->initial_greatest_ranking = ranking;
    if (dens > this->initial_greatest_dens)
        this->initial_greatest_dens = dens;
    for (unsigned int i=0; i<transOverlap.size(); i++)
    {
        if ((transOverlap).at(i) > (this->initial_greatest_transOverlap).at(i))
            (this->initial_greatest_transOverlap).at(i) = (transOverlap).at(i);
    }
}

string MetaData::initial_avg_q_str2Dec()
{
    return convert2DecStr(this->initial_avg_q);
}

string MetaData::initial_avg_delta_str2Dec()
{
    return convert2DecStr(this->initial_avg_delta);
}

string MetaData::initial_avg_transDens_str2Dec()
{
    return convert2DecStr(this->initial_avg_dens);
}

string MetaData::initial_avg_transOverlap_str()
{
    return "(" + convert2DecStr(this->initial_avg_transOverlap.at(0)) + ", " +
                 convert2DecStr(this->initial_avg_transOverlap.at(1)) + ", " +
                 convert2DecStr(this->initial_avg_transOverlap.at(2)) + ")";
}


/* numb_relations = how many relations will be computed per test;
 * numb_quotientings = how many quotientings will be done per test;
 * numb_prunings = how many prunings will be done per test. */
TestData::TestData(unsigned int numb_relations, unsigned int numb_quotientings, unsigned int numb_prunings) :
    avg_sizes_relations(numb_relations,0), avg_times_relations(numb_relations,0), avg_times_quotientings(numb_quotientings,0),
    avg_times_prunings(numb_prunings,0), avg_numbs_refinements(numb_relations,0),
    q_reduction_buckets(10,0), delta_reduction_buckets(10,0), transDens_reduction_buckets(10,0)
{
    this->numb_relations = numb_relations;
}

TestData::TestData() :
    avg_sizes_relations(0), avg_times_relations(0), avg_times_quotientings(0), avg_times_prunings(0), avg_numbs_refinements(0),
    q_reduction_buckets(10,0), delta_reduction_buckets(10,0), transDens_reduction_buckets(10,0)
{
}

void TestData::set(unsigned int numb_relations, unsigned int numb_quotientings, unsigned int numb_prunings)
{
    this->avg_sizes_relations.assign(numb_relations,0);
    this->avg_times_relations.assign(numb_relations,0);
    this->avg_times_quotientings.assign(numb_quotientings,0);
    this->avg_times_prunings.assign(numb_prunings,0);
    this->avg_numbs_refinements.assign(numb_relations,0);
}

void TestData::inc()
{
    this->total++;
}

void TestData::updateAvgSizesRels(vector<unsigned int> cards)
{
    if ((avg_sizes_relations.empty() == false) && (cards.size() != avg_sizes_relations.size()))
        exit_with_error("TestData::updateAvgSize must receive a vector<unsigned int> of size " + avg_sizes_relations.size());

    for (unsigned int i=0; i<avg_sizes_relations.size(); i++)
        this->avg_sizes_relations[i] = ( this->avg_sizes_relations[i]*(this->total-1) + cards[i] ) / (float) this->total;
}

void TestData::updateAvgNumbsRefs(vector<float> numbs_refs)
{
    if (numbs_refs.size() != avg_numbs_refinements.size())
        exit_with_error("Success::updateAvgNumbsRefs must receive a vector<float> nums_refs of size " + avg_numbs_refinements.size());

    for (unsigned int i=0; i<numbs_refs.size(); i++)
        this->avg_numbs_refinements[i] = ( this->avg_numbs_refinements[i]*(this->total-1) + numbs_refs[i] ) / (float) this->total;
}

void TestData::updateAvgTimes(vector<float> times_relations, vector<float> times_quotientings, vector<float> times_prunings, float time_consCheck)
{
    if (times_relations.size() != avg_times_relations.size())
        exit_with_error("Success::updateAvgTime must receive a vector<size_t> times_relations of size " + avg_times_relations.size());
    if (times_prunings.size() != avg_times_prunings.size())
        exit_with_error("Success::updateAvgTime must receive a vector<size_t> times_prunings of size " + avg_times_prunings.size());
    if (times_quotientings.size() != avg_times_quotientings.size())
        exit_with_error("Success::updateAvgTime must receive a vector<size_t> times_quotientings of size " + avg_times_quotientings.size());

    for (unsigned int i=0; i<avg_times_relations.size(); i++)
        this->avg_times_relations[i] = ( this->avg_times_relations[i]*(this->total-1) + times_relations[i] ) / (float) this->total;

    for (unsigned int i=0; i<avg_times_quotientings.size(); i++)
        this->avg_times_quotientings[i] = ( this->avg_times_quotientings[i]*(this->total-1) + times_quotientings[i] ) / (float) this->total;

    for (unsigned int i=0; i<avg_times_prunings.size(); i++)
        this->avg_times_prunings[i] = ( this->avg_times_prunings[i]*(this->total-1) + times_prunings[i] ) / (float) this->total;

    this->avg_time_consCheck = ( this->avg_time_consCheck*(this->total-1) + time_consCheck ) / (float) this->total;
}

void TestData::updateAvgTimes(vector<float> times_relations, vector<float> times_quotientings, vector<float> times_prunings)
{
    if (times_relations.size() != avg_times_relations.size())
        exit_with_error("Success::updateAvgTime must receive a vector<size_t> times_relations of size " + avg_times_relations.size());
    if (times_prunings.size() != avg_times_prunings.size())
        exit_with_error("Success::updateAvgTime must receive a vector<size_t> times_prunings of size " + avg_times_prunings.size());
    if (times_quotientings.size() != avg_times_quotientings.size())
        exit_with_error("Success::updateAvgTime must receive a vector<size_t> times_quotientings of size " + avg_times_quotientings.size());

    for (unsigned int i=0; i<avg_times_relations.size(); i++)
        this->avg_times_relations[i]
            = ( this->avg_times_relations[i]*(this->total-1) + times_relations[i] ) / (float) this->total;

    for (unsigned int i=0; i<avg_times_quotientings.size(); i++)
        this->avg_times_quotientings[i]
            = ( this->avg_times_quotientings[i]*(this->total-1) + times_quotientings[i] ) / (float) this->total;

    for (unsigned int i=0; i<avg_times_prunings.size(); i++)
        this->avg_times_prunings[i]
            = ( this->avg_times_prunings[i]*(this->total-1) + times_prunings[i] ) / (float) this->total;

}

void TestData::checkGreatestReductions(float q_red, float delta_red, float transDens_red)
{
    if (q_red < this->greatest_q_reduction)
        this->greatest_q_reduction = q_red;
    if (delta_red < this->greatest_delta_reduction)
        this->greatest_delta_reduction = delta_red;
    if (transDens_red < this->greatest_transDens_reduction)
        this->greatest_transDens_reduction = transDens_red;
}

void TestData::updateAvgReductions(float q_red, float delta_red, float transDens_red)
{
    this->avg_q_reduction         = ( this->avg_q_reduction*(this->total-1) + q_red ) / (float) this->total;
    this->avg_delta_reduction     = ( this->avg_delta_reduction*(this->total-1) + delta_red ) / (float) this->total;
    if (transDens_red > 0.0)
        this->avg_transDens_reduction = ( this->avg_transDens_reduction*(this->total-1) + transDens_red ) / (float) this->total;
}

void TestData::updateAvgReductions(const AutData& autData_smaller, const AutData& autData_larger)
{
    float q_red          =  measureStatesReduction(autData_smaller, autData_larger/*, true*/);
    float delta_red      =  measureTransitionsReduction(autData_smaller, autData_larger/*, true*/);
    float transDens_red  =  measureTransDensReduction(autData_smaller, autData_larger/*, true*/);

    this->updateAvgReductions(q_red, delta_red, transDens_red);
}

void TestData::updateAvgSizes(unsigned int q, unsigned int delta, float transDens)
{
    this->avg_q_size      = ((float) ( this->avg_q_size*(this->total-1) + q )) / (float) this->total;
    this->avg_delta_size  = ((float) ( this->avg_delta_size*(this->total-1) + delta )) / (float) this->total;
    if (transDens > 0.0)
        this->avg_transDens   = ( this->avg_transDens*(this->total-1) + transDens ) / (float) this->total;
}

void TestData::updateAvgSizes(const AutData& autData)
{
    this->updateAvgSizes(getNumbUsedStates(autData), getNumbTransitions(autData),
                         getTransitionDensity(autData));
}

void TestData::updateAvgTime(float time)
{
    this->avg_time  =  ( this->avg_time*(this->total-1) + time ) / (float) this->total;
}

void TestData::updateReductionBuckets(float q_red, float delta_red, float transDens_red)
{
    if (q_red >= 100)     // >= instead of ==, to make the float comparison safe.
        this->q_reduction_buckets[9]++;
    else
        this->q_reduction_buckets[q_red / 10]++;

    if (delta_red >= 100)
        this->delta_reduction_buckets[9]++;
    else
        this->delta_reduction_buckets[delta_red / 10]++;

    if (transDens_red >= 100)
        this->transDens_reduction_buckets[9]++;
    else
        this->transDens_reduction_buckets[transDens_red / 10]++;
}

void printBucket(vector<unsigned int> bucket, const string filename)
{
    for (unsigned int i=0; i<9; i++)
        outputText(std::to_string(bucket[i]) + "\n", filename);
    outputText(std::to_string(bucket[9]) + "\n", filename);
}

void TestData::printReductionBuckets(const string filename)
{
    outputText("Reduction (%) - states: \n", filename);
    printBucket(this->q_reduction_buckets, filename);

    outputText("Reduction (%) - transitions: \n", filename);
    printBucket(this->delta_reduction_buckets, filename);

    outputText("Reduction (%) - transition density: \n", filename);
    printBucket(this->transDens_reduction_buckets, filename);
}

string TestData::avg_q_str2Dec()
{
    return convert2DecStr(this->avg_q_size);
}

string TestData::avg_delta_str2Dec()
{
    return convert2DecStr(this->avg_delta_size);
}

string TestData::avg_transDens_str2Dec()
{
    return convert2DecStr(this->avg_transDens);
}

string TestData::avg_q_red_str2Dec()
{
    return convert2DecStr(this->avg_q_reduction);
}

string TestData::avg_delta_red_str2Dec()
{
    return convert2DecStr(this->avg_delta_reduction);
}

string TestData::avg_transDens_red_str2Dec()
{
    return convert2DecStr(this->avg_transDens_reduction);
}

string TestData::avg_time_str2Dec()
{
    return convert2DecStr(this->avg_time);
}

vector<string> TestData::avg_sizes_relations_str2Dec()
{
    vector<string> vec(this->numb_relations);
    for (unsigned int i=0; i<this->avg_sizes_relations.size(); i++)
        vec.at(i) = convert2DecStr(this->avg_sizes_relations.at(i));

    return vec;
}

void Timeout::inc()
{
    this->total++;
}

void Timeout::checkSmallest(unsigned int q, unsigned int delta, float dens)
{
    if (q < this->smallest_q)
        this->smallest_q = q;
    if (delta < this->smallest_delta)
        this->smallest_delta = delta;
    if (dens < this->smallest_dens)
        this->smallest_dens = dens;
}


void QueriesCounter::incNumbInserts()
{
    this->numb_inserts++;
}

void QueriesCounter::incNumbSucLookups()
{
    this->numb_sucLookups++;
}

void QueriesCounter::incNumbUnsucLookups()
{
    this->numb_unsucLookups++;
}


void log_time(string log_filename, float time)
{
    string time_2DecVals = convert2DecStr(time);
    writeToFile(log_filename, time_2DecVals + "\t");
}

/* All our measures count the special initial state as a state and the initial transitions
 * as ordinary transitions, therefore the default value for ignoreInitislSt is false. */
void log_autSizes(string log_filename, const Automaton& aut, bool ignoreInitialSt)
{
    writeToFile(log_filename,
                getNumbUsedStates_str(aut,ignoreInitialSt) + "\t" +
                getNumbTransitions_str(aut,ignoreInitialSt) + "\t" +
                convert2DecStr(getTransitionDensity(aut,ignoreInitialSt)) + "\t");
}

void log_autSizes(string log_filename,
                  unsigned int q, unsigned int delta)
{
    writeToFile(log_filename,
                std::to_string(q) + "\t" +
                std::to_string(delta) + "\t" /*+
                std::to_string(getTransitionDensity(q,s,delta)) + "\t"*/);
}

void log_autSizes(string log_filename, const AutData& autData, bool ignoreInitialSt)
{
    log_autSizes(log_filename, getAut(autData), ignoreInitialSt);
}

void log_autNonDet(string log_filename, const Automaton& aut,
                   unsigned int numb_transitions)
{
    tuple<float,float> aut_nonDet = measureNonDeterminism(aut, numb_transitions);

    writeToFile(log_filename,
                convert2DecStr(std::get<0>(aut_nonDet)) + "\t" +
                convert2DecStr(std::get<1>(aut_nonDet)) + "\t");
}

void log_autNonDet(string log_filename, const AutData& autData,
                   unsigned int numb_transitions)
{
    log_autNonDet(log_filename, getAut(autData), numb_transitions);
}

void log_autTransOverlap(string log_filename, const AutData& autData)
{
    writeToFile(log_filename,
                measureTransOverlaps_str2Dec(autData) + "\t");
}

void log_autReduction(string log_filename,
                      const Automaton& aut_smaller, const Automaton& aut_larger, float time)
{
    writeToFile(log_filename,
                convert2DecStr(measureStatesReduction(aut_smaller, aut_larger)) + "\t" +
                convert2DecStr(measureTransitionsReduction(aut_smaller, aut_larger)) + "\t" +
                convert2DecStr(measureTransDensReduction(aut_smaller, aut_larger)) + "\t");

    if (time > 0.0) log_time(log_filename, time);
}

void log_autReduction(string log_filename,
                      float q_red, float delta_red, float transDens_red, float time)
{
    writeToFile(log_filename,
                convert2DecStr(q_red) + "\t" +
                convert2DecStr(delta_red) + "\t" +
                convert2DecStr(transDens_red) + "\t");

    if (time > 0.0) log_time(log_filename, time);
}

void log_autReduction(string log_filename,
                      float q_red, float delta_red)
{
    writeToFile(log_filename,
                convert2DecStr(q_red) + "\t" +
                convert2DecStr(delta_red) + "\t");
}

void log_autReduction(string log_filename,
                      const AutData& autData_smaller, const AutData autData_larger, float time)
{
    log_autReduction(log_filename, getAut(autData_smaller), getAut(autData_larger), time);
}

void log_autReduction(string log,
                      unsigned int q_smaller, unsigned int dt_smaller,
                      unsigned int q_larger, unsigned int dt_larger)
{
    float q_red = measureStatesReduction(q_smaller, q_larger);
    float dt_red = measureTransitionsReduction(dt_smaller, dt_larger);

    log_autReduction(log, q_red, dt_red/*, td_red*/);
}
