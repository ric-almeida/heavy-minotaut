
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Objects of the classes declared in this header file are used to keep record *
 *  of measures during sequences of tests (see, for e.g., the functions under       *
 *  src/testers/). In some cases they also keep the current averages of certain     *
 *  measures.                                                                       *
 * 																					*
 ************************************************************************************/

#ifndef STATISTICALRESULTS_H
#define STATISTICALRESULTS_H

#include "common.hh"
#include "automatonHelper.hh"


/* Objects of this class store general data of the experiments, as measures of the automata given from input. */
class MetaData
{
    public:
        unsigned int total = 0;
        float initial_avg_q = 0;        // The average number of states in the automata prior to any reduction.
        float initial_avg_delta = 0;    // The average number of transitions in the automata prior to any reduction.
        float initial_avg_sigma = 0;
        float initial_avg_ranking = 0;
        float initial_avg_dens = 0;     // The average transition density in the automata prior to any reduction.
        unsigned int initial_greatest_q = 0;    // The average number of states in the automata prior to any reduction.
        unsigned int initial_greatest_delta = 0;    // The average number of transitions in the automata prior to any reduction.
        float initial_greatest_sigma = 0;
        float initial_greatest_ranking = 0;
        float initial_greatest_dens = 0;     // The average transition density in the automata prior to any reduction.
        vector<float> initial_avg_transOverlap = {};
        vector<float> initial_greatest_transOverlap = {};
        MetaData();
        void inc();
        void updateInitialAvg(unsigned int q, unsigned int delta, unsigned int sigma = 0, float avg_ranking = 0.0, float dens = 0.0, vector<float> transOverlap = {});
        void updateInitialAvg(const Automaton& aut);
        void updateInitialAvg(const AutData& autData);
        void checkInitialGreatest(unsigned int q, unsigned int delta, unsigned int sigma, float ranking, float dens, vector<float> transOverlap = {});
        string initial_avg_q_str2Dec();
        string initial_avg_delta_str2Dec();
        string initial_avg_transDens_str2Dec();
        string initial_avg_transOverlap_str();
};

/* An object of this class stores data relative to a specific kind of test. */
class TestData
{
    public:
        unsigned int total = 0;
        unsigned int numb_relations = 0;
        float VERY_LARGE_NUMB = 999999;     /* Must be defined as public and before the declarations that make use of it. */
        float greatest_q_reduction = VERY_LARGE_NUMB;
        float greatest_delta_reduction = VERY_LARGE_NUMB;
        float greatest_transDens_reduction = VERY_LARGE_NUMB;
        float avg_q_reduction = 0;              // on average, how many of the states (%) are still present in the automata after the reduction
        float avg_delta_reduction = 0;          // on average, how many of the transitions (%) are still present in the automata after the reduction
        float avg_transDens_reduction = 0;          // on average, how does the transitions density before the reduction compare to that after it.
        float avg_q_size = 0;
        float avg_delta_size = 0;
        float avg_transDens = 0;
        float avg_time = 0;
        vector<float> avg_sizes_relations;
        vector<float> avg_times_relations;
        vector<float> avg_times_quotientings;
        vector<float> avg_times_prunings;
        float avg_time_consCheck = 0;           // How long it took to check if the reduced aut was equivalent to the initial one.
        vector<float> avg_numbs_refinements;     // Number of matrix-refinements necessary to compute the relation. E.g., 1.5 means a loop and a half in the matrix.
        vector<unsigned int> q_reduction_buckets;
        vector<unsigned int> delta_reduction_buckets;
        vector<unsigned int> transDens_reduction_buckets;
        TestData();
        TestData(unsigned int numb_relations, unsigned int numb_quotientings, unsigned int numb_prunings);
        void inc();
        void set(unsigned int numb_relations, unsigned int numb_quotientings, unsigned int numb_prunings);
        void updateAvgSizesRels(vector<unsigned int> cards);
        void updateAvgNumbsRefs(vector<float> numbs_refs);
        void updateAvgTime(float time);
        void updateAvgTimes(vector<float> times_relations, vector<float> times_quotientings, vector<float> times_prunings, float time_consCheck);
        void updateAvgTimes(vector<float> times_relations, vector<float> times_quotientings, vector<float> times_prunings);
        void checkGreatestReductions(float q_red, float delta_red, float transDens_red);
        void updateAvgReductions(float q_red, float delta_red, float transDens_red = 0.0);
        void updateAvgReductions(const AutData& autData_smaller, const AutData& autData_larger);
        void updateAvgSizes(unsigned int q, unsigned int delta, float transDens = 0.0);
        void updateAvgSizes(const AutData& autData);
        void updateReductionBuckets(float q_red, float delta_red, float transDens_red);
        void printReductionBuckets(const string filename);
        bool isEmpty(const TestData& testData);
        string avg_q_str2Dec();
        string avg_delta_str2Dec();
        string avg_transDens_str2Dec();
        string avg_q_red_str2Dec();
        string avg_delta_red_str2Dec();
        string avg_transDens_red_str2Dec();
        string avg_time_str2Dec();
        vector<string> avg_sizes_relations_str2Dec();
};


/* Records in objects of this class are relative to tests that were aborted due to a timeout. */
class Timeout
{
    public:

        unsigned int total = 0;
        unsigned int VERY_LARGE_NUMB = 999999;
        unsigned int smallest_q = VERY_LARGE_NUMB;
        unsigned int smallest_delta = VERY_LARGE_NUMB;
        float smallest_dens = VERY_LARGE_NUMB;

        void inc();
        void checkSmallest(unsigned int q, unsigned int delta, float dens);
};


class QueriesCounter
{
    public:
        unsigned int numb_inserts = 0;
        unsigned int numb_sucLookups = 0;
        unsigned int numb_unsucLookups = 0;

        void incNumbInserts();
        void incNumbSucLookups();
        void incNumbUnsucLookups();
};

extern MetaData emptyMetaData;
extern TestData emptyTestData;
extern Timeout emptyTimeout;

extern int NO_OUT;
extern int LOG;
extern int STD_OUT_HUMAN;
extern int STD_OUT_MACHINE;

void log_time(string log_filename, float time);

void log_autSizes(string log_filename, const Automaton& aut, bool ignoreInitialSt = false);
void log_autSizes(string log_filename, const AutData& autData, bool ignoreInitialSt = false);
void log_autSizes(string log_filename,
                  unsigned int q, unsigned int delta/*, float time = 0.0*/);

void log_autTransOverlap(string log_filename, const AutData& autData);

void log_autReduction(string log_filename,
                      const Automaton& aut_smaller, const Automaton& aut_larger, float time = 0.0);
void log_autReduction(string log_filename, const AutData& autData_smaller, const AutData autData_larger, float time = 0.0);
void log_autReduction(string log_filename, float q_red, float delta_red, float transDens_red, float time = 0.0);
void log_autReduction(string log_filename, float q_red, float delta_red);
void log_autReduction(string log,
                      unsigned int q_smaller, unsigned int dt_smaller,
                      unsigned int q_larger, unsigned int dt_larger);

void log_autNonDet(string log_filename, const Automaton& aut, unsigned int numb_transitions = 0);
void log_autNonDet(string log_filename, const AutData& autData, unsigned int numb_transitions = 0);


#endif // STATISTICALRESULTS_H
