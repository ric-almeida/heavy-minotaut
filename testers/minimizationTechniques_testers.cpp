
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Implementation file defining a collection of functions that perform tests   *
 *  to minimization techniques based on the computation of both lookahead downward  *
 *  simulation and lookahead upward simulation.                                     *
 * 																					*
 ************************************************************************************/

#include "minimizationTechniques_testers.hh"

void printSummary_procedure(unsigned int la_dw, unsigned int la_up, MetaData& metadata,
                                   TestData& testData_ru,
                                   TestData& testData_quot,   Timeout& timeout_quot,
                                   TestData& testData_qAndP,  Timeout& timeout_qAndP,
                                   TestData& testData_heavy,  TestData& testData_heavy_rels,
                                   Timeout& timeout_heavy,    string log_humanread_filename = "")
{

    outputText("\nSummary results of applying the following Procedure to a battery of test automata: \n  1) Removing the useless states (i.e., states which cannot be reached starting from a leaf-rule or which cannot reach a final state); \n",
               log_humanread_filename);

    outputText("  2) Removing the useless states + Quotienting with dw-" + std::to_string(la_dw) + " sim.; \n"
             + "  3) Removing the useless states + Quotienting with dw-" + std::to_string(la_dw)
             + " sim. + Pruning with P(id, dw-" + std::to_string(la_dw) + " sim.); \n"
             + "  4) Removing the useless states + Applying the Heavy(" + std::to_string(la_dw)
             + ", " + std::to_string(la_up) + ") method, defined as a double loop using the following sequence of steps: \n"
             + "    4.1) computing the dw-" + std::to_string(la_dw) + " sim. + quotienting with it; \n"
             + "    4.2) pruning with P(id, strict dw-" + std::to_string(la_dw) + " sim.); \n"
             + "    4.3) removing the useless states; \n"
             + "    4.4) computing the up-" + std::to_string(la_up) + "(id) sim. + quotienting with it; \n"
             + "    4.5) computing the strict up-1(id) sim.; \n"
             + "    4.6) computing the dw-" + std::to_string(la_dw) + " sim. again; \n"
             + "    4.7) pruning with P(strict up-1(id) sim., dw-" + std::to_string(la_dw) + " sim.); \n"
             + "    4.8) removing the useless states; \n"
             + "    4.9) repetition of 4.4): computing the up-" + std::to_string(la_up)
             + "(id) sim. + quotienting with it; \n"
             + "    4.10) computing the dw-1 sim.; \n"
             + "    4.11) computing the up-" + std::to_string(la_up) + "(dw-1) sim.; \n"
             + "    4.12) pruning with P(up-" + std::to_string(la_up) + "(dw-1) sim., strict dw-1 sim.); \n"
             + "    4.13) removing the useless states; \n"

              + "We performed tests on a total of " + std::to_string(metadata.total) + " automata, with an average of "
                 + std::to_string(metadata.initial_avg_q) + " states, " + std::to_string(metadata.initial_avg_delta) + " transitions, "
                 + std::to_string(metadata.initial_avg_sigma) + " symbols, average rank per symbol of "
                 + std::to_string(metadata.initial_avg_ranking) + " and transition density of " + std::to_string(metadata.initial_avg_dens) + ". "
                 + std::to_string(metadata.initial_greatest_q) + ", " + std::to_string(metadata.initial_greatest_delta) + ", "
                 + std::to_string(metadata.initial_greatest_sigma) + ", " + std::to_string(metadata.initial_greatest_ranking) + " and "
                 + std::to_string(metadata.initial_greatest_dens) + " were the greatest number of states, number of transitions, number of symbols, average rank per symbol and transition density observed;\n"

                 + " -  When Removing the useless states, on average: \n"
                  + "    - the number of states at the end of each reduction was "
                  + std::to_string(testData_ru.avg_q_reduction) + "%, the number of transitions was "
                    + std::to_string(testData_ru.avg_delta_reduction) + "% and the transition density was "
                    + std::to_string(testData_ru.avg_transDens_reduction) + "%; \n"
                    + "    - the most significant reduction in terms of number of states happened for an automaton whose number of states was reduced to "
                    + std::to_string(testData_ru.greatest_q_reduction) + "%;\n"
                    + "    - the most significant reduction in terms of number of transitions happened for an automaton whose number of transitions was reduced to "
                    + std::to_string(testData_ru.greatest_delta_reduction) + "%;\n"
                    + "    - the most significant reduction in terms of transition density happened for an automaton whose transition density at the end of the minimization was "
                    + std::to_string(testData_ru.greatest_transDens_reduction) + "%;\n"

                 + " - Quotienting with dw-" + std::to_string(la_dw) + " timed out in "
                  + std::to_string(timeout_quot.total)
                  + " (" + std::to_string(( (float) timeout_quot.total / (float) metadata.total ) * (float) 100) + "%) "
                  + "of the automata. On average: \n"
                  + "    - the number of states at the end of each reduction was "
                  + std::to_string(testData_quot.avg_q_reduction) + "%, the number of transitions was "
                    + std::to_string(testData_quot.avg_delta_reduction) + "% and the transition density was "
                    + std::to_string(testData_quot.avg_transDens_reduction) + "%; \n"
                    + "    - the most significant reduction in terms of number of states happened for an automaton whose number of states was reduced to "
                    + std::to_string(testData_quot.greatest_q_reduction) + "%;\n"
                    + "    - the most significant reduction in terms of number of transitions happened for an automaton whose number of transitions was reduced to "
                    + std::to_string(testData_quot.greatest_delta_reduction) + "%;\n"
                    + "    - the most significant reduction in terms of transition density happened for an automaton whose transition density at the end of the minimization was "
                    + std::to_string(testData_quot.greatest_transDens_reduction) + "%;\n", log_humanread_filename);
        testData_quot.printReductionBuckets(log_humanread_filename);

        outputText(" - Quotienting with dw-" + std::to_string(la_dw)
                + " + Pruning with P(id, dw-" + std::to_string(la_dw) + ") timed out in "
                  + std::to_string(timeout_qAndP.total)
                  + " (" + std::to_string(( (float) timeout_qAndP.total / (float) metadata.total ) * (float) 100) + "%) "
                  + "of the automata. On average: \n"
                    + "    - the number of states at the end of each reduction was "
                    + std::to_string(testData_qAndP.avg_q_reduction) + "%, the number of transitions was "
                    + std::to_string(testData_qAndP.avg_delta_reduction) + "% and the transition density was "
                    + std::to_string(testData_qAndP.avg_transDens_reduction) + "%; \n"
                    + "    - the most significant reduction in terms of number of states happened for an automaton whose number of states was reduced to "
                    + std::to_string(testData_qAndP.greatest_q_reduction) + "%;\n"
                    + "    - the most significant reduction in terms of number of transitions happened for an automaton whose number of transitions was reduced to "
                    + std::to_string(testData_qAndP.greatest_delta_reduction) + "%;\n"
                    + "    - the most significant reduction in terms of transition density happened for an automaton whose transition density at the end of the minimization was "
                    + std::to_string(testData_qAndP.greatest_transDens_reduction) + "%;\n",
                   log_humanread_filename);
        testData_qAndP.printReductionBuckets(log_humanread_filename);

        outputText(" - Heavy(" + std::to_string(la_dw) + "," + std::to_string(la_up) + ") timed out in "
                   + std::to_string(timeout_heavy.total)
                   + " (" + std::to_string(( (float) timeout_heavy.total / (float) metadata.total ) * (float) 100) + "%) "
                   + "of the automata. On average: \n"
                     + "    - the number of states at the end of each reduction was "
                     + std::to_string(testData_heavy.avg_q_reduction) + "%, the number of transitions was "
                     + std::to_string(testData_heavy.avg_delta_reduction) + "% and the transition density was "
                     + std::to_string(testData_heavy.avg_transDens_reduction) + "%; \n"
                     + "    - the most significant reduction in terms of number of states happened for an automaton whose number of states was reduced to "
                     + std::to_string(testData_heavy.greatest_q_reduction) + "%;\n"
                     + "    - the most significant reduction in terms of number of transitions happened for an automaton whose number of transitions was reduced to "
                     + std::to_string(testData_heavy.greatest_delta_reduction) + "%;\n"
                     + "    - the most significant reduction in terms of transition density happened for an automaton whose transition density at the end of the minimization was "
                     + std::to_string(testData_heavy.greatest_transDens_reduction) + "%;\n"
                     + "  The following statistics are relative to the computations using the dw and up lookaheads given as parameters: \n"
                     + "    - the dw-" + std::to_string(la_dw) + " sim. computed contained "
                     + std::to_string(testData_heavy_rels.avg_sizes_relations[DW_LA_1ST_TIME])
                     + " pairs of states (including pairs with the special initial state ψ)"
                     + " and took " + std::to_string(testData_heavy_rels.avg_numbs_refinements[DW_LA_1ST_TIME])
                     + " refinements and " + std::to_string(testData_heavy_rels.avg_times_relations[DW_LA_1ST_TIME])
                     + " seconds to compute. \n"
                     + "    - the quotienting with dw-" + std::to_string(la_dw) + " sim. took "
                     + std::to_string(testData_heavy_rels.avg_times_quotientings[QUOT_WITH_DW_LA]) + " sec. \n"
                     + "    - the pruning with P(id, dw-" + std::to_string(la_dw) + " sim.) took "
                     + std::to_string(testData_heavy_rels.avg_times_prunings[PRUN_WITH_P_ID_AND_DW_LA]) + " sec. \n"
                     + "    - the up-" + std::to_string(la_up) + "(id) sim computed the first time contained "
                     + std::to_string(testData_heavy_rels.avg_sizes_relations[UP_LA_WITH_ID_1ST_TIME])
                     + " pairs of states and took "
                     + std::to_string(testData_heavy_rels.avg_numbs_refinements[UP_LA_WITH_ID_1ST_TIME])
                     + " refinements and " + std::to_string(testData_heavy_rels.avg_times_relations[UP_LA_WITH_ID_1ST_TIME])
                     + " seconds to compute. \n"
                     + "    - the first quotienting with up-" + std::to_string(la_up) + "(id) took "
                     + std::to_string(testData_heavy_rels.avg_times_quotientings[QUOT_WITH_UP_LA_WITH_ID_1ST]) + " sec. \n"
                     + "    - the strict up-1(id) sim contained "
                     + std::to_string(testData_heavy_rels.avg_sizes_relations[STRICT_UP_1_WITH_ID]) + " pairs of states"
                     + " and took " + std::to_string(testData_heavy_rels.avg_numbs_refinements[STRICT_UP_1_WITH_ID])
                     + " refinements and " + std::to_string(testData_heavy_rels.avg_times_relations[STRICT_UP_1_WITH_ID])
                     + " seconds to compute. \n"
                     + "    - the pruning with P(up-1(id), dw-" + std::to_string(la_dw) + ") took "
                     + std::to_string(testData_heavy_rels.avg_times_prunings[PRUN_WITH_P_UP_1_WITH_ID_AND_DW_LA])
                     + " sec. \n"
                     + "    - the dw-" + std::to_string(la_dw) + " sim computed the second time contained, on average, "
                     + std::to_string(testData_heavy_rels.avg_sizes_relations[DW_LA_2ND_TIME])
                     + " pairs of states and took "
                     + std::to_string(testData_heavy_rels.avg_numbs_refinements[DW_LA_2ND_TIME])
                     + " refinements and " + std::to_string(testData_heavy_rels.avg_times_relations[DW_LA_2ND_TIME])
                     + " seconds to compute. \n"
                     + "    - the second quotienting with up-" + std::to_string(la_up) + "(id) took "
                     + std::to_string(testData_heavy_rels.avg_times_quotientings[QUOT_WITH_UP_LA_WITH_ID_2ND]) + " sec. \n"
                     + "    - the dw-1 sim contained " + std::to_string(testData_heavy_rels.avg_sizes_relations[DW_1])
                     + " pairs of states and took " + std::to_string(testData_heavy_rels.avg_times_relations[DW_1])
                     + " seconds to compute. \n"
                     + "    - the computed up-" + std::to_string(la_up) + "(dw-1 sim) sim contained "
                     + std::to_string(testData_heavy_rels.avg_sizes_relations[UP_LA_WITH_DW_1]) + " pairs of states"
                     + " and took " + std::to_string(testData_heavy_rels.avg_numbs_refinements[UP_LA_WITH_DW_1])
                     + " refinements and " + std::to_string(testData_heavy_rels.avg_times_relations[UP_LA_WITH_DW_1])
                     + " seconds to compute.\n"
                     + "    - the pruning with P(up-" + std::to_string(la_up) + "(dw-1), strict dw-1) took "
                     + std::to_string(testData_heavy_rels.avg_times_prunings[PRUN_WITH_P_UP_LA_WITH_DW_1_AND_DW_1])
                     + " sec. \n",
                     log_humanread_filename);
         testData_heavy.printReductionBuckets(log_humanread_filename);

}

