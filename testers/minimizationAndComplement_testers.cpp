
#include "minimizationAndComplement_testers.hh"


#include <iostream>     // provides std::cout
#include <utility>      // provides std::pair


void printSummary_complement_after_heavy_tester(unsigned int la_dw, unsigned int la_up,
                                                       MetaData& metadata, TestData& testData,
                                                       Timeout& timeout)
{

    outputText("\nSummary: We minimized (using Heavy(" + std::to_string(la_dw) + "," + std::to_string(la_up)
               + ")) and complemented a total of " + std::to_string(metadata.total) + " automata, with an average of "
               + std::to_string(metadata.initial_avg_q) + " states, "
               + std::to_string(metadata.initial_avg_delta) + " transitions, "
               + std::to_string(metadata.initial_avg_sigma) + " symbols, average rank per symbol of "
               + std::to_string(metadata.initial_avg_ranking) + " and transition density of "
               + std::to_string(metadata.initial_avg_dens) + ". "
               + std::to_string(metadata.initial_greatest_q) + ", "
               + std::to_string(metadata.initial_greatest_delta) + ", "
               + std::to_string(metadata.initial_greatest_sigma) + ", "
               + std::to_string(metadata.initial_greatest_ranking) + " and "
               + std::to_string(metadata.initial_greatest_dens)
               + " were the greatest number of states, number of transitions, number of symbols, average rank per symbol and transition density observed;\n"
               + "Heavy(" + std::to_string(la_dw) + "," + std::to_string(la_up) + ") timed out in "
               + std::to_string(timeout.total)
               + " (" + std::to_string(( (float) timeout.total / (float) metadata.total ) * (float) 100) + "%) "
               + "of the automata. On average: \n"
               + "    - the number of states at the end of each reduction dropped to "
               + std::to_string(testData.avg_q_reduction) + "%, the number of transitions dropped to "
               + std::to_string(testData.avg_delta_reduction) + "% and the transition density was "
               + std::to_string(testData.avg_transDens_reduction) + "% of the initial one. "
               + "On average, the minimization and the complement together took "
               + std::to_string(testData.avg_time) + " seconds; \n"
               + "    - the most significant reduction in terms of number of states happened for an automaton whose number of states was reduced to "
               + std::to_string(testData.greatest_q_reduction) + "%;\n"
               + "    - the most significant reduction in terms of number of transitions happened for an automaton whose number of transitions was reduced to "
               + std::to_string(testData.greatest_delta_reduction) + "%;\n"
               + "    - the most significant reduction in terms of transition density happened for an automaton whose transition density at the end of the minimization was "
               + std::to_string(testData.greatest_transDens_reduction) + "%;\n"
               );

}

tuple<AutData,stateDict> complement(const AutData& autData1, TestData& testData, string log)
{
    float seconds;
    tuple<AutData,stateDict> tup = complement(autData1,seconds);
    AutData autData2 = std::get<0>(tup);

    testData.inc();
    unsigned int q = getNumbUsedStates(autData2);
    unsigned int delta = getNumbTransitions(autData2);
    testData.updateAvgSizes(q, delta/*, td*/);
    testData.updateAvgTime(seconds);

    log_autSizes(log, q, delta);
    log_time(log, seconds);

    return tup;
}

