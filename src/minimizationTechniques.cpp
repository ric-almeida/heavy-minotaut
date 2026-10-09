
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Implementation file for the automata minimization procedures. It includes   *
 *  the Heavy algorithm (see README.txt/Publications).                              *
 * 																					*
 ************************************************************************************/

#include "minimizationTechniques.hh"

/* Encodings */
const unsigned int DW_LA_1ST_TIME         = 0;
const unsigned int UP_LA_WITH_ID_1ST_TIME = 1;
const unsigned int STRICT_UP_1_WITH_ID    = 2;
const unsigned int DW_LA_2ND_TIME         = 3;
const unsigned int UP_LA_WITH_ID_2ND_TIME = 4;
const unsigned int DW_1                   = 5;
const unsigned int UP_LA_WITH_DW_1        = 6;
const unsigned int numbRelations          = 7;

const unsigned int QUOT_WITH_DW_LA             = 0;
const unsigned int QUOT_WITH_UP_LA_WITH_ID_1ST = 1;
const unsigned int QUOT_WITH_UP_LA_WITH_ID_2ND = 2;
const unsigned int numbQuotientings            = 3;

const unsigned int PRUN_WITH_P_ID_AND_DW_LA             = 0;
const unsigned int PRUN_WITH_P_UP_1_WITH_ID_AND_DW_LA   = 1;
const unsigned int PRUN_WITH_P_UP_LA_WITH_DW_1_AND_DW_1 = 2;
const unsigned int numbPrunings                         = 3;


AutData removeDeadStates(const AutData& autData)
{
    const Automaton& aut_i = getAut(autData);

    Automaton aut_f = removeInitialState(aut_i);

    aut_f = aut_f.RemoveUselessStates();

    aut_f = addInitialState(aut_f);

    return wrapAutData(aut_f,getRanks(autData));
}


AutData applyQuotAndPrunDwLa(const AutData& autData_i,
                             unsigned int la_dw,
                             unsigned int greatest_state  = 0,
                             unsigned int greatest_symbol = 0,
                             string log_humanread_filename = "")
{

    const Automaton aut_i = getAut(autData_i);
    greatest_state  = !greatest_state  ? getGreatestUsedState(aut_i)  : greatest_state;
    greatest_symbol = !greatest_symbol ? getGreatestUsedSymbol(aut_i) : greatest_symbol;

    auto start   = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::high_resolution_clock::now() - start;

    //AutData aut = removeUselessStates(autData_i);

    /* Computing the dw-la sim. */
    outputText("Computing the dw-" + std::to_string(la_dw) + " simulation... \n", log_humanread_filename);
    vector<vector<bool> > dw_la;
    start = std::chrono::high_resolution_clock::now();
    dw_la = dw_simulation(autData_i, la_dw);
    elapsed = std::chrono::high_resolution_clock::now() - start;

    /* Quotient with dw-la sim. */
    start = std::chrono::high_resolution_clock::now();
    AutData aut_quot = quotientAutomaton(autData_i,transClosure(dw_la));
    elapsed = std::chrono::high_resolution_clock::now() - start;

    /* Prune with P(id,dw-la sim). */
    start = std::chrono::high_resolution_clock::now();
    AutData aut_prun = prune(aut_quot, generateIdRelation(greatest_state+1), dw_la, true);
    /* We must generate the id relation based on the number of states which existed before the quotienting,
     * since the quotienting does not force the reindexing of the states.
     * Alternatively, we could try to force this reindexation.
     * libvata provides a method for the reindexation of the states, but it doesn't appear to be preserving the lang.' */
    elapsed = std::chrono::high_resolution_clock::now() - start;

    return aut_prun;

}