static void printSummary_quotienting(unsigned int la_dw, unsigned int la_up,
                                     MetaData& metadata, TestData& testData,
                                     Timeout& timeout, string log_humanread_filename = "")
{
    outputText("\nSummary results of applying a quotienting method to a battery of test automata using dw lookahead = "
               + std::to_string(la_dw) + " sim. and up lookahead = " + std::to_string(la_up) + ".\n"
              + "We performed tests on a total of " + std::to_string(metadata.total) + " automata, with an average of "
                 + std::to_string(metadata.initial_avg_q) + " states, " + std::to_string(metadata.initial_avg_delta) + " transitions, "
                 + std::to_string(metadata.initial_avg_sigma) + " symbols, average rank per symbol of "
                 + std::to_string(metadata.initial_avg_ranking) + " and transition density of " + std::to_string(metadata.initial_avg_dens) + ". "
                 + std::to_string(metadata.initial_greatest_q) + ", " + std::to_string(metadata.initial_greatest_delta) + ", "
                 + std::to_string(metadata.initial_greatest_sigma) + ", " + std::to_string(metadata.initial_greatest_ranking) + " and "
                 + std::to_string(metadata.initial_greatest_dens) + " were the greatest number of states, number of transitions, number of symbols, average rank per symbol and transition density observed;\n"

                + " - Quotienting imed out in "
                  + std::to_string(timeout.total)
                  + " (" + std::to_string(( (float) timeout.total / (float) metadata.total ) * (float) 100) + "%) "
                  + "of the automata. On average: \n"
                  + "    - the number of states at the end of each reduction was "
                  + std::to_string(testData.avg_q_reduction) + "%, the number of transitions was "
                    + std::to_string(testData.avg_delta_reduction) + "% and the transition density was "
                    + std::to_string(testData.avg_transDens_reduction) + "%; \n"
                    + "    - the most significant reduction in terms of number of states happened for an automaton whose number of states was reduced to "
                    + std::to_string(testData.greatest_q_reduction) + "%;\n"
                    + "    - the most significant reduction in terms of number of transitions happened for an automaton whose number of transitions was reduced to "
                    + std::to_string(testData.greatest_delta_reduction) + "%;\n"
                    + "    - the most significant reduction in terms of transition density happened for an automaton whose transition density at the end of the minimization was "
                    + std::to_string(testData.greatest_transDens_reduction) + "%;\n", log_humanread_filename);
        testData.printReductionBuckets(log_humanread_filename);

}