tuple<AutData,stateDict> complement_after_heavy(const AutData& autData_i, stateDict& sDict, unsigned int la_dw, unsigned int la_up,
                               TestData& testData_h, TestData& testData_hAndc, TestData& testData_hAndcAndh, TestData& testData_hAndcAndhAnds,
                               string log, bool apply_heavy_in_the_end, bool apply_heavy_and_sat_in_the_end)
{
    /* Run Heavy(la_dw,la_up) on the aut and time it. */
    auto start_h = startTimer();
    AutData autData_h;
    try
    {
        autData_h = heavy(autData_i, la_dw, la_up);
    }
    catch (timeout_& e)
    {
        autData_h = wrapAutData(e.getAut(), getRanks(autData_i));
    }
    float time_h = elapsedSec(start_h);

    /* Record the statistical measures of the minimized aut. */
    testData_h.inc();
    float q_red  = measureStatesReduction(autData_h, autData_i);
    float dt_red = measureTransitionsReduction(autData_h, autData_i);
    testData_h.updateAvgReductions(q_red, dt_red/*, td_red*/);
    log_autReduction(log, q_red, dt_red/*, td_red*/);

    /* Run Complement on the minimized aut and time it. */
    float time_c;
    tuple<AutData,stateDict> tup_c = complement(autData_h, time_c);
    AutData autData_c = std::get<0>(tup_c);
    sDict = std::get<1>(tup_c);

    /* Record the statistical measures of the complemented aut. */
    testData_hAndc.inc();
    unsigned int q = getNumbUsedStates(autData_c);
    unsigned int delta = getNumbTransitions(autData_c);
    testData_hAndc.updateAvgSizes(q, delta/*, td*/);
    log_autSizes(log, q, delta);

    /* Record the accumulated time of the 2 operations. */
    testData_hAndc.updateAvgTime(time_h + time_c);
    log_time(log, time_h + time_c);

    if (apply_heavy_in_the_end)
    {
        /* Run Heavy(la_dw,la_up) on the complemented aut and time it. */
        auto start_h2 = startTimer();
        AutData autData_h2;
        try
        {
            autData_h2 = heavy(autData_c, la_dw, la_up);
        }
        catch (timeout_& e)
        {
            autData_h2 = wrapAutData(e.getAut(), getRanks(autData_c));
        }
        float time_h2 = elapsedSec(start_h2);

        /* Record the statistical measures of the minimized complement aut. */
        unsigned int q = getNumbUsedStates(autData_h2);
        unsigned int delta = getNumbTransitions(autData_h2);
        log_autSizes(log, q, delta);
        testData_hAndcAndh.inc();
        testData_hAndcAndh.updateAvgSizes(q, delta);

        /* Record the accumulated time of the 2 operations. */
        log_time(log, time_h + time_c + time_h2);
        testData_hAndcAndh.updateAvgTime(time_h + time_c + time_h2);

        autData_c = autData_h2;
    }

    if (apply_heavy_and_sat_in_the_end)
    {
        /* Run Heavy(la_dw,la_up)+Sat(la_dw,la_up) on the complemented aut and time it. */
        auto start_h3 = startTimer();
        AutData autData_h3;
        try
        {
            autData_h3 = applyHeavyWithSat(autData_c, la_dw, la_up, la_dw, la_up, 2, 1);
        }
        catch (timeout_& e)
        {
            autData_h3 = wrapAutData(e.getAut(), getRanks(autData_c));
        }
        float time_h3 = elapsedSec(start_h3);

        /* Record the statistical measures of the minimized complement aut. */
        unsigned int q = getNumbUsedStates(autData_h3);
        unsigned int delta = getNumbTransitions(autData_h3);
        log_autSizes(log, q, delta);
        testData_hAndcAndhAnds.inc();
        testData_hAndcAndhAnds.updateAvgSizes(q, delta);

        /* Record the accumulated time of the 2 operations. */
        log_time(log, time_h + time_c + time_h3);
        testData_hAndcAndhAnds.updateAvgTime(time_h + time_c + time_h3);

        autData_c = autData_h3;
    }

    tuple<AutData,stateDict> tup = std::make_tuple(autData_c,sDict);
    return tup;
}

tuple<AutData,stateDict> call_complement_after_heavy(const AutData& autData_i, stateDict& sDict, unsigned int la_dw, unsigned int la_up,
                                    MetaData& metaData,
                                    TestData& testData_h, TestData& testData_hAndc, TestData& testData_hAndcAndh, TestData& testData_hAndcAndhAnds, TestData& testData_c,
                                    string log, bool apply_heavy_in_the_end, bool apply_heavy_and_sat_in_the_end)
{
    metaData.inc();
    unsigned int q = getNumbUsedStates(autData_i);
    unsigned int delta = getNumbTransitions(autData_i);
    metaData.updateInitialAvg(q,delta);
    log_autSizes(log, q, delta);

    tuple<AutData,stateDict> tup_hAndc = complement_after_heavy(autData_i, sDict, la_dw, la_up, testData_h, testData_hAndc, testData_hAndcAndh, testData_hAndcAndhAnds, log,
                                               apply_heavy_in_the_end, apply_heavy_and_sat_in_the_end);

    complement(autData_i, testData_c, log);

    writeToFile(log, "\n");

    return tup_hAndc;
}