AutData applyMinimizationSequence(const AutData& autData_i,
                                  unsigned int la_dw, unsigned int la_up,
                                  unsigned int greatest_state,
                                  unsigned int greatest_symbol,
                                  Time& timeout_start,
                                  TestData& testData,
                                  string log_humanread_filename)
{

    const Automaton aut_i = getAut(autData_i);
    greatest_state  = !greatest_state  ? getGreatestUsedState(aut_i)   : greatest_state;
    greatest_symbol = !greatest_symbol ? getGreatestUsedSymbol(aut_i)  : greatest_symbol;

    auto start   = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::high_resolution_clock::now() - start;

    vector<unsigned int> sizesRel(numbRelations);
    vector<float>        numbRefs(numbRelations);
    vector<float>        timesRel(numbRelations);
    vector<float>        timesQuot(numbQuotientings);
    vector<float>        timesPrun(numbPrunings);

    /* Common declarations. */
    float refin = 0;
    AutData aut = removeUselessStates(autData_i);

    /* Computing the dw-la sim. */
    if (log_humanread_filename != "")
        outputText("Computing the dw-" + std::to_string(la_dw) + " simulation... \n", log_humanread_filename);
    vector<vector<bool> > dw_la;
    start = std::chrono::high_resolution_clock::now();

    dw_la = dw_simulation(aut, la_dw, &refin, true, timeout_start);

    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesRel[DW_LA_1ST_TIME]
            = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    sizesRel[DW_LA_1ST_TIME] = getSizeOfRel(dw_la);

    numbRefs[DW_LA_1ST_TIME] = refin;
    if (log_humanread_filename != "") outputText("The dw-" + std::to_string(la_dw) + " simulation has "
               + std::to_string(sizesRel[DW_LA_1ST_TIME]) + " pairs ", log_humanread_filename);
    if (log_humanread_filename != "")
            outputText(" and took " + std::to_string(numbRefs[DW_LA_1ST_TIME]) + " matrix-refinements and "
                       + std::to_string(timesRel[DW_LA_1ST_TIME]) + " seconds to compute. ", log_humanread_filename);

    /* Quotient with dw-la sim. */
    start = std::chrono::high_resolution_clock::now();
    AutData aut_quot = quotientAutomaton(aut,transClosure(dw_la));
    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesQuot[QUOT_WITH_DW_LA] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    if (log_humanread_filename != "") outputText("The quotienting took " + std::to_string(timesQuot[QUOT_WITH_DW_LA])
               + " seconds ", log_humanread_filename);

    /* Prune with P(id,dw-la sim). */
    start = std::chrono::high_resolution_clock::now();
    AutData aut_prun = prune(aut_quot, generateIdRelation(getGreatestUsedState(aut)+1), dw_la, true);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesPrun[PRUN_WITH_P_ID_AND_DW_LA] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    if (log_humanread_filename != "")
        outputText("and the pruning took " + std::to_string(timesPrun[PRUN_WITH_P_ID_AND_DW_LA]) + " seconds. ",
                log_humanread_filename);

    AutData aut_ru = removeUselessStates(aut_prun);

    /* Computing the up-la(id) sim. */
    if (log_humanread_filename != "") outputText("\nComputing the up-" + std::to_string(la_up) + "(id) simulation... \n",
               log_humanread_filename);
    start = std::chrono::high_resolution_clock::now();
    vector<vector<bool> > up_la_with_id_1st_time =
            up_simulation(aut_ru, la_up, generateIdRelation(greatest_state+1), false,
                          greatest_state+1, greatest_symbol+1, &refin, timeout_start);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    sizesRel[UP_LA_WITH_ID_1ST_TIME]
            = getSizeOfRel(up_la_with_id_1st_time);
    numbRefs[UP_LA_WITH_ID_1ST_TIME] = refin;
    timesRel[UP_LA_WITH_ID_1ST_TIME] =
            std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    if (log_humanread_filename != "") outputText("The up-" + std::to_string(la_up) + "(id) simulation has "
              + std::to_string(sizesRel[UP_LA_WITH_ID_1ST_TIME]) + " pairs and it took "
              + std::to_string(numbRefs[UP_LA_WITH_ID_1ST_TIME]) + " matrix-refinements and "
              + std::to_string(timesRel[UP_LA_WITH_ID_1ST_TIME]) + " seconds to compute and ",
               log_humanread_filename);

    /* Quotienting with the up-la(id) sim the 1st time. */
    start = std::chrono::high_resolution_clock::now();
    AutData aut_quot2 = quotientAutomaton(aut_ru, transClosure(up_la_with_id_1st_time));
    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesQuot[QUOT_WITH_UP_LA_WITH_ID_1ST] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    if (log_humanread_filename != "")
        outputText("the quotienting took " + std::to_string(timesQuot[QUOT_WITH_UP_LA_WITH_ID_1ST]) + " seconds. ",
                   log_humanread_filename);

    /* Prune with P(strict up-la(id) sim, id) */
    up_la_with_id_1st_time = strictRel(up_la_with_id_1st_time);
    start = std::chrono::high_resolution_clock::now();
    AutData aut_prun2 = prune(aut_quot2, up_la_with_id_1st_time, generateIdRelation(greatest_state+1), false);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    float seconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    if (log_humanread_filename != "")
        outputText("The pruning with P(strict up-la(id) sim, id) took " + std::to_string(seconds) + " seconds. ",
               log_humanread_filename);

    AutData aut_ru2 = removeUselessStates(aut_prun2);

    /* Computing the strict up-1(id) sim. */
    if (log_humanread_filename != "") outputText("\nComputing the strict up-1(id) simulation... \n", log_humanread_filename);
    start = std::chrono::high_resolution_clock::now();
    vector<vector<bool> > strict_up_1_with_id
            = up_simulation_strict(aut_ru2, 1, generateIdRelation(greatest_state+1), false,
                                   greatest_state+1, greatest_symbol+1, &refin, timeout_start);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    sizesRel[STRICT_UP_1_WITH_ID] = getSizeOfRel(strict_up_1_with_id);
    numbRefs[STRICT_UP_1_WITH_ID] = refin;
    timesRel[STRICT_UP_1_WITH_ID] =
            std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    if (log_humanread_filename != "")
        outputText("The strict up-1(id) simulation has " + std::to_string(sizesRel[STRICT_UP_1_WITH_ID])
              + " pairs and the computation of the relation took "
              + std::to_string(numbRefs[STRICT_UP_1_WITH_ID]) + " matrix-refinements and "
              + std::to_string(timesRel[STRICT_UP_1_WITH_ID]) + " seconds. ", log_humanread_filename);

    /* Computing the dw-la sim again. */
    if (log_humanread_filename != "") outputText("\nComputing the dw-" + std::to_string(la_dw) + " simulation again... \n",
               log_humanread_filename);
    start = std::chrono::high_resolution_clock::now();

    dw_la = dw_simulation(aut_ru2, la_dw, &refin, true, timeout_start);

    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesRel[DW_LA_2ND_TIME] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    sizesRel[DW_LA_2ND_TIME] = getSizeOfRel(dw_la);
    numbRefs[DW_LA_2ND_TIME] = refin;
    if (log_humanread_filename != "") outputText("The dw-" + std::to_string(la_dw) + " simulation has " + std::to_string(sizesRel[DW_LA_2ND_TIME])
              + " pairs and took " + std::to_string(numbRefs[DW_LA_2ND_TIME])
              + " matrix-refinements and " + std::to_string(timesRel[DW_LA_2ND_TIME])
              + " seconds to compute. ",
              log_humanread_filename);

    /* Pruning with P(strict up-1(id) sim, dw-la sim) */
    start = std::chrono::high_resolution_clock::now();
    AutData aut_prun3 = prune(aut_ru2, strict_up_1_with_id, dw_la, false);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesPrun[PRUN_WITH_P_UP_1_WITH_ID_AND_DW_LA]
            = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    if (log_humanread_filename != "")
        outputText("The pruning took " + std::to_string(timesPrun[PRUN_WITH_P_UP_1_WITH_ID_AND_DW_LA]) + " seconds. ",
               log_humanread_filename);

    AutData aut_ru3 = removeUselessStates(aut_prun3);

    /* Computing the up-la(id) sim. again */
    if (log_humanread_filename != "")
        outputText("\nComputing the up-" + std::to_string(la_up) + "(id) simulation again... \n", log_humanread_filename);
    start = std::chrono::high_resolution_clock::now();
    vector<vector<bool> > up_la_with_id_2nd_time
            = up_simulation(aut_ru3, la_up, generateIdRelation(greatest_state+1), false,
                            greatest_state+1, greatest_symbol+1, &refin, timeout_start);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    sizesRel[UP_LA_WITH_ID_2ND_TIME]
            = getSizeOfRel(up_la_with_id_2nd_time);
    numbRefs[UP_LA_WITH_ID_2ND_TIME] = refin;
    timesRel[UP_LA_WITH_ID_2ND_TIME] =
            std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    if (log_humanread_filename != "")
        outputText("The up-" + std::to_string(la_up) + "(id) simulation has "
              + std::to_string(sizesRel[UP_LA_WITH_ID_2ND_TIME]) + " pairs and it took "
              + std::to_string(numbRefs[UP_LA_WITH_ID_2ND_TIME]) + " matrix-refinements and "
              + std::to_string(timesRel[UP_LA_WITH_ID_2ND_TIME]) + " seconds to compute and ",
               log_humanread_filename);

    /* Quotienting again with the up-la(id) sim. */
    start = std::chrono::high_resolution_clock::now();
    AutData aut_quot3 = quotientAutomaton(aut_ru3,
                            transClosure(up_la_with_id_2nd_time));
    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesQuot[QUOT_WITH_UP_LA_WITH_ID_2ND] =
            std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    if (log_humanread_filename != "")
        outputText("the quotienting took " + std::to_string(timesQuot[QUOT_WITH_UP_LA_WITH_ID_2ND]) + " seconds. ",
               log_humanread_filename);

    /* Computing the dw-1 sim. */
    if (log_humanread_filename != "") outputText("\nComputing the dw-1 simulation... \n",
               log_humanread_filename);
    start = std::chrono::high_resolution_clock::now();
    vector<vector<bool> > dw_1;
    dw_1 = dw_simulation(aut_quot3, 1);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesRel[DW_1] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    sizesRel[DW_1] = getSizeOfRel(dw_1);
    numbRefs[DW_1] = refin;
    if (log_humanread_filename != "") outputText("The dw-1 simulation has " + std::to_string(sizesRel[DW_1])
               + " and took " + std::to_string(timesRel[DW_1]) + " seconds to compute. ",
               log_humanread_filename);

    /* Computing the up-la(dw-1) sim */
    if (log_humanread_filename != "") outputText("\nComputing the up-" + std::to_string(la_up) + "(dw-1) simulation... \n",
               log_humanread_filename);
    start = std::chrono::high_resolution_clock::now();
    vector<vector<bool> > up_la_with_dw_1
            = up_simulation(aut_quot3, la_up, dw_1, false, greatest_state+1, greatest_symbol+1, &refin, timeout_start);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    sizesRel[UP_LA_WITH_DW_1] = getSizeOfRel(up_la_with_dw_1);
    numbRefs[UP_LA_WITH_DW_1] = refin;
    timesRel[UP_LA_WITH_DW_1] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    if (log_humanread_filename != "")
        outputText("The up-" + std::to_string(la_up) + "(dw-1) simulation has "
              + std::to_string(sizesRel[UP_LA_WITH_DW_1])
              + " pairs and the computation of the relation took "
              + std::to_string(numbRefs[UP_LA_WITH_DW_1]) + " matrix-refinements and "
              + std::to_string(timesRel[UP_LA_WITH_DW_1]) + " seconds to compute, ",
               log_humanread_filename);

    /* Pruning with P(up-la(dw-1) sim, strict dw-1 sim) */
    start = std::chrono::high_resolution_clock::now();
    AutData aut_prun4 = prune(aut_quot3, up_la_with_dw_1, dw_1, true);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    timesPrun[PRUN_WITH_P_UP_LA_WITH_DW_1_AND_DW_1] = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    if (log_humanread_filename != "")
        outputText("the pruning took " + std::to_string(timesPrun[PRUN_WITH_P_UP_LA_WITH_DW_1_AND_DW_1]) + " seconds. \n",
               log_humanread_filename);

    AutData aut_ru4 = removeUselessStates(aut_prun4);

    if (testData.numb_relations > 0)
    {
        testData.inc();
        testData.updateAvgSizesRels(sizesRel);
        testData.updateAvgNumbsRefs(numbRefs);
        testData.updateAvgTimes(timesRel, timesQuot, timesPrun);
    }

    std::cout << std::flush;

    return aut_ru4;
}