void heavy_tester_file(unsigned int la_dw, unsigned int la_up,
                       vector<filename> input_files, string output_dir, filename output_filename,
                       unsigned int max_numb_tests, int outputStat_type)
{

    MetaData metaData         =  MetaData();
    TestData testData_heavy   =  TestData();
    Timeout timeout_heavy     =  Timeout();

    const unsigned int numb_tests = max_numb_tests==0 ? input_files.size() : min(max_numb_tests,input_files.size());

    outputText("\n");
    for (unsigned int i=0; i<numb_tests; i++)
    {
        string filename = input_files.at(i);
        string local_filename = getLocalFilename(filename);

        /* Parsing and taking data from the automaton. */
        stateDict sDict;
        AutData autData = parseFromFile(filename, sDict);

        if (getNumbTransitions(getAut(autData)) == 0)
        {
            outputText("This automaton has 0 transitions and therefore will be skipped. \n");
            continue;
        }

        if (outputStat_type == STD_OUT_HUMAN)
            outputText("\tAutomaton " + std::to_string(i+1) + " - " + filename + ":\t");

        AutData autData_heavy = applyHeavy(autData, metaData, la_dw, la_up,
                                           testData_heavy, timeout_heavy, outputStat_type);

        /* Output resulting automaton (optional). */
        if (output_dir != "")
        {
            string aut_heavy_filename;
            if (output_filename != "")
                aut_heavy_filename = appendTimbukFilename(sanitizeDirStr(output_dir) + output_filename,
                                     "");
            else
                aut_heavy_filename = appendTimbukFilename(sanitizeDirStr(output_dir) + local_filename,
                                    "_minimized_with_Heavy(" + std::to_string(la_dw) + "," +
                                    std::to_string(la_up) + ")");
            saveAutToFile(autData_heavy, aut_heavy_filename);
        }

    }

    if (outputStat_type == STD_OUT_HUMAN)
    {
        outputText("\nAverage results:\t"
                   "Q_i: "               + metaData.initial_avg_q_str2Dec()
                   + "\tTrans_i: "         + metaData.initial_avg_delta_str2Dec()
                   + "\tTransDens_i: "     + metaData.initial_avg_transDens_str2Dec()
                   + "\tQ_f/Q_i: "         + testData_heavy.avg_q_red_str2Dec() + "%"
                   + "\tTrans_f/Trans_i: " + testData_heavy.avg_delta_red_str2Dec() + "%"
                   + "\tTD_f/TD_i: "       + testData_heavy.avg_transDens_red_str2Dec() + "%"
                   + "\tTime: "            + testData_heavy.avg_time_str2Dec() + "s"
                   + "\t");
        outputText("\nThe End\n");
    }

}

void heavy_tester_dir(unsigned int la_dw, unsigned int la_up,
                  string input_dir, string output_dir, unsigned int max_numb_tests,
                  int outputStat_type, string log_avgs)
{

    MetaData metaData         =  MetaData();
    TestData testData_heavy   =  TestData();
    Timeout timeout_heavy     =  Timeout();

    string log;
    if (outputStat_type == LOG)
    {
        log = "log_of_heavy(" + std::to_string(la_dw) + "," + std::to_string(la_up) +
                ")_input_folder:" + getLocalFilename(input_dir) + "_date:" + localTime() + ".txt";
        writeToFile(log,
                    "Each row of the following table contains results of an experiment on a particular tree automaton. The columns should be read as follows: \n"
                    "\t Filename - the name of the file containing the input tree automaton (TA) \n"
                    "\t Q_i - the number of states of TA (includes the implicit initial state) \n"
                    "\t Dt_i - the number of transitions of TA (includes the initial transitions) \n"
                    "\t TD_i - the transition density of TA (includes the initial state/transitions/symbols) \n"
                   // "\t (TOL_i_1,TOL_i_2,TOL_I_3) - the transitions-overlapping tuple in TA (consult the references in README.txt for more details) \n"
                    "\t Q_h(%) - the percentage of the number of states that remained after the application of the Heavy algorithm on TA \n"
                    "\t Dt_h(%) - the percentage of the number of transitions that remained after the application of the Heavy algorithm on TA \n"
                    "\t TD_h(%) - t.d. of Heavy(TA) / t.d. of TA \n"
                    "\t T_h - the time (in seconds) taken to minimize TA with Heavy \n \n");
        writeToFile(log, "Filename\tQ_i\tDt_i\tTD_i\t");
        // writeToFile(log, "(TOL_i_1,TOL_i_2,TOL_I_3)\t");
        writeToFile(log, "Q_h(%)\tDt_h(%)\tTD_h(%)\tT_h \n");
    }

    vector<filename> filenames = getAllFilenames(input_dir);
    const unsigned int numb_tests = max_numb_tests==0 ? filenames.size() : min(max_numb_tests,filenames.size());

    for (unsigned int i=0; i<numb_tests; i++)
    {
        string filename = filenames.at(i);
        string local_filename = getLocalFilename(filename);
        if (outputStat_type == LOG) writeToFile(log, local_filename + "\t");

        /* Parsing and taking data from the automaton. */
        stateDict stateDict;
        AutData autData = parseFromFile(filename, stateDict);

        if (getNumbTransitions(getAut(autData)) == 0)
        {
            string output_msg = "This automaton has 0 transitions and therefore will be skipped. \n";
            outputText(output_msg);
            continue;
        }

        if (outputStat_type == STD_OUT_HUMAN)
            outputText("\tAutomaton " + std::to_string(i+1) + " - " + filename + ": ");

        AutData autData_heavy = applyHeavy(autData, metaData, la_dw, la_up,
                                           testData_heavy, timeout_heavy, outputStat_type, log);

        /* Output resulting automaton (optional). */
        if (output_dir != "")
        {
            string aut_heavy_filename = appendTimbukFilename(sanitizeDirStr(output_dir) + local_filename,
                                    "_minimized_with_Heavy(" + std::to_string(la_dw) + "," +
                                    std::to_string(la_up) + ")");
            saveAutToFile(autData_heavy, aut_heavy_filename);
        }

    }

    if (outputStat_type == LOG)
        outputText("\n"
               "  Please consult the file ./" + log +
               " for individual experimental results concerning each automaton in the folder " +
               getLocalFilename(input_dir) + ".\n \n");

    if (outputStat_type == STD_OUT_HUMAN)
    {
        outputText("\nAverage results:\t"
                   "Q_i: "               + metaData.initial_avg_q_str2Dec()
                   + "\tTrans_i: "         + metaData.initial_avg_delta_str2Dec()
                   + "\tTransDens_i: "     + metaData.initial_avg_transDens_str2Dec()
                   + "\tQ_f/Q_i: "         + testData_heavy.avg_q_red_str2Dec() + "%"
                   + "\tTrans_f/Trans_i: " + testData_heavy.avg_delta_red_str2Dec() + "%"
                   + "\tTD_f/TD_i: "       + testData_heavy.avg_transDens_red_str2Dec() + "%"
                   + "\tTime: "            + testData_heavy.avg_time_str2Dec() + "s"
                   + "\t");
        outputText("\nThe End\n");
    }
    if (!log_avgs.empty())
        writeToFile(log_avgs,
                    getLocalFilename(input_dir) + "\t" +
                    metaData.initial_avg_q_str2Dec() + "\t" +
                    metaData.initial_avg_delta_str2Dec() + "\t" +
                    metaData.initial_avg_transDens_str2Dec() + "\t" +
                    //metaData.initial_avg_transOverlap_str() + "\t" +
                    testData_heavy.avg_q_red_str2Dec() + "\t" +
                    testData_heavy.avg_delta_red_str2Dec() + "\t" +
                    testData_heavy.avg_transDens_red_str2Dec() + "\t" +
                    testData_heavy.avg_time_str2Dec() + "\n");

}