void complement_after_heavy_tester_file(unsigned int la_dw, unsigned int la_up, vector<string> input_files, string output_dir,
                                   unsigned int numb_tests, string log_avgs, bool apply_heavy_in_the_end, bool apply_heavy_and_sat_in_the_end)
{

    MetaData metaData           = MetaData();
    TestData testData_h         = TestData();
    TestData testData_hAndc     = TestData();
    TestData testData_hAndcAndh = TestData();
    TestData testData_hAndcAndhAnds = TestData();
    TestData testData_c         = TestData();

    string log = "log_of_comp_after_heavy(" + std::to_string(la_dw) + ","
            + std::to_string(la_up) + ")_date:" + localTime() + ".txt";
    writeToFile(log, "Each row of the following table contains results of an experiment on a particular tree automaton. The columns should be read as follows: \n"
                "\t Filename - the name of the file containing the input tree automaton (TA) \n"
                "\t Q_i - the number of states of TA (includes the implicit initial state) \n"
                "\t Dt_i - the number of transitions of TA (includes the initial transitions) \n"
               // "\t TD_i - the transition density of TA (includes the initial state/transitions/symbols) \n"
               // "\t OL_i - the transitions overlapping in TA \n"
                "\t Q_h(%) - the percentage of the number of states that remained after the application of the Heavy algorithm on TA \n"
                "\t Dt_h(%) - the percentage of the number of transitions that remained after the application of the Heavy algorithm on TA \n"
               // "\t TD_h(%) - the t.d. of TA vs the t.d. of Heavy(TA) \n"
                "\t Q_hc - the number of states in the complement of Heavy(TA) \n"
                "\t Dt_hc - the number of transitions in the complement of Heavy(TA) \n"
               // "\t TD_hc - the transition density of the complement of Heavy(TA) \n"
            //    "\t ND1_hc(%) - the percentage of states from which there are at least 2 transitions with the same symbol \n"
            //    "\t ND2_hc(%) - the percentage of transitions for which there is at least one other transition from the same state and with the same symbol \n"
                "\t T_hc - the time (in seconds) taken to compute Heavy(TA) and then its complement, altogether \n");
    if (apply_heavy_in_the_end)
        writeToFile(log,
                    "\t Q_hch(%) - the percentage of the number of states in the minimized version of the complement \n"
                    "\t Dt_hch(%) - the percentage of the number of transitions in the minimized version of the complement \n"
            //        "\t TD_hch(%) - the t.d. of the minimized version of the complement vs the t.d. of the complement \n"
            //        "\t ND1_hch(%) - the percentage of states from which there are at least 2 transitions with the same symbol \n"
            //        "\t ND2_hch(%) - the percentage of transitions for which there is at least one other transition from the same state and with the same symbol \n"
                    "\t T_hch - the time (in seconds) taken to compute Heavy(TA), its complement and then apply Heavy again, altogether \n");
    if (apply_heavy_and_sat_in_the_end)
        writeToFile(log,
                    "\t Q_hchs(%) - the percentage of the number of states in the result of applying Heavy+Sat the complement \n"
                    "\t Dt_hchs(%) - the percentage of the number of transitions in the result of applying Heavy+Sat the complement \n"
            //        "\t TD_hch(%) - the t.d. of the minimized version of the complement vs the t.d. of the complement \n"
            //        "\t ND1_hch(%) - the percentage of states from which there are at least 2 transitions with the same symbol \n"
            //        "\t ND2_hch(%) - the percentage of transitions for which there is at least one other transition from the same state and with the same symbol \n"
                    "\t T_hchs - the time (in seconds) taken to compute Heavy(TA), its complement and then apply Heavy+Sat, altogether \n");
    writeToFile(log,
                "\t Q_c - the number of states in the complement of TA \n"
                "\t Dt_c - the number of transitions in the complement of TA \n"
            //    "\t TD_c - the transition density of the complement of TA \n"
            //    "\t ND1_c(%) - the percentage of states from which there are at least 2 transitions with the same symbol \n"
            //    "\t ND2_c(%) - the percentage of transitions for which there is at least one other transition from the same state and with the same symbol \n"
                "\t T_c - the time (in seconds) taken to compute the complement of TA, alone \n \n");
    writeToFile(log, "Filename\tQ_i\tDt_i\t");
    // writeToFile(log_avgs, "TD_i\t");
    writeToFile(log, "Q_h(%)\tDt_h(%)\t");
    // writeToFile(log_avgs, "TD_h(%)\t");
    writeToFile(log, "Q_hc\tDt_hc\t");
    // writeToFile(log_avgs, "TD_hc\t");
    //writeToFile(log, "ND1_hc\tND2_hc\t");
    writeToFile(log, "T_hc\t");
    if (apply_heavy_in_the_end)
    {
        writeToFile(log, "Q_hch\tDt_hch\t");
        // writeToFile(log, "TD_hch\t");
        // writeToFile(log, "ND1_hch\tND2_hch\t");
        writeToFile(log, "T_hch\t");
    }
    if (apply_heavy_and_sat_in_the_end)
    {
        writeToFile(log, "Q_hchs\tDt_hchs\t");
        // writeToFile(log, "TD_hch\t");
        // writeToFile(log, "ND1_hch\tND2_hch\t");
        writeToFile(log, "T_hchs\t");
    }
    writeToFile(log, "Q_c\tDt_c\t");
    // writeToFile(log, "TD_c\t");
    // writeToFile(log, "ND1_c\tND2_c\t");
    writeToFile(log, "T_c \n");

    unsigned int numb_aut = !numb_tests ? input_files.size() : min(numb_tests, input_files.size());

    for (unsigned int i=0; i<numb_aut; i++)
    {
        /* Parsing and taking data from the automaton. */
        stateDict sDict;
        AutData autData_i = parseFromFile(input_files.at(i), sDict);

        if (getNumbTransitions(autData_i) == 0)
        {
            string output_msg = "This automaton has 0 transitions and therefore will be skipped. \n";
            outputText(output_msg);
            continue;
        }

        string local_filename = getLocalFilename(input_files.at(i));
        writeToFile(log, local_filename + "\t");

        string header1  = "\tAutomaton " + std::to_string(i+1) + " - " + input_files.at(i) + ": ";
        outputText(header1);

        tuple<AutData,stateDict> tup_hAndc = call_complement_after_heavy(autData_i, sDict, la_dw, la_up, metaData, testData_h, testData_hAndc, testData_hAndcAndh, testData_hAndcAndhAnds,
                                                                         testData_c, log, apply_heavy_in_the_end, apply_heavy_and_sat_in_the_end);
        AutData aut_hAndc = std::get<0>(tup_hAndc);

        /* Output resulting automaton (optional). */
        if (output_dir != "")
        {
            string aut_hAndc_filename = appendTimbukFilename(sanitizeDirStr(output_dir) + local_filename,
                                    "_minimized_with_Heavy(" + std::to_string(la_dw) + "," +
                                    std::to_string(la_up) + ")_and_complemented");
            if (apply_heavy_in_the_end || apply_heavy_and_sat_in_the_end) aut_hAndc_filename += "_and_minimized_again";
            saveAutToFile(aut_hAndc, aut_hAndc_filename);
        }

        outputText("Done. \n");
    }

    outputText("\n"
               "  Please consult the file ./" + log +
               " for individual experimental results concerning each automaton. \n");

    if (!log_avgs.empty())
        writeToFile(log_avgs,
                    metaData.initial_avg_q_str2Dec() + "\t" + metaData.initial_avg_delta_str2Dec() + "\t" +
                    // metaData.initial_avg_transDens_str2Dec() + "\t" +
                    testData_h.avg_q_red_str2Dec() + "\t" + testData_h.avg_delta_red_str2Dec() + "\t" +
                    // testData_h.avg_transDens_red_str2Dec() + "\t" +
                    testData_hAndc.avg_q_str2Dec() + "\t" + testData_hAndc.avg_delta_str2Dec() + "\t" +
                    // testData_hAndc.avg_transDens_str2Dec() + "\t" +
                    testData_hAndc.avg_time_str2Dec() + "\t" +
                    testData_hAndcAndh.avg_q_str2Dec() + "\t" + testData_hAndcAndh.avg_delta_str2Dec() + "\t" +
                    // testData_hAndc.avg_transDens_str2Dec() + "\t" +
                    testData_hAndcAndh.avg_time_str2Dec() + "\t" +
                    testData_hAndcAndhAnds.avg_q_str2Dec() + "\t" + testData_hAndcAndhAnds.avg_delta_str2Dec() + "\t" +
                    // testData_hAndc.avg_transDens_str2Dec() + "\t" +
                    testData_hAndcAndhAnds.avg_time_str2Dec() + "\t" +
                    testData_c.avg_q_str2Dec() + "\t" + testData_c.avg_delta_str2Dec() + "\t" +
                    // testData_c.avg_transDens_str2Dec() + "\t" +
                    testData_c.avg_time_str2Dec() + "\t" +
                    "\n");

}