AutData heavy(const AutData& autData, unsigned int la_dw, unsigned int la_up,
              Time& timeout_start, TestData& testData, string log_humanread_filename)
{

    Automaton aut_i = copyAut(getAut(autData)), aut_f;
    const vector<typerank> ranks = getRanks(autData);
    symbol greatest_symbol  =  getGreatestUsedSymbol(aut_i);

    Time timeout_start_ = (timerNotStarted(timeout_start)) ? startTimer() : timeout_start;

    while(true)
    {
        if (la_dw>1 || la_up>1)
        {
            while(true)
            {

                aut_f = getAut(applyMinimizationSequence(wrapAutData(aut_i,ranks), 1, 1,
                                                         getGreatestUsedState(aut_i),
                                                         greatest_symbol,
                                                         timeout_start_, emptyTestData,
                                                         log_humanread_filename));
                if (equalSize(aut_i, aut_f)) break;
                else aut_i = aut_f;

            }
        }
        aut_f = getAut(applyMinimizationSequence(wrapAutData(aut_i,ranks), la_dw, la_up,
                                                 getGreatestUsedState(aut_i),
                                                 greatest_symbol,
                                                 timeout_start_, testData,
                                                 log_humanread_filename));
        if (equalSize(aut_i, aut_f)) return wrapAutData(aut_f,ranks);
        else aut_i = aut_f;

    }

    return wrapAutData(aut_f,ranks);

}

AutData heavyGradual(const AutData& autData_i,
                     unsigned int la_dw, unsigned int la_dw_max,
                     unsigned int greatest_symbol,
                     TestData& testData, Time& timeout_start, string log_humanread_filename)
{

    if (la_dw > la_dw_max) return autData_i;

    AutData autData_f = applyMinimizationSequence(autData_i,
                                                  la_dw, ((unsigned int) pow(2.0, la_dw)) - 1,
                                                  getGreatestUsedState(getAut(autData_i)),
                                                  greatest_symbol,
                                                  timeout_start, testData, log_humanread_filename);

    if (!equalSize(autData_i,autData_f))
        return heavyGradual(autData_f, 1, la_dw_max, greatest_symbol, testData, timeout_start, log_humanread_filename);
    else
        return heavyGradual(autData_f, ++la_dw, la_dw_max, greatest_symbol, testData, timeout_start, log_humanread_filename);

}

void applyProcedure(const AutData& autData_i,   MetaData& metaData,
                    TestData& testData_ru,
                    TestData& testData_quot,    Timeout& timeout_quot,
                    TestData& testData_qAndP,   Timeout& timeout_qAndP,
                    TestData& testData_heavy11, Timeout& timeout_heavy11,
                    TestData& testData_heavy24, Timeout& timeout_heavy24,
                    string log, bool ignore_i, bool ignore_red)
{
    Automaton aut_i = getAut(autData_i);

    /* Declarations and initializations independent of each reducing method. */
    metaData.inc();
    metaData.updateInitialAvg(getNumbUsedStates(aut_i,ignore_i), getNumbTransitions(aut_i,ignore_i),
                              getNumbSymbols(aut_i,ignore_i), getAvgRank(aut_i),
                              getTransitionDensity(aut_i,ignore_i));
    metaData.checkInitialGreatest(getNumbUsedStates(aut_i,ignore_i),
                                  getNumbTransitions(aut_i,ignore_i),
                                  getNumbSymbols(aut_i,ignore_i), getAvgRank(aut_i),
                                  getTransitionDensity(aut_i,ignore_i));
    //auto start = std::chrono::high_resolution_clock::now();
    //auto elapsed = std::chrono::high_resolution_clock::now();
    Time start;
    float seconds, q_red, delta_red, transDens_red;

    log_autSizes(log, autData_i, ignore_i);

    state greatest_state    =  getGreatestUsedState(aut_i);
    symbol greatest_symbol  =  getGreatestUsedSymbol(aut_i);

    /* Applying Remove Useless States */
    start = startTimer(); //std::chrono::high_resolution_clock::now();
    AutData autData_ru = removeUselessStates(autData_i);
    //elapsed = std::chrono::high_resolution_clock::now() - start;
    seconds = elapsedSec(start); // std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;

    Automaton aut_ru     =  getAut(autData_ru);
    q_red          =  measureStatesReduction(aut_ru, aut_i, ignore_red);
    delta_red      =  measureTransitionsReduction(aut_ru, aut_i, ignore_red);
    transDens_red  =  measureTransDensReduction(aut_ru, aut_i, ignore_red);
    testData_ru.inc();
    testData_ru.updateAvgReductions(q_red, delta_red, transDens_red);
    testData_ru.checkGreatestReductions(q_red, delta_red, transDens_red);
    testData_ru.updateReductionBuckets(q_red, delta_red, transDens_red);
    testData_ru.updateAvgTime(seconds);
    
    log_autReduction(log, q_red, delta_red, transDens_red, seconds);

    /* Applying Quotienting with dw-1 sim. */
    start = startTimer(); // std::chrono::high_resolution_clock::now();
    AutData autData_quot;
    try
    {
        autData_quot = quotient_with_dw_la_sim(removeUselessStates(autData_i), 1,greatest_state, greatest_symbol);
    }
    catch (timeout_& e)
    {
        timeout_quot.inc();
        timeout_quot.checkSmallest(getNumbUsedStates(aut_i),
                                    getNumbTransitions(aut_i),
                                    getTransitionDensity(aut_i));
        autData_quot = wrapAutData(e.getAut(), getRanks(autData_i));
    }
    //elapsed = std::chrono::high_resolution_clock::now() - start;
    seconds = elapsedSec(start); // std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    Automaton aut_quot = getAut(autData_quot);
    q_red          =  measureStatesReduction(aut_quot, aut_i, ignore_red);
    delta_red      =  measureTransitionsReduction(aut_quot, aut_i, ignore_red);
    transDens_red  =  measureTransDensReduction(aut_quot, aut_i, ignore_red);
    testData_quot.inc();
    testData_quot.updateAvgReductions(q_red, delta_red, transDens_red);
    testData_quot.checkGreatestReductions(q_red, delta_red, transDens_red);
    testData_quot.updateReductionBuckets(q_red, delta_red, transDens_red);
    testData_quot.updateAvgTime(seconds);
    log_autReduction(log, q_red,delta_red,transDens_red,seconds);


    /* Applying Quotienting & Pruning with dw-1 sim. */
    start = startTimer();
    AutData autData_qAndP;
    try
    {
        //auto start = std::chrono::high_resolution_clock::now();
        autData_qAndP = applyQuotAndPrunDwLa(removeUselessStates(autData_i), 1, greatest_state, greatest_symbol);
        //auto elapsed = std::chrono::high_resolution_clock::now() - start;
        //float seconds =
          //      std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
    }
    catch (timeout_& e)
    {
        timeout_qAndP.inc();
        timeout_qAndP.checkSmallest(getNumbUsedStates(aut_i),
                                    getNumbTransitions(aut_i),
                                    getTransitionDensity(aut_i));
        autData_qAndP = wrapAutData(e.getAut(), getRanks(autData_i));
    }
    seconds = elapsedSec(start);
    Automaton aut_qAndP  =  getAut(autData_qAndP);
    q_red          =  measureStatesReduction(aut_qAndP, aut_i, ignore_red);
    delta_red      =  measureTransitionsReduction(aut_qAndP, aut_i, ignore_red);
    transDens_red  =  measureTransDensReduction(aut_qAndP, aut_i, ignore_red);
    testData_qAndP.inc();
    testData_qAndP.updateAvgReductions(q_red, delta_red, transDens_red);
    testData_qAndP.checkGreatestReductions(q_red, delta_red, transDens_red);
    testData_qAndP.updateReductionBuckets(q_red, delta_red, transDens_red);
    testData_qAndP.updateAvgTime(seconds);
    log_autReduction(log, q_red,delta_red,transDens_red,seconds);


    /* Applying Heavy(1,1). */
    unsigned int la_dw = 1, la_up = 1;
    start = startTimer();
    AutData autData_heavy11;
    try
    {
        autData_heavy11 = heavy(autData_i, la_dw, la_up);
    }
    catch (timeout_& e)
    {
        timeout_heavy11.inc();
        timeout_heavy11.checkSmallest(getNumbUsedStates(aut_i,true), getNumbTransitions(aut_i,true), getTransitionDensity(aut_i,true));
        autData_heavy11 = wrapAutData(e.getAut(), getRanks(autData_i));
    }
    seconds = elapsedSec(start);
    Automaton aut_heavy11 =  getAut(autData_heavy11);
    q_red           =  measureStatesReduction(aut_heavy11, aut_i, ignore_red);
    delta_red       =  measureTransitionsReduction(aut_heavy11, aut_i, ignore_red);
    transDens_red   =  measureTransDensReduction(aut_heavy11, aut_i, ignore_red);
    testData_heavy11.inc();
    testData_heavy11.updateAvgReductions(q_red, delta_red, transDens_red);
    testData_heavy11.checkGreatestReductions(q_red, delta_red, transDens_red);
    testData_heavy11.updateReductionBuckets(q_red, delta_red, transDens_red);
    testData_heavy11.updateAvgTime(seconds);
    log_autReduction(log, q_red,delta_red,transDens_red,seconds);

    /* Applying Heavy(2,4). */
    la_dw = 2;
    la_up = 4;
    start = startTimer();
    AutData autData_heavy24;
    try
    {
        autData_heavy24 = heavy(autData_i, la_dw, la_up);
    }
    catch (timeout_& e)
    {
        timeout_heavy24.inc();
        timeout_heavy24.checkSmallest(getNumbUsedStates(aut_i,true), getNumbTransitions(aut_i,true), getTransitionDensity(aut_i,true));
        autData_heavy24 = wrapAutData(e.getAut(), getRanks(autData_i));
    }
    seconds = elapsedSec(start);
    Automaton aut_heavy24 = getAut(autData_heavy24);
    q_red           =  measureStatesReduction(aut_heavy24, aut_i, ignore_red);
    delta_red       =  measureTransitionsReduction(aut_heavy24, aut_i, ignore_red);
    transDens_red   =  measureTransDensReduction(aut_heavy24, aut_i, ignore_red);
    testData_heavy24.inc();
    testData_heavy24.updateAvgReductions(q_red, delta_red, transDens_red);
    testData_heavy24.checkGreatestReductions(q_red, delta_red, transDens_red);
    testData_heavy24.updateReductionBuckets(q_red, delta_red, transDens_red);
    testData_heavy24.updateAvgTime(seconds);
    log_autReduction(log, q_red,delta_red,transDens_red,seconds);

    writeToFile(log, "\n");

}