void heavy_tester_dir(unsigned int la_dw, unsigned int la_up,
                      vector<string> input_dirs, string output_dir, unsigned int numb_tests,
                      int outputStat_type, string log_avgs_)
{
    string log_avgs;
    if (outputStat_type == LOG)
    {
        log_avgs = log_avgs_=="" ? "log_of_avg_values_of_applying_heavy(" + std::to_string(la_dw) + "," +
                           std::to_string(la_up) + ")_date:" + localTime() + ".txt" : log_avgs_;
        writeToFile(log_avgs, "Each row of the following table contains avg. result values "
                              "respecting experiments on the automata in a given directory. "
                              "The columns should be read as follows: \n"
                              "\t Folder - the name of the folder that this avg. values correspond to \n"
                              "\t Q_i - the avg. number of states of the TA (includes the implicit initial state) \n"
                              "\t Dt_i - the avg. number of transitions of the TA (includes the initial transitions) \n"
                              "\t TD_i - the avg. transition density of the TA (includes the initial state/transitions/symbols) \n"
    /*                          "\t (TOL_i_1,TOL_i_2,TOL_I_3) - the avg. transitions-overlapping tuple in TA (consult the references in README.txt for more details) \n"    */
                              "\t Q_h(%) - the avg. percentage of the number of states that remained after the application of the Heavy algorithm on the TA \n"
                              "\t Dt_h(%) - the avg. percentage of the number of transitions that remained after the application of the Heavy algorithm on the TA \n"
                              "\t TD_h(%) - the avg. relation (t.d. of Heavy(TA) / t.d. of the TA) \n"
                              "\t T_h - the avg. time (in seconds) taken to compute Heavy(TA) \n \n");
        writeToFile(log_avgs, "Folder\t\tQ_i\tDt_i\tTD_i\t"
                    "Q_h(%)\tDt_h(%)\tTD_h(%)\tT_h \n");
    }

    for (const string input_dir : input_dirs)
        heavy_tester_dir(la_dw, la_up, input_dir, output_dir, numb_tests, outputStat_type, log_avgs);

    if (outputStat_type == LOG)
    {
        writeToFile(log_avgs, "\nThe End\n");
        outputText("The End. Please consult file " + log_avgs + " for average result values for each input directory. \n");
    }
}

void procedure_tester(string input_dir, unsigned int max_numb_tests, string log_avgs)
{

    MetaData metaData         =  MetaData();
    TestData testData_ru      =  TestData();
    TestData testData_ruq     =  TestData();
    Timeout timeout_ruq       =  Timeout();
    TestData testData_ruqp    =  TestData();
    Timeout timeout_ruqp      =  Timeout();
    TestData testData_heavy11 =  TestData();
    Timeout timeout_heavy11   =  Timeout();
    TestData testData_heavy24 =  TestData();
    Timeout timeout_heavy24   =  Timeout();

    string log = "log_of_Procedure_input_folder:" + getLocalFilename(input_dir) + "_date:" + localTime() + ".txt";
    writeToFile(log, "Each row of the following table contains results of an experiment on a particular tree automaton. The columns should be read as follows: \n"
                          "\t Folder - the name of the folder that these values correspond to \n"
                          "\t Q_i - the number of states of the TA (includes the implicit initial state) \n"
                          "\t Dt_i - the number of transitions of the TA (includes the initial transitions) \n"
                          "\t Q_ru(%) - the percentage of the number of states that remained after removing useless (ru) states on the TA \n"
                          "\t Dt_ru(%) - the percentage of the number of transitions that remained after removing useless (ru) states on the TA \n"
                          "\t T_ru - the time (in seconds) taken to remove useless (ru) states on the TA \n"
                          "\t Q_ruq(%) - the percentage of the number of states that remained after ru + quotienting with dw-simulation the TA \n"
                          "\t Dt_ruq(%) - the percentage of the number of transitions that remained after ru + quotienting with dw-simulation the TA \n"
                          "\t T_ruq - the time (in seconds) taken to ru + quotient with dw-simulation the TA \n"
                          "\t Q_ruqp(%) - the percentage of the number of states that remained after ruq + pruning with P(id,strict dw-sim.) the TA \n"
                          "\t Dt_ruqp(%) - the percentage of the number of transitions that remained after ruq + pruning with P(id,strict dw-sim.) the TA \n"
                          "\t T_ruqp - the time (in seconds) taken to ruq + pruning with P(id,strict dw-sim.) the TA \n"
                          "\t Q_h(1,1)(%) - the percentage of the number of states that remained after the application of the Heavy(1,1) algorithm on the TA \n"
                          "\t Dt_h(1,1)(%) - the percentage of the number of transitions that remained after the application of the Heavy(1,1) algorithm on the TA \n"
                          "\t T_h(1,1) - the time (in seconds) taken to compute Heavy(1,1) on the TA \n"
                          "\t Q_h(2,4)(%) - the percentage of the number of states that remained after the application of the Heavy(2,4) algorithm on the TA \n"
                          "\t Dt_h(2,4)(%) - the percentage of the number of transitions that remained after the application of the Heavy(2,4) algorithm on the TA \n"
                          "\t T_h(2,4) - the time (in seconds) taken to compute Heavy(2,4) on the TA \n");
    writeToFile(log, "Folder\t\tQ_i\tDt_i\t"
                          "Q_ru(%)\tDt_ru(%)\tT_ru\t"
                          "Q_ruq(%)\tDt_ruq(%)\tT_ruq\t"
                          "Q_ruqp(%)\tDt_ruqp(%)\tT_ruqp\t"
                          "Q_h(1,1)(%)\tDt_h(1,1)(%)\tT_h(1,1)\t"
                          "Q_h(2,4)(%)\tDt_h(2,4)(%)\tT_h(2,4) \n");


    vector<filename> filenames = getAllFilenames(input_dir);
    const unsigned int numb_tests = max_numb_tests==0 ? filenames.size() : min(max_numb_tests,filenames.size());

    for (unsigned int i=0; i<numb_tests; i++)
    {
        string filename = filenames.at(i);
        string local_filename = getLocalFilename(filename);
        writeToFile(log, local_filename + "\t");

        string header  = "\tAutomaton " + std::to_string(i+1) + " - " + filename + ": ";
        outputText(header);

        /* Parsing and taking data from the automaton. */
        stateDict stateDict;
        AutData autData = parseFromFile(filename, stateDict);

        if (getNumbTransitions(getAut(autData)) == 0)
        {
            string output_msg = "This automaton has 0 transitions and therefore will be skipped. \n";
            outputText(output_msg);
            continue;
        }

        applyProcedure(autData, metaData, testData_ru,
                       testData_ruq, timeout_ruq, testData_ruqp, timeout_ruqp,
                       testData_heavy11, timeout_heavy11, testData_heavy24, timeout_heavy24, log, false, false);

        outputText("Done. \n");
    }

    outputText("\n"
               "  Please consult the file ./" + log +
               " for individual experimental results concerning each automaton in the folder " +
               getLocalFilename(input_dir) + ".\n \n");

    if (!log_avgs.empty())
        writeToFile(log_avgs,
                    getLocalFilename(input_dir) + "\t" +
                    metaData.initial_avg_q_str2Dec() + "\t" +
                    metaData.initial_avg_delta_str2Dec() + "\t" +
                    testData_ru.avg_q_red_str2Dec() + "\t" +
                    testData_ru.avg_delta_red_str2Dec() + "\t" +
                    testData_ru.avg_time_str2Dec() + "\t" +
                    testData_ruq.avg_q_red_str2Dec() + "\t" +
                    testData_ruq.avg_delta_red_str2Dec() + "\t" +
                    testData_ruq.avg_time_str2Dec() + "\t" +
                    testData_ruqp.avg_q_red_str2Dec() + "\t" +
                    testData_ruqp.avg_delta_red_str2Dec() + "\t" +
                    testData_ruqp.avg_time_str2Dec() + "\t" +
                    testData_heavy11.avg_q_red_str2Dec() + "\t" +
                    testData_heavy11.avg_delta_red_str2Dec() + "\t" +
                    testData_heavy11.avg_time_str2Dec() + "\t" +
                    testData_heavy24.avg_q_red_str2Dec() + "\t" +
                    testData_heavy24.avg_delta_red_str2Dec() + "\t" +
                    testData_heavy24.avg_time_str2Dec() + "\n");

}

