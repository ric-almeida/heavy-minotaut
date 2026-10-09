
#include "relationsComputation_testers.hh"


void compute_la_dw_sim_tester(unsigned int la, string input_dir, unsigned int max_numb_tests, string log_avgs)
{
    TestData testData   =  TestData(1,0,0);
    Timeout timeout     =  Timeout();

    string log = "log_of_computing_" + std::to_string(la)
            + "-dw-sim_using_HGA=" + getOPT(TYPE_OF_HISTORY_OF_GOOD_ATKS)
            + "_and_HBA=" + getOPT(TYPE_OF_HISTORY_OF_BAD_ATKS)
            + (pr_depth != AUTO_PR ? ("_and_BPR=" + std::to_string(pr_depth)) : "")
            + "_input_folder:" + getLocalFilename(input_dir)
            + "_date:" + localTime() + ".txt";
    writeToFile(log,
                "Each row of the following table contains results of an experiment on a particular tree automaton. The columns should be read as follows: \n"
                "\t Filename - the name of the file containing the input tree automaton (TA) \n"
               // "\t Q_i - the number of states of TA \n"
               // "\t Dt_i - the number of transitions of TA \n"
               // "\t TD_i - the transition density of TA \n"
               // "\t (TOL_i_1,TOL_i_2,TOL_I_3) - the transitions-overlapping tuple in TA (consult the references in README.txt for more details) \n"
                "\t |R| - the cardinality of the downward simulation \n"
                "\t T - the time (in seconds) taken to compute the downward simulation \n \n");
    writeToFile(log, "Filename\t"
                "|R|\tT \n");

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

        vector<vector<bool>> W;
        try
        {
            auto start = startTimer();
            W = dw_simulation(autData, la, NULL, true, start);
            float time = elapsedSec(start);
            unsigned int W_card = getSizeOfRel(W);
            testData.inc();
            testData.updateAvgTime(time);
            testData.updateAvgSizesRels({W_card});
            writeToFile(log, std::to_string(W_card) + "\t");
            log_time(log, time);

        }
        catch (timeout_ & e)
        {
            timeout.inc();
            writeToFile(log, "0\tTO");
        }
        writeToFile(log, "\n");

        outputText("Done. \n");
    }

    outputText("\n"
               "  Please consult the file ./" + log +
               " for individual experimental results concerning each automaton in the folder " +
               getLocalFilename(input_dir) + ".\n \n");

    if (!log_avgs.empty())
        writeToFile(log_avgs,
                    getLocalFilename(input_dir) + "\t" +
                    testData.avg_time_str2Dec() + "\t" +
                    std::to_string(timeout.total) + "\t" +
                    testData.avg_sizes_relations_str2Dec().at(0) + "\n");

}

void compute_la_dw_sim_tester(unsigned int la, vector<string> input_dirs, unsigned int numb_tests, string log_avgs_)
{
    string log_avgs = log_avgs_=="" ? "log_of_avg_values_of_computing_" + std::to_string(la)
                                      + "-downward-simulation_using_HGA=" + getOPT(TYPE_OF_HISTORY_OF_GOOD_ATKS)
                                      + "_and_HBA=" + getOPT(TYPE_OF_HISTORY_OF_BAD_ATKS)
                                      + (pr_depth != AUTO_PR ? ("_and_BPR=" + std::to_string(pr_depth)) : "")
                                      + "_date:" + localTime() + ".txt" : log_avgs_;
    writeToFile(log_avgs, "Each row of the following table contains avg. result values "
                          "respecting experiments on the automata in a given directory. "
                          "The columns should be read as follows: \n"
                          "\t Folder - the name of the folder that this avg. values correspond to \n"
                         // "\t Q_i - the avg. number of states of the TA \n"
                         // "\t Dt_i - the avg. number of transitions of the TA \n"
                         // "\t TD_i - the avg. transition density of the TA \n"
/*                        "\t (TOL_i_1,TOL_i_2,TOL_I_3) - the avg. transitions-overlapping tuple in TA (consult the references in README.txt for more details) \n"    */
                          "\t |R| - the avg. cardinality of the downward simulation \n"
                          "\t T - the avg. time (in seconds) taken to compute the lookahead simulation \n "
                          "\t TO - the number of instances that timed out (and therefore are not considered for the previous two measures) \n \n");
    writeToFile(log_avgs, "Folder\tT\tTO\t|R| \n");

    for (const string input_dir : input_dirs)
        compute_la_dw_sim_tester(la, input_dir, numb_tests, log_avgs);

    writeToFile(log_avgs, "\nThe End\n");

    outputText("The End. Please consult file " + log_avgs + " for average result values for each input directory. \n");
}