AutData applyHeavy(const AutData& autData_i, MetaData& metaData,
                   unsigned int la_dw, unsigned int la_up,
                   TestData& testData_heavy, Timeout& timeout_heavy,
                   int outputStat_type, string log)
{

    Automaton aut_i = getAut(autData_i);

    /* Statistical measures of the input automaton */
    metaData.inc();
    metaData.updateInitialAvg(getNumbUsedStates(aut_i), getNumbTransitions(aut_i),
                               getNumbSymbols(aut_i), getAvgRank(aut_i),
                               getTransitionDensity(aut_i),
                              /*measureTransOverlaps(aut_i)*/{});
    metaData.checkInitialGreatest(getNumbUsedStates(aut_i),
                                  getNumbTransitions(aut_i),
                                  getNumbSymbols(aut_i), getAvgRank(aut_i),
                                  getTransitionDensity(aut_i),
                                  /*measureTransOverlaps(aut_i)*/ {});
    if (outputStat_type == LOG)
        log_autSizes(log, autData_i);
    if (outputStat_type == STD_OUT_HUMAN)
        outputText("Q_i: " + std::to_string(getNumbUsedStates(aut_i)) + "\tTrans_i: " + std::to_string(getNumbTransitions(aut_i))
                   + "\tTransDens_i: " + convert2DecStr(getTransitionDensity(aut_i)) + "\t");
    if (outputStat_type == STD_OUT_MACHINE)
        outputText(std::to_string(getNumbUsedStates(aut_i)) + "\t" + std::to_string(getNumbTransitions(aut_i)) + "\t"
                   + convert2DecStr(getTransitionDensity(aut_i)) + "\t");
    //log_autTransOverlap(log, autData_i);

    Time start = startTimer();
    AutData autData_heavy;
    try
    {
        autData_heavy = heavy(autData_i, la_dw, la_up, start, testData_heavy);
    }
    catch (timeout_& e)
    {
        timeout_heavy.inc();
        timeout_heavy.checkSmallest(getNumbUsedStates(aut_i), getNumbTransitions(aut_i), getTransitionDensity(aut_i));
        autData_heavy = wrapAutData(e.getAut(), getRanks(autData_i));
    }

    /* Statistical measures of the reduced automaton */
    float seconds = elapsedSec(start);
    float q_red          =  measureStatesReduction(autData_heavy, autData_i);
    float delta_red      =  measureTransitionsReduction(autData_heavy, autData_i);
    float transDens_red  =  measureTransDensReduction(autData_heavy, autData_i);
    testData_heavy.inc();
    testData_heavy.updateAvgReductions(q_red, delta_red, transDens_red);
    testData_heavy.checkGreatestReductions(q_red, delta_red, transDens_red);
    testData_heavy.updateReductionBuckets(q_red, delta_red, transDens_red);
    testData_heavy.updateAvgTime(seconds);
    if (outputStat_type == LOG)
    {
        log_autReduction(log, autData_heavy, autData_i);
        log_time(log, seconds);
        writeToFile(log, "\n");
    }
    if (outputStat_type == STD_OUT_HUMAN)
    {
        outputText("Q_f/Q_i: " + convert2DecStr(q_red) + "%\t");
        outputText("Trans_f/Trans_i: " + convert2DecStr(delta_red) + "%\t");
        outputText("TD_f/TD_i: " + convert2DecStr(transDens_red) + "%\t");
        outputText("Time: " + convert2DecStr(seconds) + "s\t");
        outputText("\n\n");
    }
    if (outputStat_type == STD_OUT_MACHINE)
    {
        outputText(convert2DecStr(q_red) + "\t");
        outputText(convert2DecStr(delta_red) + "\t");
        outputText(convert2DecStr(transDens_red) + "\t");
        outputText(convert2DecStr(seconds) + "\t");
    }

    return autData_heavy;
}

/* Auxiliary function to applyHeavyWithSat: returns true iff the first automaton has less states than the second
 * one OR has the same number of states but less transitions. */
bool autIsBetter(const AutData& autData1, const AutData& autData2,
                 unsigned int q1=0, unsigned int delta1=0, unsigned int q2=0, unsigned int delta2=0)
{
    unsigned int q1_ = (q1==0) ? getNumbUsedStates(autData1) : q1;
    unsigned int q2_ = (q2==0) ? getNumbUsedStates(autData2) : q2;

    if (q1_ < q2_) return true;
    if (q1_ > q2_) return false;

    unsigned int delta1_ = (delta1==0) ? getNumbTransitions(autData1) : delta1;
    unsigned int delta2_ = (delta2==0) ? getNumbTransitions(autData2) : delta2;

    if (delta1_ < delta2_) return true;

    return false;
}