void procedure_tester(vector<string> input_dirs, unsigned int numb_tests, string log_avgs_)
{
    string log_avgs = log_avgs_=="" ? "log_of_avg_values_of_applying_Procedure_date:" + localTime() + ".txt" : log_avgs_;
    writeToFile(log_avgs, "Each row of the following table contains avg. result values "
                          "respecting experiments on the automata in a given directory. "
                          "The columns should be read as follows: \n"
                          "\t Folder - the name of the folder that this avg. values correspond to \n"
                          "\t Q_i - the avg. number of states of the TA (includes the implicit initial state) \n"
                          "\t Dt_i - the avg. number of transitions of the TA (includes the initial transitions) \n"
                          "\t Q_ru(%) - the avg. percentage of the number of states that remained after removing useless (ru) states on the TA \n"
                          "\t Dt_ru(%) - the avg. percentage of the number of transitions that remained after removing useless (ru) states on the TA \n"
                          "\t T_ru - the avg. time (in seconds) taken to remove useless (ru) states on the TA \n"
                          "\t Q_ruq(%) - the avg. percentage of the number of states that remained after ru + quotienting with dw-simulation the TA \n"
                          "\t Dt_ruq(%) - the avg. percentage of the number of transitions that remained after ru + quotienting with dw-simulation the TA \n"
                          "\t T_ruq - the avg. time (in seconds) taken to ru + quotient with dw-simulation the TA \n"
                          "\t Q_ruqp(%) - the avg. percentage of the number of states that remained after ruq + pruning with P(id,strict dw-sim.) the TA \n"
                          "\t Dt_ruqp(%) - the avg. percentage of the number of transitions that remained after ruq + pruning with P(id,strict dw-sim.) the TA \n"
                          "\t T_ruqp - the avg. time (in seconds) taken to ruq + pruning with P(id,strict dw-sim.) the TA \n"
                          "\t Q_h(1,1)(%) - the avg. percentage of the number of states that remained after the application of the Heavy(1,1) algorithm on the TA \n"
                          "\t Dt_h(1,1)(%) - the avg. percentage of the number of transitions that remained after the application of the Heavy(1,1) algorithm on the TA \n"
                          "\t T_h(1,1) - the avg. time (in seconds) taken to compute Heavy(1,1) on the TA \n"
                          "\t Q_h(2,4)(%) - the avg. percentage of the number of states that remained after the application of the Heavy(2,4) algorithm on the TA \n"
                          "\t Dt_h(2,4)(%) - the avg. percentage of the number of transitions that remained after the application of the Heavy(2,4) algorithm on the TA \n"
                          "\t T_h(2,4) - the avg. time (in seconds) taken to compute Heavy(2,4) on the TA \n");
    writeToFile(log_avgs, "Folder\t\tQ_i\tDt_i\t"
                          "Q_ru(%)\tDt_ru(%)\tT_ru\t"
                          "Q_ruq(%)\tDt_ruq(%)\tT_ruq\t"
                          "Q_ruqp(%)\tDt_ruqp(%)\tT_ruqp\t"
                          "Q_h(1,1)(%)\tDt_h(1,1)(%)\tT_h(1,1)\t"
                          "Q_h(2,4)(%)\tDt_h(2,4)(%)\tT_h(2,4) \n");

    for (const string input_dir : input_dirs)
        procedure_tester(input_dir, numb_tests, log_avgs);

    writeToFile(log_avgs, "\nThe End\n");

    outputText("The End. Please consult file " + log_avgs + " for average result values for each input directory. \n");
}

void heavy_with_sat_tester_file(unsigned int la_dw, unsigned int la_up,
                                unsigned int la_dw_sat, unsigned int la_up_sat, unsigned int sat_version,
                                int max_attempts_sat,
                                vector<filename> input_files, string output_dir, unsigned int max_numb_tests,
                                int outputStat_type)
{
    MetaData metaData = MetaData();
    TestData testData = TestData();
    Timeout timeout   = Timeout();

    const unsigned int numb_tests = max_numb_tests==0 ? input_files.size() : min(max_numb_tests,input_files.size());

    outputText("\n");
    for (unsigned int i=0; i<numb_tests; i++)
    {
        string filename = input_files.at(i);
        string local_filename = getLocalFilename(filename);

        /* Parsing and taking data from the automaton. */
        stateDict sDict;
        AutData autData = parseFromFile(filename, sDict);

        if (getNumbTransitions(getAut(autData)) == 0)
        {
            string output_msg = "This automaton has 0 transitions and therefore will be skipped. \n";
            outputText(output_msg);
            continue;
        }

        if (outputStat_type == STD_OUT_HUMAN)
            outputText("\tAutomaton " + std::to_string(i+1) + " - " + filename + ":\t");

        AutData autData_final = applyHeavyWithSat(autData, la_dw, la_up,
                                                  la_dw_sat, la_up_sat, sat_version, max_attempts_sat,
                                                  metaData, testData, timeout, outputStat_type);

        /* Output resulting automaton (optional). */
        if (output_dir != "")
        {
            string aut_final_filename = appendTimbukFilename(sanitizeDirStr(output_dir) + local_filename,
                                    "_minimized_with_Heavy(" + std::to_string(la_dw) + "," +
                                    std::to_string(la_up) + ")_with_Saturation(" +
                                    std::to_string(la_dw_sat) + "," + std::to_string(la_up_sat) + ")");
            saveAutToFile(autData_final, sDict, aut_final_filename);
        }
    }

    if (outputStat_type == STD_OUT_HUMAN)
    {
        outputText("\nAverage results:\t"
                   "Q_i: "               + metaData.initial_avg_q_str2Dec()
                   + "\tTrans_i: "         + metaData.initial_avg_delta_str2Dec()
                   + "\tTransDens_i: "     + metaData.initial_avg_transDens_str2Dec()
                   + "\tQ_f/Q_i: "         + testData.avg_q_red_str2Dec() + "%"
                   + "\tTrans_f/Trans_i: " + testData.avg_delta_red_str2Dec() + "%"
                   + "\tTD_f/TD_i: "       + testData.avg_transDens_red_str2Dec() + "%"
                   + "\tTime: "            + testData.avg_time_str2Dec() + "s"
                   + "\t");
        outputText("\nThe End\n");
    }
}