void complement_after_heavy_tester_dir(unsigned int la_dw, unsigned int la_up, string input_dir, string output_dir,
                                   unsigned int numb_tests, string log_avgs, bool apply_heavy_in_the_end, bool apply_heavy_and_sat_in_the_end)
{

    MetaData metaData           = MetaData();
    TestData testData_h         = TestData();
    TestData testData_hAndc     = TestData();
    TestData testData_hAndcAndh = TestData();
    TestData testData_hAndcAndhAnds = TestData();
    TestData testData_c         = TestData();

    string log = "log_of_comp_after_heavy(" + std::to_string(la_dw) + ","
            + std::to_string(la_up) + ")_input_folder:" + getLocalFilename(input_dir) + "_date:" + localTime() + ".txt";
    writeToFile(log, "Each row of the following table contains results of an experiment on a particular tree automaton. The columns should be read as follows: \n"
                "\t Filename - the name of the file containing the input tree automaton (TA) \n"
                "\t Q_i - the number of states of TA (includes the implicit initial state) \n"
                "\t Dt_i - the number of transitions of TA (includes the initial transitions) \n"
               // "\t TD_i - the transition density of TA (includes the initial state/transitions/symbols) \n"
               // "\t OL_i - the transitions overlapping in TA \n"
                "\t Q_h(%) - the percentage of the number of states that remained after the application of the Heavy algorithm on TA \n"
                "\t Dt_h(%) - the percentage of the number of transitions that remained after the application of the Heavy algorithm on TA \n"
               // "\t TD_h(%) - the t.d. of TA vs the t.d. of Heavy(TA) \n"
                "\t Q_hc - the number of states in the complement of Heavy(TA) \n"
                "\t Dt_hc - the number of transitions in the complement of Heavy(TA) \n"
               // "\t TD_hc - the transition density of the complement of Heavy(TA) \n"
            //    "\t ND1_hc(%) - the percentage of states from which there are at least 2 transitions with the same symbol \n"
            //    "\t ND2_hc(%) - the percentage of transitions for which there is at least one other transition from the same state and with the same symbol \n"
                "\t T_hc - the time (in seconds) taken to compute Heavy(TA) and then its complement, altogether \n");
    if (apply_heavy_in_the_end)
        writeToFile(log,
                    "\t Q_hch(%) - the percentage of the number of states in the minimized version of the complement \n"
                    "\t Dt_hch(%) - the percentage of the number of transitions in the minimized version of the complement \n"
            //        "\t TD_hch(%) - the t.d. of the minimized version of the complement vs the t.d. of the complement \n"
            //        "\t ND1_hch(%) - the percentage of states from which there are at least 2 transitions with the same symbol \n"
            //        "\t ND2_hch(%) - the percentage of transitions for which there is at least one other transition from the same state and with the same symbol \n"
                    "\t T_hch - the time (in seconds) taken to compute Heavy(TA), its complement and then apply Heavy again, altogether \n");
    if (apply_heavy_and_sat_in_the_end)
        writeToFile(log,
                    "\t Q_hchs(%) - the percentage of the number of states in the result of applying Heavy+Sat the complement \n"
                    "\t Dt_hchs(%) - the percentage of the number of transitions in the result of applying Heavy+Sat the complement \n"
            //        "\t TD_hch(%) - the t.d. of the minimized version of the complement vs the t.d. of the complement \n"
            //        "\t ND1_hch(%) - the percentage of states from which there are at least 2 transitions with the same symbol \n"
            //        "\t ND2_hch(%) - the percentage of transitions for which there is at least one other transition from the same state and with the same symbol \n"
                    "\t T_hchs - the time (in seconds) taken to compute Heavy(TA), its complement and then apply Heavy+Sat, altogether \n");
    writeToFile(log,
                "\t Q_c - the number of states in the complement of TA \n"
                "\t Dt_c - the number of transitions in the complement of TA \n"
            //    "\t TD_c - the transition density of the complement of TA \n"
            //    "\t ND1_c(%) - the percentage of states from which there are at least 2 transitions with the same symbol \n"
            //    "\t ND2_c(%) - the percentage of transitions for which there is at least one other transition from the same state and with the same symbol \n"
                "\t T_c - the time (in seconds) taken to compute the complement of TA, alone \n \n");
    writeToFile(log, "Filename\tQ_i\tDt_i\t");
    // writeToFile(log_avgs, "TD_i\t");
    writeToFile(log, "Q_h(%)\tDt_h(%)\t");
    // writeToFile(log_avgs, "TD_h(%)\t");
    writeToFile(log, "Q_hc\tDt_hc\t");
    // writeToFile(log_avgs, "TD_hc\t");
    //writeToFile(log, "ND1_hc\tND2_hc\t");
    writeToFile(log, "T_hc\t");
    if (apply_heavy_in_the_end)
    {
        writeToFile(log, "Q_hch\tDt_hch\t");
        // writeToFile(log, "TD_hch\t");
        // writeToFile(log, "ND1_hch\tND2_hch\t");
        writeToFile(log, "T_hch\t");
    }
    if (apply_heavy_and_sat_in_the_end)
    {
        writeToFile(log, "Q_hchs\tDt_hchs\t");
        // writeToFile(log, "TD_hch\t");
        // writeToFile(log, "ND1_hch\tND2_hch\t");
        writeToFile(log, "T_hchs\t");
    }
    writeToFile(log, "Q_c\tDt_c\t");
    // writeToFile(log, "TD_c\t");
    // writeToFile(log, "ND1_c\tND2_c\t");
    writeToFile(log, "T_c \n");

    vector<filename> filenames = getAllSubFilenames(input_dir);
    unsigned int numb_aut = !numb_tests ? filenames.size() : min(numb_tests, filenames.size());

    for (unsigned int i=0; i<numb_aut; i++)
    {        
        /* Parsing and taking data from the automaton. */
        stateDict sDict;
        AutData autData_i = parseFromFile(filenames.at(i), sDict);

        if (getNumbTransitions(autData_i) == 0)
        {
            string output_msg = "This automaton has 0 transitions and therefore will be skipped. \n";
            outputText(output_msg);
            continue;
        }

        string local_filename = getLocalFilename(filenames.at(i));
        writeToFile(log, local_filename + "\t");

        string header1  = "\tAutomaton " + std::to_string(i+1) + " - " + filenames.at(i) + ": ";
        outputText(header1);

        tuple<AutData,stateDict> tup_hAndc = call_complement_after_heavy(autData_i, sDict, la_dw, la_up, metaData, testData_h, testData_hAndc, testData_hAndcAndh, testData_hAndcAndhAnds,
                                                                         testData_c, log, apply_heavy_in_the_end, apply_heavy_and_sat_in_the_end);
        AutData aut_hAndc = std::get<0>(tup_hAndc);

        /* Output resulting automaton (optional). */
        if (output_dir != "")
        {
            string output_fulldir = output_dir + "/" + getLocalFilename(input_dir);
            createDir(output_fulldir);

            string aut_hAndc_filename = appendTimbukFilename(sanitizeDirStr(output_fulldir) + local_filename,
                                    "_minimized_with_Heavy(" + std::to_string(la_dw) + "," +
                                    std::to_string(la_up) + ")_and_complemented");
            if (apply_heavy_in_the_end || apply_heavy_and_sat_in_the_end) aut_hAndc_filename += "_and_minimized_again";
            saveAutToFile(aut_hAndc, aut_hAndc_filename);
        }

        outputText("Done. \n");
    }

    outputText("\n"
               "  Please consult the file ./" + log +
               " for individual experimental results concerning each automaton in the folder " +
               getLocalFilename(input_dir) + ".\n \n");

    if (!log_avgs.empty())
        writeToFile(log_avgs,
                    getLocalFilename(input_dir) + "\t" +
                    metaData.initial_avg_q_str2Dec() + "\t" + metaData.initial_avg_delta_str2Dec() + "\t" +
                    // metaData.initial_avg_transDens_str2Dec() + "\t" +
                    testData_h.avg_q_red_str2Dec() + "\t" + testData_h.avg_delta_red_str2Dec() + "\t" +
                    // testData_h.avg_transDens_red_str2Dec() + "\t" +
                    testData_hAndc.avg_q_str2Dec() + "\t" + testData_hAndc.avg_delta_str2Dec() + "\t" +
                    // testData_hAndc.avg_transDens_str2Dec() + "\t" +
                    testData_hAndc.avg_time_str2Dec() + "\t" +
                    testData_hAndcAndh.avg_q_str2Dec() + "\t" + testData_hAndcAndh.avg_delta_str2Dec() + "\t" +
                    // testData_hAndc.avg_transDens_str2Dec() + "\t" +
                    testData_hAndcAndh.avg_time_str2Dec() + "\t" +
                    testData_hAndcAndhAnds.avg_q_str2Dec() + "\t" + testData_hAndcAndhAnds.avg_delta_str2Dec() + "\t" +
                    // testData_hAndc.avg_transDens_str2Dec() + "\t" +
                    testData_hAndcAndhAnds.avg_time_str2Dec() + "\t" +
                    testData_c.avg_q_str2Dec() + "\t" + testData_c.avg_delta_str2Dec() + "\t" +
                    // testData_c.avg_transDens_str2Dec() + "\t" +
                    testData_c.avg_time_str2Dec() + "\t" +
                    "\n");

}