bool autIsWorse(const AutData& autData1, const AutData& autData2,
                unsigned int q1=0, unsigned int delta1=0, unsigned int q2=0, unsigned int delta2=0)
{
    unsigned int q1_ = (q1==0) ? getNumbUsedStates(autData1) : q1;
    unsigned int q2_ = (q2==0) ? getNumbUsedStates(autData2) : q2;

    if (q1_ > q2_) return true;

    unsigned int delta1_ = (delta1==0) ? getNumbTransitions(autData1) : delta1;
    unsigned int delta2_ = (delta2==0) ? getNumbTransitions(autData2) : delta2;

    if (delta1_ > delta2_) return true;

    return false;
}

/* Auxiliary function to applyHeavyWithSat: returns the best automaton between the two given. */
AutData bestAut(const AutData& autData1, const AutData& autData2,
                unsigned int q1=0, unsigned int delta1=0, unsigned int q2=0, unsigned int delta2=0)
{
    if (autIsBetter(autData1,autData2,q1,delta1,q2,delta2)) return autData1;
    else return autData2;
}

/* Applies Heavy interleaved with Saturation. The parameter max_sat_attempts gives the number of consecutive
 * times that Saturation+Heavy is allowed to increase (instead of decrease) the number of transitions.
 * The resulting automaton will be in best_aut, a variable which this function receives by reference
 * from applyHeavyWithSat, so that when applyHeavyWithSat catches a timeout exception it can compare the
 * returned automaton with best_aut and return the best one.    */
void heavyWithSat5(const AutData& autData_i, AutData& best_aut, unsigned int la_dw, unsigned int la_up,
                     unsigned int la_dw_sat, unsigned int la_up_sat, unsigned int max_sat_attempts,
                     Time& timeout_start, TestData& testData)
{
    Time timeout_start_ = (timerNotStarted(timeout_start)) ? startTimer() : timeout_start;

    AutData autData1, autData2;
    autData1 = heavy(autData_i, la_dw, la_up, timeout_start_, testData);
    best_aut = autData1;
    unsigned int counter = 0;

    while(true)
    {
        /* Perform downwards saturation */
        set<transition> new_trans;
        autData2 = saturate_with_la_dw_sim(autData1, la_dw_sat, new_trans,0,timeout_start_);
        unsigned int n_new_trans_dw = new_trans.size();

        if (n_new_trans_dw > 0)
        {
            /* Perform upwards reductions */
            vector<vector<bool> > W = up_simulation_of_id(autData2,la_up,0,0,NULL,timeout_start_);
            autData2 = quotient_with_up_la_of_id_sim(autData2,la_up,W,0,0,timeout_start_);
            autData2 = prune_with_strict_up_sim_of_id_and_dw_la_sim(autData2,la_dw,true,{},{},0,0,timeout_start_);
            autData2 = quotient_with_up_la_of_id_sim(autData2,la_up,{},0,0,timeout_start_);
            autData2 = prune_with_up_la_sim_of_dw_sim_and_strict_dw_sim(autData2,la_up,true,0,0,timeout_start_);

            /* Perform upwards saturation */
            autData2 = saturate_with_la_up_sim_of_id(autData2,la_up_sat,new_trans,0,0,timeout_start_);

            /* Call Heavy again, as the transitions added may pave way for further reductions */
            autData2 = heavy(autData2,la_dw,la_up,timeout_start_,testData);
        }
        else
        {
            /* Perform upwards saturation */
            autData2 = saturate_with_la_up_sim_of_id(autData2,la_up_sat,new_trans,0,0,timeout_start_);
            unsigned int n_new_trans_up = new_trans.size();
            if (n_new_trans_up > 0)
            {
                /* Call Heavy again, as the transitions added may pave way for further reductions */
                autData2 = heavy(autData2,la_dw,la_up,timeout_start_);
            }
            else
                return;
        }

        unsigned int q2 = getNumbUsedStates(autData2);
        unsigned int delta2 = getNumbTransitions(autData2);
        if (autIsBetter(autData2,best_aut,q2,delta2))
        {
            counter  = 0;
            autData1 = autData2;
            best_aut = autData2;
        }
        else if (++counter < max_sat_attempts)
            autData1 = autData2;
        else
            return;
    }

    return;
}

void heavyWithSat2(const AutData& autData_i, AutData& best_aut, unsigned int la_dw, unsigned int la_up,
                   unsigned int la_dw_sat, unsigned int la_up_sat, unsigned int max_sat_attempts,
                   Time& timeout_start, TestData& testData)
{
    Time timeout_start_ = (timerNotStarted(timeout_start)) ? startTimer() : timeout_start;

    AutData autData0, autData1, autData2;
    autData0 = heavy(autData_i, la_dw, la_up, timeout_start_, testData);
    autData1 = autData0;
    best_aut = autData1;
    unsigned int counter = 0;

    while(true)
    {
        set<transition> new_trans;
        unsigned int n_new_trans = 0;
        while(true)
        {
            /* Perform downwards saturation */
            unsigned int n_new_trans_local = n_new_trans;
            autData2 = saturate_with_la_dw_sim(autData1, la_dw_sat, new_trans, 0, timeout_start_);
            n_new_trans = new_trans.size();

            if ((n_new_trans - n_new_trans_local) == 0)
                break;

            /* Perform upwards reductions */
            std::set<transition> pruned_trans;
            vector<vector<bool> > W = up_simulation_of_id(autData2,la_up,0,0,NULL,timeout_start_);
            autData2 = quotient_with_up_la_of_id_sim(autData2,la_up,W,0,0,timeout_start_);
            autData2 = prune_with_strict_up_la_sim_of_id_and_id(autData2,la_up,pruned_trans,true,strictRel(W),0,0,timeout_start_);
            autData2 = prune_with_strict_up_sim_of_id_and_dw_la_sim(autData2,la_dw,pruned_trans,true,{},{},0,0,timeout_start_);
            autData2 = quotient_with_up_la_of_id_sim(autData2,la_up,{},0,0,timeout_start_);
            autData2 = prune_with_up_la_sim_of_dw_sim_and_strict_dw_sim(autData2,la_up,pruned_trans,true,0,0,timeout_start_);

            unsigned int q2 = getNumbUsedStates(autData2);
            unsigned int delta2 = getNumbTransitions(autData2);
            if (autIsWorse(autData2,autData1,q2,delta2))
                break;
            else if (pruned_trans == new_trans)
                break;
            else
            {
                autData1 = autData2;  /* continue. */
                best_aut = bestAut(autData2,best_aut,q2,delta2);
            }

        }

        /* Perform upwards saturation */
        autData2 = saturate_with_la_up_sim_of_id(autData2,la_up_sat,new_trans,0,0,timeout_start_);
        n_new_trans = new_trans.size();
        if (n_new_trans > 0)
        {
            /* Call Heavy again, as the transitions added may pave the way for further reductions */
            autData2 = heavy(autData2,la_dw,la_up,timeout_start_);
        }
        else
            return;

        unsigned int q2 = getNumbUsedStates(autData2);
        unsigned int delta2 = getNumbTransitions(autData2);
        if (autIsBetter(autData2,best_aut,q2,delta2))
        {
            counter  = 0;
            autData1 = autData2;
            best_aut = autData2;
        }
        else if (++counter < max_sat_attempts)
            autData1 = autData2;
        else
            return;
    }

    return;
}