void heavy_with_sat_tester_dir(unsigned int la_dw, unsigned int la_up,
                               unsigned int la_dw_sat, unsigned int la_up_sat, unsigned int sat_version,
                               int max_attempts_sat,
                               string input_dir, string output_dir, unsigned int max_numb_tests,
                               int outputStat_type, string log_avgs)
{
    MetaData metaData = MetaData();
    TestData testData = TestData();
    Timeout timeout   = Timeout();

    string log;
    if (outputStat_type == LOG)
    {
        log = "log_of_heavy(" + std::to_string(la_dw) + "," + std::to_string(la_up) +
            ")_with_saturation(" + std::to_string(la_dw_sat) + "," + std::to_string(la_up_sat) +
            ")_max_attempts_sat=" + std::to_string(max_attempts_sat) +
            "_input_folder:" + getLocalFilename(input_dir) + "_date:" + localTime() + ".txt";
        writeToFile(log,
                    "Each row of the following table contains results of an experiment on a particular tree automaton. The columns should be read as follows: \n"
                    "\t Filename - the name of the file containing the input tree automaton (TA) \n"
                    "\t Q_i - the number of states of TA (includes the implicit initial state) \n"
                    "\t Dt_i - the number of transitions of TA (includes the initial transitions) \n"
                    "\t TD_i - the transition density of TA (includes the initial state/transitions/symbols) \n"
                   // "\t (TOL_i_1,TOL_i_2,TOL_I_3) - the transitions-overlapping tuple in TA (consult the references in README.txt for more details) \n"
                    "\t Q_f(%) - the percentage of the number of states that remained after the application of Heavy+Saturation on TA \n"
                    "\t Dt_f(%) - the percentage of the number of transitions that remained after the application of Heavy+Saturation on TA \n"
                    "\t TD_f(%) - the t.d. of Heavy+Saturation(TA) / the t.d. of TA vs \n"
                    "\t T_f - the time (in seconds) taken to minimize TA with Heavy+Saturation \n \n");
        writeToFile(log, "Filename\tQ_i\tDt_i\tTD_i\t");
        // writeToFile(log, "(TOL_i_1,TOL_i_2,TOL_I_3)\t");
        writeToFile(log, "Q_f(%)\tDt_f(%)\tTD_f(%)\tT_f \n");
    }

    vector<filename> filenames = getAllFilenames(input_dir);
    const unsigned int numb_tests = max_numb_tests==0 ? filenames.size() : min(max_numb_tests,filenames.size());

    for (unsigned int i=0; i<numb_tests; i++)
    {
        string filename = filenames.at(i);
        string local_filename = getLocalFilename(filename);
        if (outputStat_type == LOG) writeToFile(log, local_filename + "\t");

        if (outputStat_type == STD_OUT_HUMAN)
            outputText("\tAutomaton " + std::to_string(i+1) + " - " + filename + ":\t");

        /* Parsing and taking data from the automaton. */
        stateDict sDict;
        AutData autData = parseFromFile(filename, sDict);

        if (getNumbTransitions(getAut(autData)) == 0)
        {
            string output_msg = "This automaton has 0 transitions and therefore will be skipped. \n";
            outputText(output_msg);
            continue;
        }

        AutData autData_final = applyHeavyWithSat(autData, la_dw, la_up,
                                                  la_dw_sat, la_up_sat, sat_version, max_attempts_sat,
                                                  metaData, testData, timeout, outputStat_type, log);

        /* Output resulting automaton (optional). */
        if (output_dir != "")
        {
            string aut_final_filename = appendTimbukFilename(sanitizeDirStr(output_dir) + local_filename,
                                    "_minimized_with_Heavy(" + std::to_string(la_dw) + "," +
                                    std::to_string(la_up) + ")_with_Saturation(" +
                                    std::to_string(la_dw_sat) + "," + std::to_string(la_up_sat) + ")");
            saveAutToFile(autData_final, sDict, aut_final_filename);
        }

    }

    if (outputStat_type == STD_OUT_HUMAN)
    {
        outputText("\nAverage results:\t"
                   "Q_i: "               + metaData.initial_avg_q_str2Dec()
                   + "\tTrans_i: "         + metaData.initial_avg_delta_str2Dec()
                   + "\tTransDens_i: "     + metaData.initial_avg_transDens_str2Dec()
                   + "\tQ_f/Q_i: "         + testData.avg_q_red_str2Dec() + "%"
                   + "\tTrans_f/Trans_i: " + testData.avg_delta_red_str2Dec() + "%"
                   + "\tTD_f/TD_i: "       + testData.avg_transDens_red_str2Dec() + "%"
                   + "\tTime: "            + testData.avg_time_str2Dec() + "s"
                   + "\t");
        outputText("\nThe End\n");
    }
    else if (outputStat_type == LOG)
        outputText("\n"
               "  Please consult the file ./" + log +
               " for individual experimental results concerning each automaton in the folder " +
               getLocalFilename(input_dir) + ".\n \n");

    if (!log_avgs.empty())
        writeToFile(log_avgs,
                    getLocalFilename(input_dir) + "\t" +
                    metaData.initial_avg_q_str2Dec() + "\t" +
                    metaData.initial_avg_delta_str2Dec() + "\t" +
                    metaData.initial_avg_transDens_str2Dec() + "\t" +
                    //metaData.initial_avg_transOverlap_str() + "\t" +
                    testData.avg_q_red_str2Dec() + "\t" +
                    testData.avg_delta_red_str2Dec() + "\t" +
                    testData.avg_transDens_red_str2Dec() + "\t" +
                    testData.avg_time_str2Dec() + "\n");

}

void heavy_with_sat_tester_dir(unsigned int la_dw, unsigned int la_up,
                               unsigned int la_dw_sat, unsigned int la_up_sat,
                               unsigned int sat_version, unsigned int max_attempts_sat,
                               vector<string> input_dirs, string output_dir, unsigned int numb_tests,
                               int outputStat_type, string log_avgs_)
{
    string log_avgs = "";
    if (outputStat_type == LOG)
    {
        log_avgs = log_avgs_=="" ? "log_of_avg_values_of_applying_heavy(" + std::to_string(la_dw) + "," +
                                          std::to_string(la_up) + ")_with_saturation(" +
                                          std::to_string(la_dw_sat) + "," + std::to_string(la_up_sat) +
                                          ")_max_attempts_sat=" + std::to_string(max_attempts_sat) +
                                          "_date:" + localTime() + ".txt" : log_avgs_;
        writeToFile(log_avgs, "Each row of the following table contains avg. result values "
                              "respecting experiments on the automata in a given directory. "
                              "The columns should be read as follows: \n"
                              "\t Folder - the name of the folder that this avg. values correspond to \n"
                              "\t Q_i - the avg. number of states of the TA (includes the implicit initial state) \n"
                              "\t Dt_i - the avg. number of transitions of the TA (includes the initial transitions) \n"
                              "\t TD_i - the avg. transition density of the TA (includes the initial state/transitions/symbols) \n"
    /*                          "\t (TOL_i_1,TOL_i_2,TOL_I_3) - the avg. transitions-overlapping tuple in TA (consult the references in README.txt for more details) \n"    */
                              "\t Q_f(%) - the avg. percentage of the number of states that remained after the application of Heavy+Saturation on the TA \n"
                              "\t Dt_f(%) - the avg. percentage of the number of transitions that remained after the application of Heavy+Saturation on the TA \n"
                              "\t TD_f(%) - the avg. relation (t.d. of the Heavy+Saturation(TA) / t.d. of the TA)   \n"
                              "\t T_f - the avg. time (in seconds) taken to compute Heavy+Saturation(TA) \n \n");
        writeToFile(log_avgs, "Folder\t\tQ_i\tDt_i\tTD_i\t"
                    "Q_f(%)\tDt_f(%)\tTD_f(%)\tT_f \n"); /* */
    }

    for (const string input_dir : input_dirs)
        heavy_with_sat_tester_dir(la_dw, la_up, la_dw_sat, la_up_sat, sat_version,
                                  max_attempts_sat, input_dir, output_dir, numb_tests, outputStat_type, log_avgs);

    if (outputStat_type == LOG)
    {
        writeToFile(log_avgs, "\nThe End\n");
        outputText("The End. Please consult file " + log_avgs + " for average result values for each input directory. \n");
    }
}