void complement_after_heavy_tester_dir(unsigned int la_dw, unsigned int la_up,
                                   vector<string> input_dirs, string output_dir, unsigned int numb_tests, bool apply_heavy_in_the_end, bool apply_heavy_and_sat_in_the_end)
{
    string log_avgs = "log_of_avg_values_of_comp_after_heavy(" + std::to_string(la_dw) + "," +
                       std::to_string(la_up) + ")_date:" + localTime() + ".txt";
    writeToFile(log_avgs, "Each row of the following table contains avg. result values "
                          "respecting experiments on the automata in a given directory. "
                          "The columns should be read as follows: \n"
                          "\t Folder - the name of the folder that this avg. values correspond to \n"
                          "\t Q_i - the avg. number of states of the TA (includes the implicit initial state) \n"
                          "\t Dt_i - the avg. number of transitions of the TA (includes the initial transitions) \n"
                          // "\t TD_i - the avg. transition density of the TA (includes the initial state/transitions/symbols) \n"
                          // "\t OL_i - the transitions overlapping in TA \n"
                          "\t Q_h(%) - the avg. percentage of the number of states that remained after the application of the Heavy algorithm on the TA \n"
                          "\t Dt_h(%) - the avg. percentage of the number of transitions that remained after the application of the Heavy algorithm on the TA \n"
                          // "\t TD_h(%) - the avg. relation: t.d. of the TA vs the t.d. of the Heavy(TA) \n"
                          "\t Q_hc - the avg. number of states in the complement of Heavy(TA) \n"
                          "\t Dt_hc - the avg. number of transitions in the complement of Heavy(TA) \n"
                          // "\t TD_hc - the avg. transition density of the complement of Heavy(TA) \n"
                        //  "\t ND1_hc(%) - the percentage of states from which there are at least 2 transitions with the same symbol \n"
                        //  "\t ND2_hc(%) - the percentage of transitions for which there is at least one other transition from the same state and with the same symbol \n"
                          "\t T_hc - the avg. time (in seconds) taken to compute Heavy(TA) and then its complement, altogether \n");
    if (apply_heavy_in_the_end)
        writeToFile(log_avgs,
                          "\t Q_hch - the avg. number of states in the minimized complement of Heavy(TA) \n"
                          "\t Dt_hch - the avg. number of transitions in the minimized complement of Heavy(TA) \n"
                          "\t T_hch - the avg. time (in seconds) taken to compute Heavy(TA), then its complement and then Heavy again, altogether \n");
    if (apply_heavy_and_sat_in_the_end)
        writeToFile(log_avgs,
                          "\t Q_hchs - the avg. number of states in the result of applying Heavy+Sat to the complement of Heavy(TA) \n"
                          "\t Dt_hchs - the avg. number of transitions in the result of applying Heavy+Sat to the complement of Heavy(TA) \n"
                          "\t T_hchs - the avg. time (in seconds) taken to compute Heavy(TA), then its complement and then Heavy+Sat, altogether \n");
    writeToFile(log_avgs,
                          "\t Q_c - the avg. number of states in the complement of TA \n"
                          "\t Dt_c - the avg. number of transitions in the complement of TA \n"
                          // "\t TD_c - the avg. transition density of the complement of TA \n"
                        //  "\t ND1_c(%) - the percentage of states from which there are at least 2 transitions with the same symbol \n"
                        //  "\t ND2_c(%) - the percentage of transitions for which there is at least one other transition from the same state and with the same symbol \n"
                          "\t T_c - the avg. time (in seconds) taken to compute the complement of TA, alone \n \n");
    writeToFile(log_avgs, "Folder\tQ_i\tDt_i\t");
    // writeToFile(log_avgs, "TD_i\t");
    writeToFile(log_avgs, "Q_h(%)\tDt_h(%)\t");
    // writeToFile(log_avgs, "TD_h(%)\t");
    writeToFile(log_avgs, "Q_hc\tDt_hc\t");
    // writeToFile(log_avgs, "TD_hc\t");
    writeToFile(log_avgs, "T_hc\t");
    if (apply_heavy_in_the_end)
    {
        writeToFile(log_avgs, "Q_hch\tDt_hch\t");
        // writeToFile(log_avgs, "TD_hch\t");
        writeToFile(log_avgs, "T_hch\t");
    }
    if (apply_heavy_and_sat_in_the_end)
    {
        writeToFile(log_avgs, "Q_hchs\tDt_hchs\t");
        // writeToFile(log_avgs, "TD_hchs\t");
        writeToFile(log_avgs, "T_hchs\t");
    }
    writeToFile(log_avgs, "Q_c\tDt_c\t");
    // writeToFile(log_avgs, "TD_c\t");
    writeToFile(log_avgs, "T_c \n");

    for (const string input_dir : input_dirs)
        complement_after_heavy_tester_dir(la_dw, la_up, input_dir, output_dir, numb_tests, log_avgs, apply_heavy_in_the_end, apply_heavy_and_sat_in_the_end);

    writeToFile(log_avgs, "\nThe End\n");

    outputText("The End. Please consult file " + log_avgs + " for average result values for each input directory. \n");
}