/* Richard's version */
void heavyWithSat1(const AutData& autData_i, AutData& best_aut, unsigned int la_dw, unsigned int la_up,
                     unsigned int la_dw_sat, unsigned int la_up_sat, unsigned int max_sat_attempts,
                     Time& timeout_start, TestData& testData)
{
    Time timeout_start_ = (timerNotStarted(timeout_start)) ? startTimer() : timeout_start;

    AutData autData1, autData2;
    autData1 = heavy(autData_i, la_dw, la_up, timeout_start_, testData);
    best_aut = autData1;
    unsigned int counter = 0;

    while(true)
    {
        /* Perform downwards saturation */
        set<transition> new_trans_dw;
        autData2 = saturate_with_la_dw_sim(autData1, la_dw_sat, new_trans_dw,0,timeout_start_);
        unsigned int n_new_trans_dw = new_trans_dw.size();

        /* Perform upwards saturation */
        set<transition> new_trans_up;
        autData2 = saturate_with_la_up_sim_of_id(autData2,la_up_sat,new_trans_up,0,0,timeout_start_);
        unsigned int n_new_trans_up = new_trans_up.size();

        if (n_new_trans_dw > 0)
        {
            /* Perform some upwards reductions */
            vector<vector<bool> > W = up_simulation_of_id(autData2,la_up,0,0,NULL,timeout_start_);
            autData2 = quotient_with_up_la_of_id_sim(autData2,la_up,W,0,0,timeout_start_);
            autData2 = prune_with_strict_up_sim_of_id_and_dw_la_sim(autData2,la_dw,true,{},{},0,0,timeout_start_);
            autData2 = quotient_with_up_la_of_id_sim(autData2,la_up,W,0,0,timeout_start_);
            autData2 = prune_with_up_la_sim_of_dw_sim_and_strict_dw_sim(autData2,la_up,true,0,0,timeout_start_);
        }

        if (n_new_trans_dw + n_new_trans_up == 0)
            break;

        autData2 = heavy(autData2,la_dw,la_up,timeout_start_,testData);

        unsigned int q2 = getNumbUsedStates(autData2);
        unsigned int delta2 = getNumbTransitions(autData2);
        if (autIsBetter(autData2,best_aut,q2,delta2))
        {
            counter  = 0;
            autData1 = autData2;
            best_aut = autData2;
        }
        else if (++counter < max_sat_attempts)
            autData1 = autData2;
        else
            return;
    }

    return;
}

void heavyWithSat3(const AutData& autData_i, AutData& best_aut, unsigned int la_dw, unsigned int la_up,
                                   unsigned int la_dw_sat, unsigned int la_up_sat, unsigned int max_sat_attempts,
                                   Time& timeout_start, TestData& testData)
{
    Time timeout_start_ = (timerNotStarted(timeout_start)) ? startTimer() : timeout_start;

    AutData autData1, autData2;
    autData1 = heavy(autData_i, la_dw, la_up, timeout_start_, testData);
    best_aut = autData1;
    unsigned int counter = 0;

    while(true)
    {
        set<transition> new_trans_dw;
        unsigned int n_new_trans_dw = 0, n_new_trans_up = 0;
        while(true)
        {
            /* Perform downwards saturation */
            autData2 = saturate_with_la_dw_sim(autData1, la_dw_sat, new_trans_dw,0,timeout_start_);
            n_new_trans_dw = new_trans_dw.size();

            /* Perform upwards saturation */
            set<transition> new_trans_up;
            autData2 = saturate_with_la_up_sim_of_id(autData2,la_up_sat,new_trans_up,0,0,timeout_start_);
            unsigned int n_new_trans_up_local = new_trans_up.size();
            new_trans_up.clear();
            n_new_trans_up += n_new_trans_up_local;

            if (n_new_trans_up_local==0)
                break;
            else
                autData1 = autData2;  /* continue. */
        }

        if (n_new_trans_dw > 0)
        {
            /* Perform some upwards reductions */
            vector<vector<bool> > W = up_simulation_of_id(autData2,la_up,0,0,NULL,timeout_start_);
            autData2 = quotient_with_up_la_of_id_sim(autData2,la_up,W,0,0,timeout_start_);
            autData2 = prune_with_strict_up_sim_of_id_and_dw_la_sim(autData2,la_dw,true,{},{},0,0,timeout_start_);
            autData2 = quotient_with_up_la_of_id_sim(autData2,la_up,W,0,0,timeout_start_);
            autData2 = prune_with_up_la_sim_of_dw_sim_and_strict_dw_sim(autData2,la_up,true,0,0,timeout_start_);
        }

        if (n_new_trans_dw + n_new_trans_up == 0)
            break;

        autData2 = heavy(autData2,la_dw,la_up,timeout_start_,testData);

        if (autIsBetter(autData2,best_aut))
        {
            counter  = 0;
            autData1 = autData2;
            best_aut = autData2;
        }
        else if (++counter < max_sat_attempts)
            autData1 = autData2;
        else
            return;
    }

    return;
}

void heavyWithSat4(const AutData& autData_i, AutData& best_aut, unsigned int la_dw, unsigned int la_up,
                                   unsigned int la_dw_sat, unsigned int la_up_sat, unsigned int max_sat_attempts,
                                   Time& timeout_start, TestData& testData)
{
    Time timeout_start_ = (timerNotStarted(timeout_start)) ? startTimer() : timeout_start;

    AutData autData1, autData2;
    autData1 = heavy(autData_i, la_dw, la_up, timeout_start_, testData);
    best_aut = autData1;
    unsigned int counter = 0;

    while(true)
    {
        unsigned int n_new_trans_dw = 0, n_new_trans_up = 0;
        while(true)
        {
            /* Perform upwards saturation */
            set<transition> new_trans_up;
            autData2 = saturate_with_la_up_sim_of_id(autData1,la_up_sat,new_trans_up,0,0,timeout_start_);
            unsigned int new_trans_up_size = new_trans_up.size();
            new_trans_up.clear();
            n_new_trans_up += new_trans_up_size;

            /* Perform downwards saturation */
            set<transition> new_trans_dw;
            autData2 = saturate_with_la_dw_sim(autData2, la_dw_sat, new_trans_dw,0,timeout_start_);
            unsigned int new_trans_dw_size = new_trans_dw.size();
            new_trans_dw.clear();
            n_new_trans_dw += new_trans_dw_size;

            if (new_trans_dw_size == 0)
                break;
            else
                autData1 = autData2;
        }

        if (n_new_trans_dw + n_new_trans_up == 0)
            break;

        autData2 = heavy(autData2,la_dw,la_up,timeout_start_,testData);

        if (autIsBetter(autData2,best_aut))
        {
            counter  = 0;
            autData1 = autData2;
            best_aut = autData2;
        }
        else if (++counter < max_sat_attempts)
            autData1 = autData2;
        else
            return;
    }

    return;
}