void heavy_with_sat_versionsComparer(unsigned int la_dw, unsigned int la_up,
                                     unsigned int la_dw_sat, unsigned int la_up_sat, int max_attempts_sat,
                                     string input_dir, unsigned int max_numb_tests, string log_avgs)
{
    MetaData metaData       = MetaData();
    TestData testData_h     = TestData();
    Timeout timeout_h       = Timeout();
    TestData testData_hs1   = TestData();
    Timeout timeout_hs1     = Timeout();
    TestData testData_hs1L  = TestData();
    Timeout timeout_hs1L    = Timeout();
    TestData testData_hs2   = TestData();
    Timeout timeout_hs2     = Timeout();
    TestData testData_hs2L  = TestData();
    Timeout timeout_hs2L    = Timeout();
    TestData testData_hs2L2 = TestData();
    Timeout timeout_hs2L2   = Timeout();

    string log = "log_of_comparing_heavy(" + std::to_string(la_dw) + "," +
            std::to_string(la_up) + ")_with_heavy_combined_with_different_versions_of_saturation(" +
            std::to_string(la_dw_sat) + "," + std::to_string(la_up_sat) + ")_max_attempts_sat=" +
            std::to_string(max_attempts_sat) + "_input_folder:"+ getLocalFilename(input_dir)+ "_date:" + localTime() + ".txt";
    writeToFile(log, "Each row of the following table contains avg. result values "
                     "respecting experiments on the automata in a given directory. "
                     "The columns should be read as follows: \n"
                     "\t Folder - the name of the folder that this values correspond to \n"
                     "\t Q_i - the number of states of the TA (includes the implicit initial state) \n"
                     "\t Dt_i - the number of transitions of the TA (includes the initial transitions) \n"
                     "\t TD_i - the transition density of the TA (includes the initial state/transitions/symbols) \n"
                     "\t Q_h(%) - the percentage of the number of states that remained after the application of Heavy on the TA \n"
                     "\t Dt_h(%) - the percentage of the number of transitions that remained after the application of Heavy on the TA \n"
                     "\t TD_h(%) - the avg. relation (the t.d. of the Heavy(TA) / the t.d. of the TA) \n"
                     "\t T_h - the time (in seconds) taken to compute Heavy(TA) \n"
                     "\t Q_h+s1(%) - the percentage of the number of states that remained after the application of Heavy+SaturationV1 on the TA \n"
                     "\t Dt_h+s1(%) - the percentage of the number of transitions that remained after the application of Heavy+SaturationV1 on the TA \n"
                     "\t TD_h+s1(%) - the avg. relation (the t.d. of the Heavy+SaturationV1(TA) / t.d. of the TA) \n"
                     "\t T_h+s1 - the time (in seconds) taken to compute Heavy+SaturationV1(TA) \n"
                     "\t Q_h+s1L(%) - the percentage of the number of states that remained after the application of Heavy+SaturationV1_with_inner_loop on the TA \n"
                     "\t Dt_h+s1L(%) - the percentage of the number of transitions that remained after the application of Heavy+SaturationV1_with_inner_loop on the TA \n"
                     "\t TD_h+s1L(%) - the avg. relation (t.d. of the Heavy+SaturationV1L(TA) / t.d. of the TA)  \n"
                     "\t T_h+s1L - the time (in seconds) taken to compute Heavy+SaturationV1_with_inner_loop(TA) \n"
                     "\t Q_h+s2(%) - the percentage of the number of states that remained after the application of Heavy+SaturationV2 on the TA \n"
                     "\t Dt_h+s2(%) - the percentage of the number of transitions that remained after the application of Heavy+SaturationV2 on the TA \n"
                     "\t TD_h+s2(%) - the avg. relation (t.d. of the Heavy+SaturationV2(TA) / t.d. of the TA) \n"
                     "\t T_h+s2 - the time (in seconds) taken to compute Heavy+SaturationV2(TA) \n"
                     "\t Q_h+s2L(%) - the percentage of the number of states that remained after the application of Heavy+SaturationV2_with_inner_loop on the TA \n"
                     "\t Dt_h+s2L(%) - the percentage of the number of transitions that remained after the application of Heavy+SaturationV2_with_inner_loop on the TA \n"
                     "\t TD_h+s2L(%) - the avg. relation (t.d. of the Heavy+SaturationV2_with_inner_loop(TA) / t.d. of the TA)  \n"
                     "\t T_h+s2L - the time (in seconds) taken to compute Heavy+SaturationV2_with_inner_loop(TA) \n"
                     "\t Q_h+s2L'(%) - the percentage of the number of states that remained after the application of Heavy+SaturationV2_with_inner_loop' on the TA \n"
                     "\t Dt_h+s2L'(%) - the percentage of the number of transitions that remained after the application of Heavy+SaturationV2_with_inner_loop' on the TA \n"
                     "\t TD_h+s2L'(%) - the avg. relation (t.d. of the Heavy+SaturationV2_with_inner_loop'(TA) / t.d. of the TA)  \n"
                     "\t T_h+s2L'' - the time (in seconds) taken to compute Heavy+SaturationV2_with_inner_loop'(TA) \n"
                     "\n");
    writeToFile(log, "Folder\t\tQ_i\tDt_i\tTD_i\t"
                     "Q_h(%)\tDt_h(%)\tTD_h(%)\tT_h\t"
                     "Q_h+s1(%)\tDt_h+s1(%)\tTD_h+s1(%)\tT_h+s1\t"
                     "Q_h+s1L(%)\tDt_h+s1L(%)\tTD_h+s1L(%)\tT_h+s1L\t"
                     "Q_h+s2(%)\tDt_h+s2(%)\tTD_h+s2(%)\tT_h+s2\t"
                     "Q_h+s2L(%)\tDt_h+s2L(%)\tTD_h+s2L(%)\tT_h+s2L\t"
                     "Q_h+s2L'(%)\tDt_h+s2L'(%)\tTD_h+s2L'(%)\tT_h+s2L'\t"
                     "\n");

    vector<filename> filenames = getAllFilenames(input_dir);
    const unsigned int numb_tests = max_numb_tests==0 ? filenames.size() : min(max_numb_tests,filenames.size());

    for (unsigned int i=0; i<numb_tests; i++)
    {
        string filename = filenames.at(i);
        string local_filename = getLocalFilename(filename);
        writeToFile(log, local_filename + "\t");

        string header  = "\tAutomaton " + std::to_string(i+1) + " - " + filename + ": ";
        outputText(header);

        /* Parsing and taking data from the automaton. */
        stateDict stateDict;
        AutData autData = parseFromFile(filename, stateDict);

        if (getNumbTransitions(getAut(autData)) == 0)
        {
            string output_msg = "This automaton has 0 transitions and therefore will be skipped. \n";
            outputText(output_msg);
            continue;
        }

        compareHeavyWithSatVersions(autData, metaData, la_dw, la_up, la_dw_sat, la_up_sat, max_attempts_sat,
                                    testData_h, timeout_h, testData_hs1, timeout_hs1, testData_hs1L, timeout_hs1L,
                                    testData_hs2, timeout_hs2, testData_hs2L, timeout_hs2L, testData_hs2L2, timeout_hs2L2,
                                    log);

        outputText("Done. \n");
    }

    outputText("\n"
               "  Please consult the file ./" + log +
               " for individual experimental results concerning each automaton in the folder " +
               getLocalFilename(input_dir) + ".\n \n");

    if (!log_avgs.empty())
        writeToFile(log_avgs,
                    getLocalFilename(input_dir) + "\t" +
                    metaData.initial_avg_q_str2Dec() + "\t" +
                    metaData.initial_avg_delta_str2Dec() + "\t" +
                    metaData.initial_avg_transDens_str2Dec() + "\t" +
                    testData_h.avg_q_red_str2Dec() + "\t" +
                    testData_h.avg_delta_red_str2Dec() + "\t" +
                    testData_h.avg_transDens_red_str2Dec() + "\t" +
                    testData_h.avg_time_str2Dec() + "\t" +
                    testData_hs1.avg_q_red_str2Dec() + "\t" +
                    testData_hs1.avg_delta_red_str2Dec() + "\t" +
                    testData_hs1.avg_transDens_red_str2Dec() + "\t" +
                    testData_hs1.avg_time_str2Dec() + "\t" +
                    testData_hs1L.avg_q_red_str2Dec() + "\t" +
                    testData_hs1L.avg_delta_red_str2Dec() + "\t" +
                    testData_hs1L.avg_transDens_red_str2Dec() + "\t" +
                    testData_hs1L.avg_time_str2Dec() + "\t" +
                    testData_hs2.avg_q_red_str2Dec() + "\t" +
                    testData_hs2.avg_delta_red_str2Dec() + "\t" +
                    testData_hs2.avg_transDens_red_str2Dec() + "\t" +
                    testData_hs2.avg_time_str2Dec() + "\t" +
                    testData_hs2L.avg_q_red_str2Dec() + "\t" +
                    testData_hs2L.avg_delta_red_str2Dec() + "\t" +
                    testData_hs2L.avg_transDens_red_str2Dec() + "\t" +
                    testData_hs2L.avg_time_str2Dec() + "\t" +
                    testData_hs2L2.avg_q_red_str2Dec() + "\t" +
                    testData_hs2L2.avg_delta_red_str2Dec() + "\t" +
                    testData_hs2L2.avg_transDens_red_str2Dec() + "\t" +
                    testData_hs2L2.avg_time_str2Dec() + "\t" +
                    "\n");

}