AutData applyHeavyWithSat(const AutData& autData_i,
                          unsigned int la_dw, unsigned int la_up,
                          unsigned int la_dw_sat, unsigned int la_up_sat, unsigned int sat_version,
                          unsigned int max_sat_attempts,
                          MetaData& metaData, TestData& testData, Timeout& timeout, int outputStat_type,
                          string log)
{

    metaData.inc();
    metaData.updateInitialAvg(getNumbUsedStates(autData_i), getNumbTransitions(autData_i),
                              getNumbSymbols(autData_i), getAvgRank(autData_i),
                              getTransitionDensity(autData_i),
                              /*measureTransOverlaps(aut_i)*/{});
    metaData.checkInitialGreatest(getNumbUsedStates(autData_i),
                                  getNumbTransitions(autData_i),
                                  getNumbSymbols(autData_i), getAvgRank(autData_i),
                                  getTransitionDensity(autData_i),
                                  /*measureTransOverlaps(aut_i)*/ {});

    if (!log.empty())     log_autSizes(log, autData_i);

    Time start = startTimer();
    AutData autData_f, best_aut;
    try
    {
        if (sat_version == 1)
            heavyWithSat1(autData_i,best_aut,la_dw,la_up,la_dw_sat,la_up_sat,
                      max_sat_attempts, start, testData);
        else if (sat_version == 2)
            heavyWithSat2(autData_i,best_aut,la_dw,la_up,la_dw_sat,la_up_sat,
                      max_sat_attempts, start, testData);
        else if (sat_version == 3)
            heavyWithSat3(autData_i,best_aut,la_dw,la_up,la_dw_sat,la_up_sat,
                      max_sat_attempts, start, testData);
        else if (sat_version == 4)
            heavyWithSat4(autData_i,best_aut,la_dw,la_up,la_dw_sat,la_up_sat,
                      max_sat_attempts, start, testData);
        else if (sat_version == 5)
            heavyWithSat5(autData_i,best_aut,la_dw,la_up,la_dw_sat,la_up_sat,
                      max_sat_attempts, start, testData);
        else
            exit_with_error("No suitable version of Sat found.");

        autData_f = best_aut;
    }
    catch (timeout_& e)
    {
            timeout.inc();
            timeout.checkSmallest(getNumbUsedStates(autData_i), getNumbTransitions(autData_i), getTransitionDensity(autData_i));

        AutData autData_e = wrapAutData(e.getAut(), getRanks(autData_i));
        autData_f = bestAut(best_aut,autData_e);
    }

    float time = elapsedSec(start);
    float q_red          =  measureStatesReduction(autData_f, autData_i);
    float delta_red      =  measureTransitionsReduction(autData_f, autData_i);
    float transDens_red  =  measureTransDensReduction(autData_f, autData_i);
    testData.inc();
    testData.updateAvgReductions(q_red, delta_red, transDens_red);
    testData.checkGreatestReductions(q_red, delta_red, transDens_red);
    testData.updateReductionBuckets(q_red, delta_red, transDens_red);
    testData.updateAvgTime(time);

    if (!log.empty())
    {
        log_autReduction(log, autData_f, autData_i);
        log_time(log, time);
        writeToFile(log, "\n");
    }
    if (outputStat_type == STD_OUT_HUMAN)
    {
        outputText("Q_f/Q_i: " + convert2DecStr(q_red) + "%\t");
        outputText("Trans_f/Trans_i: " + convert2DecStr(delta_red) + "%\t");
        outputText("TD_f/TD_i: " + convert2DecStr(transDens_red) + "%\t");
        outputText("Time: " + convert2DecStr(time) + "s\t");
        outputText("\n\n");
    }
    if (outputStat_type == STD_OUT_MACHINE)
    {
        outputText(convert2DecStr(q_red) + "\t");
        outputText(convert2DecStr(delta_red) + "\t");
        outputText(convert2DecStr(transDens_red) + "\t");
        outputText(convert2DecStr(time) + "\t");
    }

    return autData_f;
}

void compareHeavyWithSatVersions(const AutData& autData_i, MetaData& metaData,
                                    unsigned int la_dw, unsigned int la_up,
                                    unsigned int la_dw_sat, unsigned int la_up_sat, unsigned int max_sat_attempts,
                                    TestData& testData_h, Timeout& timeout_h,
                                    TestData& testData_v1, Timeout& timeout_v1,
                                    TestData& testData_v2, Timeout& timeout_v2,
                                    TestData& testData_v3, Timeout& timeout_v3,
                                    TestData& testData_v4, Timeout& timeout_v4,
                                    TestData& testData_v5, Timeout& timeout_v5,
                                    string log)
{

    metaData.inc();
    metaData.updateInitialAvg(getNumbUsedStates(autData_i), getNumbTransitions(autData_i),
                               getNumbSymbols(autData_i), getAvgRank(autData_i),
                               getTransitionDensity(autData_i), {});
    metaData.checkInitialGreatest(getNumbUsedStates(autData_i),
                                  getNumbTransitions(autData_i),
                                  getNumbSymbols(autData_i), getAvgRank(autData_i),
                                  getTransitionDensity(autData_i), {});
    log_autSizes(log, autData_i);

    /* Common declarations */
    Time start;
    float q_red, delta_red, transDens_red, time;

    /* Heavy only */
    start = startTimer();
    AutData autData_h;
    try
    {
        autData_h = heavy(autData_i,la_dw,la_up,start,testData_h);
    }
    catch (timeout_& e)
    {
        timeout_h.inc();
        timeout_h.checkSmallest(getNumbUsedStates(autData_i), getNumbTransitions(autData_i), getTransitionDensity(autData_i));
        autData_h = wrapAutData(e.getAut(), getRanks(autData_i));
    }
    time = elapsedSec(start);
    q_red          =  measureStatesReduction(autData_h, autData_i);
    delta_red      =  measureTransitionsReduction(autData_h, autData_i);
    transDens_red  =  measureTransDensReduction(autData_h, autData_i);
    testData_h.inc();
    testData_h.updateAvgReductions(q_red, delta_red, transDens_red);
    testData_h.checkGreatestReductions(q_red, delta_red, transDens_red);
    testData_h.updateReductionBuckets(q_red, delta_red, transDens_red);
    testData_h.updateAvgTime(time);
    log_autReduction(log, autData_h, autData_i);
    log_time(log, time);

    /* Version 1 */
    start = startTimer();
    AutData autData_v1, best_aut_v1;
    try
    {
        heavyWithSat1(autData_i,best_aut_v1,la_dw,la_up,la_dw_sat,la_up_sat,
                      max_sat_attempts, start, testData_v1);
        autData_v1 = best_aut_v1;
    }
    catch (timeout_& e)
    {
        timeout_v1.inc();
        timeout_v1.checkSmallest(getNumbUsedStates(autData_i), getNumbTransitions(autData_i), getTransitionDensity(autData_i));
        AutData autData_e = wrapAutData(e.getAut(), getRanks(autData_i));
        autData_v1 = bestAut(best_aut_v1,autData_e);
    }
    time = elapsedSec(start);
    q_red          =  measureStatesReduction(autData_v1, autData_i);
    delta_red      =  measureTransitionsReduction(autData_v1, autData_i);
    transDens_red  =  measureTransDensReduction(autData_v1, autData_i);
    testData_v1.inc();
    testData_v1.updateAvgReductions(q_red, delta_red, transDens_red);
    testData_v1.checkGreatestReductions(q_red, delta_red, transDens_red);
    testData_v1.updateReductionBuckets(q_red, delta_red, transDens_red);
    testData_v1.updateAvgTime(time);
    log_autReduction(log, autData_v1, autData_i);
    log_time(log, time);

    /* Version 2 */
    start = startTimer();
    AutData autData_v2, best_aut_v2;
    try
    {
        heavyWithSat2(autData_i,best_aut_v2,la_dw,la_up,la_dw_sat,la_up_sat,
                      max_sat_attempts, start, testData_v2);
        autData_v2 = best_aut_v2;
    }
    catch (timeout_& e)
    {
        timeout_v2.inc();
        timeout_v2.checkSmallest(getNumbUsedStates(autData_i), getNumbTransitions(autData_i), getTransitionDensity(autData_i));
        AutData autData_e = wrapAutData(e.getAut(), getRanks(autData_i));
        autData_v2 = bestAut(best_aut_v2,autData_e);
    }
    time = elapsedSec(start);
    q_red          =  measureStatesReduction(autData_v2, autData_i);
    delta_red      =  measureTransitionsReduction(autData_v2, autData_i);
    transDens_red  =  measureTransDensReduction(autData_v2, autData_i);
    testData_v2.inc();
    testData_v2.updateAvgReductions(q_red, delta_red, transDens_red);
    testData_v2.checkGreatestReductions(q_red, delta_red, transDens_red);
    testData_v2.updateReductionBuckets(q_red, delta_red, transDens_red);
    testData_v2.updateAvgTime(time);
    log_autReduction(log, autData_v2, autData_i);
    log_time(log, time);

    /* Version 3 */
    start = startTimer();
    AutData autData_v3, best_aut_v3;
    try
    {
        heavyWithSat3(autData_i,best_aut_v3,la_dw,la_up,la_dw_sat,la_up_sat,
                      max_sat_attempts, start, testData_v3);
        autData_v3 = best_aut_v3;
    }
    catch (timeout_& e)
    {
        timeout_v3.inc();
        timeout_v3.checkSmallest(getNumbUsedStates(autData_i), getNumbTransitions(autData_i), getTransitionDensity(autData_i));
        AutData autData_e = wrapAutData(e.getAut(), getRanks(autData_i));
        autData_v3 = bestAut(best_aut_v3,autData_e);
    }
    time = elapsedSec(start);
    q_red          =  measureStatesReduction(autData_v3, autData_i);
    delta_red      =  measureTransitionsReduction(autData_v3, autData_i);
    transDens_red  =  measureTransDensReduction(autData_v3, autData_i);
    testData_v3.inc();
    testData_v3.updateAvgReductions(q_red, delta_red, transDens_red);
    testData_v3.checkGreatestReductions(q_red, delta_red, transDens_red);
    testData_v3.updateReductionBuckets(q_red, delta_red, transDens_red);
    testData_v3.updateAvgTime(time);
    log_autReduction(log, autData_v3, autData_i);
    log_time(log, time);

    /* Version 4 */
    start = startTimer();
    AutData autData_v4, best_aut_v4;
    try
    {
        heavyWithSat4(autData_i,best_aut_v4,la_dw,la_up,la_dw_sat,la_up_sat,
                                      max_sat_attempts, start, testData_v4);
        autData_v4 = best_aut_v4;
    }
    catch (timeout_& e)
    {
        timeout_v4.inc();
        timeout_v4.checkSmallest(getNumbUsedStates(autData_i), getNumbTransitions(autData_i), getTransitionDensity(autData_i));
        AutData autData_e = wrapAutData(e.getAut(), getRanks(autData_i));
        autData_v4 = bestAut(best_aut_v4,autData_e);
    }
    time = elapsedSec(start);
    q_red          =  measureStatesReduction(autData_v4, autData_i);
    delta_red      =  measureTransitionsReduction(autData_v4, autData_i);
    transDens_red  =  measureTransDensReduction(autData_v4, autData_i);
    testData_v4.inc();
    testData_v4.updateAvgReductions(q_red, delta_red, transDens_red);
    testData_v4.checkGreatestReductions(q_red, delta_red, transDens_red);
    testData_v4.updateReductionBuckets(q_red, delta_red, transDens_red);
    testData_v4.updateAvgTime(time);
    log_autReduction(log, autData_v4, autData_i);
    log_time(log, time);

    /* Version 5 */
    start = startTimer();
    AutData autData_v5, best_aut_v5;
    try
    {
        heavyWithSat5(autData_i, best_aut_v5, la_dw, la_up, la_dw_sat, la_up_sat,
                                       max_sat_attempts, start, testData_v5);
        autData_v5 = best_aut_v5;
    }
    catch (timeout_& e)
    {
        timeout_v5.inc();
        timeout_v5.checkSmallest(getNumbUsedStates(autData_i), getNumbTransitions(autData_i), getTransitionDensity(autData_i));
        AutData autData_e = wrapAutData(e.getAut(), getRanks(autData_i));
        autData_v5 = bestAut(best_aut_v5, autData_e);
    }
    time = elapsedSec(start);
    q_red          =  measureStatesReduction(autData_v5, autData_i);
    delta_red      =  measureTransitionsReduction(autData_v5, autData_i);
    transDens_red  =  measureTransDensReduction(autData_v5, autData_i);
    testData_v5.inc();
    testData_v5.updateAvgReductions(q_red, delta_red, transDens_red);
    testData_v5.checkGreatestReductions(q_red, delta_red, transDens_red);
    testData_v5.updateReductionBuckets(q_red, delta_red, transDens_red);
    testData_v5.updateAvgTime(time);
    log_autReduction(log, autData_v5, autData_i);
    log_time(log, time);

    writeToFile(log, "\n");

    return;
}