void heavy_with_sat_versionsComparer(unsigned int la_dw, unsigned int la_up,
                                     unsigned int la_dw_sat, unsigned int la_up_sat, unsigned int max_attempts_sat,
                                     const vector<string>& input_dirs, unsigned int numb_tests, string log_avgs_)
{
    string log_avgs = log_avgs_=="" ? "log_of_avg_values_of_comparing_heavy(" + std::to_string(la_dw) + "," +
                                      std::to_string(la_up) + ")_with_heavy_combined_with_different_versions_of_saturation(" +
                                      std::to_string(la_dw_sat) + "," + std::to_string(la_up_sat) +
                                      ")_max_attempts_sat=" + std::to_string(max_attempts_sat) +
                                      "_date:" + localTime() + ".txt" : log_avgs_;
    writeToFile(log_avgs, "Each row of the following table contains avg. result values "
                          "respecting experiments on the automata in a given directory. "
                          "The columns should be read as follows: \n"
                          "\t Folder - the name of the folder that this avg. values correspond to \n"
                          "\t Q_i - the avg. number of states of the TA (includes the implicit initial state) \n"
                          "\t Dt_i - the avg. number of transitions of the TA (includes the initial transitions) \n"
                          "\t TD_i - the avg. transition density of the TA (includes the initial state/transitions/symbols) \n"
                          "\t Q_h(%) - the avg. percentage of the number of states that remained after the application of Heavy on the TA \n"
                          "\t Dt_h(%) - the avg. percentage of the number of transitions that remained after the application of Heavy on the TA \n"
                          "\t TD_h(%) - the avg. relation (t.d. of the Heavy(TA) / t.d. of the TA) \n"
                          "\t T_h - the avg. time (in seconds) taken to compute Heavy(TA) \n"
                          "\t Q_h+s1(%) - the avg. percentage of the number of states that remained after the application of Heavy+SaturationV1 on the TA \n"
                          "\t Dt_h+s1(%) - the avg. percentage of the number of transitions that remained after the application of Heavy+SaturationV1 on the TA \n"
                          "\t TD_h+s1(%) - the avg. relation (t.d. of the Heavy+SaturationV1(TA) / t.d. of the TA) \n"
                          "\t T_h+s1 - the avg. time (in seconds) taken to compute Heavy+SaturationV1(TA) \n"
                          "\t Q_h+s1L(%) - the avg. percentage of the number of states that remained after the application of Heavy+SaturationV1_with_inner_loop on the TA \n"
                          "\t Dt_h+s1L(%) - the avg. percentage of the number of transitions that remained after the application of Heavy+SaturationV1_with_inner_loop on the TA \n"
                          "\t TD_h+s1L(%) - the avg. relation (t.d. of the Heavy+SaturationV1L(TA) / t.d. of the TA) \n"
                          "\t T_h+s1L - the avg. time (in seconds) taken to compute Heavy+SaturationV1_with_inner_loop(TA) \n"
                          "\t Q_h+s2(%) - the avg. percentage of the number of states that remained after the application of Heavy+SaturationV2 on the TA \n"
                          "\t Dt_h+s2(%) - the avg. percentage of the number of transitions that remained after the application of Heavy+SaturationV2 on the TA \n"
                          "\t TD_h+s2(%) - the avg. relation (t.d. of the Heavy+SaturationV2(TA) / t.d. of the TA) \n"
                          "\t T_h+s2 - the avg. time (in seconds) taken to compute Heavy+SaturationV2(TA) \n"
                          "\t Q_h+s2L(%) - the avg. percentage of the number of states that remained after the application of Heavy+SaturationV2_with_inner_loop on the TA \n"
                          "\t Dt_h+s2L(%) - the avg. percentage of the number of transitions that remained after the application of Heavy+SaturationV2_with_inner_loop on the TA \n"
                          "\t TD_h+s2L(%) - the avg. relation (t.d. of the Heavy+SaturationV2_with_inner_loop(TA) / t.d. of the TA) \n"
                          "\t T_h+s2L - the avg. time (in seconds) taken to compute Heavy+SaturationV2_with_inner_loop(TA) \n"
                          "\t Q_h+s2L'(%) - the avg. percentage of the number of states that remained after the application of Heavy+SaturationV2_with_inner_loop' on the TA \n"
                          "\t Dt_h+s2L'(%) - the avg. percentage of the number of transitions that remained after the application of Heavy+SaturationV2_with_inner_loop' on the TA \n"
                          "\t TD_h+s2L'(%) - the avg. relation (t.d. of the Heavy+SaturationV2_with_inner_loop'(TA) / t.d. of the TA) \n"
                          "\t T_h+s2L' - the avg. time (in seconds) taken to compute Heavy+SaturationV2_with_inner_loop'(TA) \n"
                          "\n");
    writeToFile(log_avgs, "Folder\t\tQ_i\tDt_i\tTD_i\t"
                          "Q_h(%)\tDt_h(%)\tTD_h(%)\tT_h\t"
                          "Q_h+s1(%)\tDt_h+s1(%)\tTD_h+s1(%)\tT_h+s1\t"
                          "Q_h+s1L(%)\tDt_h+s1L(%)\tTD_h+s1L(%)\tT_h+s1L\t"
                          "Q_h+s2(%)\tDt_h+s2(%)\tTD_h+s2(%)\tT_h+s2\t"
                          "Q_h+s2L(%)\tDt_h+s2L(%)\tTD_h+s2L(%)\tT_h+s2L\t"
                          "Q_h+s2L'(%)\tDt_h+s2L'(%)\tTD_h+s2L'(%)\tT_h+s2L'\t"
                          "\n");

    for (const string input_dir : input_dirs)
        heavy_with_sat_versionsComparer(la_dw, la_up, la_dw_sat, la_up_sat, max_attempts_sat, input_dir, numb_tests, log_avgs);

    writeToFile(log_avgs, "\nThe End\n");

    outputText("The End. Please consult file " + log_avgs + " for average result values for each input directory. \n");
}

void combined_relation_tester(unsigned int la_dw, unsigned int la_up,
                              vector<string> dir,
                              string summarized_results_filename = "",
                              string log_humanread_filename = "")
{    
    MetaData metaData = MetaData();
    TestData testData = TestData();
    Timeout timeout   = Timeout();

    string log_machread_filename =
            "results/logs/log_machine_readable_of_combined_relation(" + std::to_string(la_dw) + ","
            + std::to_string(la_up) + ")_tester_" + localTime();
    string log_machread_times_filename =
            "results/logs/log_machine_readable_of_combined_relation(" + std::to_string(la_dw) + ","
            + std::to_string(la_up) + ")_tester_times_" + localTime();

    writeToFile(log_machread_times_filename,
                "Name \t #Q_i \t #Dt_i \t Q reduc. (Combined_preorder("
                + std::to_string(la_dw) + ","
                + std::to_string(la_up) + ")) (%) \t Dt reduc. (Combined_preorder("
                + std::to_string(la_dw) + ","
                + std::to_string(la_up) + ")) (%) \t Times(s) (Combined_preorder("
                + std::to_string(la_dw) + ","
                + std::to_string(la_up) + "))  \n");

    vector<filename> filenames    = getAllFilenames(dir);
    const unsigned int NUMB_TESTS = filenames.size();

    for (unsigned int i=0; i<NUMB_TESTS  ; i++)
    {
        string autStr = VATA::Util::ReadFile(filenames.at(i));
        /* Parsing and taking data from the automaton. */
        stateDict sDict;
        AutData autData = parseFromString(autStr,sDict);

        if (getNumbTransitions(autData) == 0)
            continue;

        string header1 = "  Test " + std::to_string(i+1) + " - " + filenames.at(i) + ":\n";
        outputText(header1, log_humanread_filename);

        writeToFile(log_machread_times_filename,
                    std::to_string(i+1) + ": " + filenames.at(i) + "\t");

        writeToFile(log_machread_filename, "\n" + header1);
        string header2 = "Q reduction (%) \t Delta reduction (%) \t TransDens reduction (%) \n";
        writeToFile(log_machread_filename, header2);

        applyQuotCombinedPreorder(autData, metaData, la_dw, la_up, testData, timeout,
                                  log_humanread_filename, log_machread_filename, log_machread_times_filename);

    }

    printSummary_quotienting(la_dw, la_up, metaData, testData, timeout, log_humanread_filename);

    writeToFile((summarized_results_filename=="") ?
                    ("summarized_results_combined_relation(" + std::to_string(la_dw) + ","
                     + std::to_string(la_up) +").txt") : summarized_results_filename,
                dir.at(0) + "\t"
               + std::to_string(testData.avg_q_reduction) + "\t"
               + std::to_string(testData.avg_delta_reduction) + "\t"
               + std::to_string(testData.avg_time) + "\n");

}