void applyQuotCombinedPreorder(const AutData& autData, MetaData& metaData,
                               unsigned int la_dw, unsigned int la_up,
                               TestData& testData, Timeout& timeout,
                               string log_humanread_filename,
                               string log_machread_filename,
                               string log_machread_times_filename)
{

    Automaton aut = getAut(autData);
    metaData.inc();
    metaData.updateInitialAvg(getNumbUsedStates(aut), getNumbTransitions(aut),
                               getNumbSymbols(aut),   getAvgRank(aut),
                               getTransitionDensity(aut),
                               measureTransOverlaps(aut));
    metaData.checkInitialGreatest(getNumbUsedStates(aut), getNumbTransitions(aut),
                                  getNumbSymbols(aut),    getAvgRank(aut),
                                  getTransitionDensity(aut),
                                  measureTransOverlaps(aut));

    string headline = "- Applying Quotienting with Combined Preorder(" + std::to_string(la_dw) + ", "
            + std::to_string(la_up) + ")... ";
    outputText(headline, log_humanread_filename);

    if (log_machread_filename != "") writeToFile(log_machread_filename, headline);

    auto start = std::chrono::high_resolution_clock::now();

    Time timeout_start = startTimer();

    AutData autData_ru = removeUselessStates(autData);

    try
    {
        AutData autData_quot =
                quotient_with_combined_relation(autData_ru, la_dw, la_up, 0, 0, timeout_start);

        auto elapsed  = std::chrono::high_resolution_clock::now() - start;
        float seconds =
                std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;

        Automaton aut_quot = getAut(autData_quot);
        if (/*equiv(aut,aut_quot)*/ true)
        {
            float q_red          =  measureStatesReduction(aut_quot, aut);
            float delta_red      =  measureTransitionsReduction(aut_quot, aut);
            float transDens_red  =  measureTransDensReduction(aut_quot, aut);
            testData.inc();
            testData.updateAvgReductions(q_red, delta_red, transDens_red);
            testData.checkGreatestReductions(q_red, delta_red, transDens_red);
            testData.updateReductionBuckets(q_red, delta_red, transDens_red);
            testData.updateAvgTime(seconds);

            string q_red_str          =  std::to_string(q_red);
            string delta_red_str      =  std::to_string(delta_red);
            string transDens_red_str  =  std::to_string(transDens_red);

            outputText("  The resulting automaton has "
                       + q_red_str + "% of its states, "
                       + delta_red_str + "% of its transitions and "
                       + transDens_red_str + "% of its transition density. \n",
                       log_humanread_filename);

            if (log_machread_filename != "")
                writeToFile(log_machread_filename,
                            q_red_str + "\t" + delta_red_str + "\t" + transDens_red_str + "\n");

            if (log_machread_times_filename != "")
                writeToFile(log_machread_times_filename,
                            q_red_str + "\t" + delta_red_str + "\t" + std::to_string(seconds) + "\n");

        }
        else
        {
           outputText("\n RU + Quotienting with Combined Preorder did NOT preserve the language!\n");
        }
    }
    catch (timeout_& e)
    {
        outputText("TIMEOUT: The Quotienting method took longer than "
                  + std::to_string( (float) TIMEOUT/60 )
                   + " minutes and therefore was aborted.\n",
                   log_humanread_filename);
        timeout.inc();
        timeout.checkSmallest(getNumbUsedStates(aut), getNumbTransitions(aut), getTransitionDensity(aut));

        float q_red          =  100.0;
        float delta_red      =  100.0;
        float transDens_red  =  100.0;
        testData.inc();
        testData.updateAvgReductions(q_red, delta_red, transDens_red);
        testData.checkGreatestReductions(q_red, delta_red, transDens_red);
        testData.updateReductionBuckets(q_red, delta_red, transDens_red);
        testData.updateAvgTime(TIMEOUT);

        string q_red_str          =  std::to_string(q_red);
        string delta_red_str      =  std::to_string(delta_red);
        string transDens_red_str  =  std::to_string(transDens_red);

        if (log_machread_filename != "")
            writeToFile(log_machread_filename,
                        q_red_str + "\t" + delta_red_str + "\t" + transDens_red_str + "\n");

        if (log_machread_times_filename != "")
            writeToFile(log_machread_times_filename,
                        q_red_str + "\t" + delta_red_str + "\t" + "TO" + "\n");

    }

}
