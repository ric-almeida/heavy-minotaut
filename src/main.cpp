
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Main file, which calls a user interface procedure.                          *
 * 																					*
 ************************************************************************************/

#include "../include/main.hh"
#include <boost/lambda/lambda.hpp>
#include <iterator>
#include <algorithm>

bool dbg = false;
/* After being declared in its header, a global variable must be defined in a different file.
 * Any files that then make use of the value of this variable must do an include of the first header. */
seconds TIMEOUT = 30*60;

void printHelp(char* argv[])
{
    std::cout << "\n"
              << "Please use one of the following: \n \n "
              << "\t - '" << argv[0] << " minimize [ options ] la-dw la-up input1 input2 ...' "
                    << "to apply Heavy(la-dw,la-up) to each automaton provided. \n"
                    << "\t Options are as follows: \n"
                      << "\t\t - [-input_format=file/dir]: " << "defines whether each input is an automaton file (with timbuk syntax) or a dir containing (only) automata files, all of which will be processed (default: file). \n"
                      << "\t\t - [-output=o_dir]: " << "if provided, each automaton will be minimized and saved in the output_dir as a file named "
                      << "ta_minimized_with_Heavy(x,y).timbuk, where ta.timbuk is the name of the input file (default: no output). \n"
                      << "\t\t - [-outputFormat=human/machine/log]: " << "outputs the results in a user-friendly or machine-friendly manner (stdout) or to a log file (default: human). \n"
                      << "\t\t - [-max_numb_tests=n]: " << "to limit to n the number of automata to process in each i_dir. \n"
                      << "\t\t - [-timeout=t]: " << "to limit to t seconds each application of the Heavy algorithm (default: " << std::to_string(TIMEOUT) << "). \n"
                      << "\t\t - [-BranchPR=d]: " << "(advanced) to set to d the depth of the pre-refinement performed before each computation of the lookahead downward simulation (default: 1 for lookaheads 2 or 3; 2 for lookaheads 4 or 5; 0 otherwise). \n"
                      << "\t\t - [-HGA_strategy=global/semi-global/local/none]: " << "(advanced) to set the type of caching for the history of good attacks (default: none). \n"
                      << "\t\t - [-HBA_strategy=global/semi-global/local/none]: " << "(advanced) to set the type of caching for the history of bad attacks (default: none). \n"
                      << "\n"
              << "\t - '" << argv[0] << " minimize_with_saturation [ options ] la-dw la-up la-dw-sat la-up-sat input1 input2 ...' "
                    << "to apply Heavy(la-dw,la-up)_with_Sat(la-dw-sat,la-up-sat) to each automaton provided. \n"
                    << "\t Options are as follows: \n"
                      << "\t\t - [-sat_version=j]: " << " the version of Sat to use (default: 2). \n"
                      << "\t\t - [-input_format=file/dir]: " << "defines whether each input is an automaton file (with timbuk syntax) or a dir containing (only) automata files, all of which will be processed (default: file). \n"
                      << "\t\t - [-output=o_dir]: " << "if provided, each automaton will be minimized and saved in the output_dir as a file named "
                      << "ta_minimized_with_Heavy(x,y)_with_SatJ(z,w).timbuk, where ta.timbuk is the input automaton file and J the version of Sat being used (default: no output). \n"
                      << "\t\t - [-outputFormat=human/machine/log]: " << "outputs the results in a user-friendly or machine-friendly manner (stdout) or to a log file (default: human). \n"
                      << "\t\t - [-max_numb_tests=n]: " << "to limit to n the number of automata to process in each i_dir. \n"
                      << "\t\t - [-timeout=t]: " << "to limit to t seconds each application of the Heavy_with_Sat algorithm (default: " << std::to_string(TIMEOUT) << "). \n"
                      << "\t\t - [-BranchPR=d]: " << "(advanced) to set to d the depth of the pre-refinement performed before each computation of the lookahead downward simulation (default: 1 for lookaheads 2 or 3; 2 for lookaheads 4 or 5; 0 otherwise). \n"
                      << "\t\t - [-HGA_strategy=global/semi-global/local/none]: " << "(advanced) to set the type of caching for the history of good attacks (default: none). \n"
                      << "\t\t - [-HBA_strategy=global/semi-global/local/none]: " << "(advanced) to set the type of caching for the history of bad attacks (default: none). \n"
                      << "\n"
              << "\t - '" << argv[0] << " minimize_and_complement [ options ] la-dw la-up input1 input2 ...' "
                    << "to apply Heavy(la-dw,la-up) to each automaton provided, "
                    << "and complement it using libvata's algorithm. For most automata, the minimization "
                    << "significantly speeds up the complementing operation. \n"
                    << "\t Options are as follows: \n"
                      << "\t\t - [-input_format=file/dir]: " << "defines whether each input is an automaton file (with timbuk syntax) or a dir containing (only) automata files, all of which will be processed (default: file). \n"
                      << "\t\t - [-output=o_dir]: " << "if provided, each automaton will "
                      << "be minimized and complemented and saved in the output_dir as a file named "
                      << "ta_minimized_with_Heavy(x,y)_and_complemented.timbuk, where ta.timbuk is the input file (default: no output). \n"
                      << "\t\t - [-max_numb_tests=n]: " << "to limit to n the number of automata to process in each i_dir. \n"
                      << "\t\t - [-timeout=t]: " << "to limit to t seconds each application of the Heavy algorithm (default: " << std::to_string(TIMEOUT) << "). \n"
                      << "\t\t - [-applyHeavyInTheEnd=true/false]: " << "to apply Heavy(la-dw,la-up) to the complemented automaton as well (default: false). \n"
                      << "\t\t - [-applyHeavyAndSatInTheEnd=true/false]: " << "to apply Heavy(la-dw,la-up)_with_Sat2(la-dw,la-up) to the complemented automaton as well (default: false). \n"
                      << "\t\t - [-BranchPR=d]: " << "(advanced) to set to d the depth of the pre-refinement performed before each computation of the lookahead downward simulation (default: 1 for lookaheads 2 or 3; 2 for lookaheads 4 or 5; 0 otherwise). "
                      << "\t\t - [-HGA_strategy=global/semi-global/local/none]: " << "(advanced) to set the type of caching for the history of good attacks (default: none). \n"
                      << "\t\t - [-HBA_strategy=global/semi-global/local/none]: " << "(advanced) to set the type of caching for the history of bad attacks (default: none). \n"
                      << "\n"
              << "\t - '" << argv[0] << " compute_la-dw-sim [ options ] la input_dir1 input_dir2 ...' "
                    << "to compute the downward simulation preorder with lookahead=la for each automaton in the input dirs provided." << "\n"
                    << "\t Options are as follows: \n"
                      << "\t\t - [-max_numb_tests=n]: " << "to limit to n the number of automata to process in each input_dir. \n"
                      << "\t\t - [-timeout=t]: " << "to limit to t seconds each computation of a lookahead downward simulation preorder (default: " << std::to_string(TIMEOUT) << "). \n"
                      << "\t\t - [-BranchPR=d]: " << "(advanced) to set to d the depth of the pre-refinement performed before each computation of the lookahead downward simulation (default: 1 for lookaheads 2 or 3; 2 for lookaheads 4 or 5; 0 otherwise). "
                      << "\t\t - [-HGA_strategy=global/semi-global/local/none]: " << "(advanced) to set the type of caching for the history of good attacks (default: none). \n"
                      << "\t\t - [-HBA_strategy=global/semi-global/local/none]: " << "(advanced) to set the type of caching for the history of bad attacks (default: none). \n"
                      << "\n"
              << "\t - '" << argv[0] << " complement [ options ] input1 input2 ...' "
                    << "to compute the complement of the automata provided in the input files." << "\n"
                    << "\t Options are as follows: \n"
                      << "\t\t - [-output=o_dir]: " << "if provided, each automaton will "
                      << "be complemented and saved in the output_dir as a file named "
                      << "ta_complemented.timbuk, where ta.timbuk is the input file (default: no output). \n"
                      << "\n"
              << "\t - '" << argv[0] << " measure_size [ options ] input_aut' "
                    << "to calculate the basic measures of an automaton: number of states, number of symbols, number of transitions and transition density." << "\n"
                    << "\t Options are as follows: \n"
                      << "\t\t - [-countInitialState=true/false]: " << "counts the implicit initial state as well (default: true). \n"
                      << "\t\t - [-countInitialTranstrue/false]: " << "counts the initial transitions/symbols as well (default: true). \n"
                      << "\t\t - [-outputFormat=human/machine]: " << "outputs the measures in a user-friendly or machine-friendly manner (default: human). \n"
                      << "\n"
              << "\t - '" << argv[0] << " measure_non_det [ options ] input_aut' to calculate two measures of non-determinism of the automaton: "
                    << "1) ND_states - the number of states (%) from which there are non-determisitic transitions (top-down), i.e., at least 2 transitions with the same symbol, "
                    << "2) ND_trans - the number of (non-initial) transitions (%) for which there is another transition from the same state and with the same symbol. \n"
                    << "\t Options are as follows: \n"
                      << "\t\t - [-outputFormat=human/machine]: " << "outputs the measures in a user-friendly or machine-friendly manner (default: human). \n"
                      << "\n"
              << "\t - '" << argv[0] << " measure_trans_overlap [ options ] input_aut' to calculate three measures of overlap in the transitions of the automaton (advanced): "
                    << "1) TOL_1 - the number of transitions (%) for which at least one of their children states is also part of another transition with the same parent and symbol and at that same position, "
                    << "2) TOL_2 - an average of how many (%) children states of a transition that overlaps are also children states of a different transition with the same parent and symbol (maintaining their positions), "
                    << "3) TOL_3 - an average of, for each transition that overlaps, how many times each of its children states appears at that same position in an additional transition with the same parent and symbol. \n"
                    << "\t Note that on large automata these measures may take a long time to compute. Options are as follows: \n"
                      << "\t\t - [-outputFormat=human/machine]: " << "outputs the measures in a user-friendly or machine-friendly manner (default: human). \n"
              << "\n" << "\n" ;
}

bool isOpt(string arg)
{
    bool result = arg.find('-') == 0;

    return result;
}

string getOptName(string arg)
{
    string optName = arg.substr(1, arg.find_first_of('=') - 1);
    return optName;
}

string getOptValue(string arg)
{
    string optValue = arg.substr(arg.find_first_of('=')+1);
    return optValue;
}

void set_HGA_strategy(string opt_value)
{
    if (opt_value == "none")
        setOPT(TYPE_OF_HISTORY_OF_GOOD_ATKS, NONE);
    else if (opt_value == "local")
        setOPT(TYPE_OF_HISTORY_OF_GOOD_ATKS, LOCAL);
    else if (opt_value == "semi-global")
        setOPT(TYPE_OF_HISTORY_OF_GOOD_ATKS, SEMI_GLOBAL);
    else if (opt_value == "global")
        setOPT(TYPE_OF_HISTORY_OF_GOOD_ATKS, GLOBAL);
    else
        exit_with_error("Error: wrong value provided for option 'HGA_strategy': '" + opt_value + "'. \n");
}

void set_HBA_strategy(string opt_value)
{
    if (opt_value == "none")
        setOPT(TYPE_OF_HISTORY_OF_BAD_ATKS, NONE);
    else if (opt_value == "local")
        setOPT(TYPE_OF_HISTORY_OF_BAD_ATKS, LOCAL);
    else if (opt_value == "semi-global")
        setOPT(TYPE_OF_HISTORY_OF_BAD_ATKS, SEMI_GLOBAL);
    else if (opt_value == "global")
        setOPT(TYPE_OF_HISTORY_OF_BAD_ATKS, GLOBAL);
    else
        exit_with_error("Error: wrong value provided for option 'HBA_strategy': '" + opt_value + "'. \n");
}

bool getApplyHeavyInTheEndVal(string opt_value)
{
    if (opt_value == "true") return true;
    if (opt_value == "false") return false;
    exit_with_error("Error: wrong value provided for option 'applyHeavyInTheEnd': '" + opt_value + "'. \n");

    return false;
}

bool getApplyHeavyAndSatInTheEndVal(string opt_value)
{
    if (opt_value == "true") return true;
    if (opt_value == "false") return false;
    exit_with_error("Error: wrong value provided for option 'applyHeavyAndSatTheEnd': '" + opt_value + "'. \n");

    return false;
}

void setBranchPRdepth(string str)
{
    pr_depth = boost::lexical_cast<unsigned int>(str);
}

void appendNonDefaultOpts(string& log)
{
    if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] != NONE)
    {
        log += "_using_hga=";
        if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == SEMI_GLOBAL)
            log += "semi-global";
        if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == LOCAL)
            log += "local";
        if (OPT[TYPE_OF_HISTORY_OF_GOOD_ATKS] == GLOBAL)
            log += "global";
    }
    if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] != NONE)
    {
        log += "_and_hba=";
        if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == SEMI_GLOBAL)
            log += "semi-global";
        if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == LOCAL)
            log += "local";
        if (OPT[TYPE_OF_HISTORY_OF_BAD_ATKS] == GLOBAL)
            log += "global";
    }
    if (pr_depth != AUTO_PR)
        log += "_and_bpr=" + std::to_string(pr_depth);
}

int parseProcedure(int argc, char* argv[], unsigned int pos)
{

    if (argc < 3)
    {
        printHelp(argv);
        return 0;
    }

    unsigned int max_numb_tests = 0;

    /* Parse opt args. */
    while (isOpt(argv[++pos]))
    {
        if (getOptName(argv[pos]) == "timeout")
            TIMEOUT = boost::lexical_cast<long int>(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "max_numb_tests")
            max_numb_tests = boost::lexical_cast<long int>(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "HGA_strategy")
            set_HGA_strategy(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "HBA_strategy")
            set_HBA_strategy(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "BranchPR")
            setBranchPRdepth(getOptValue(argv[pos]));
        else
            exit_with_error("Error: Wrong option name given '" + getOptName(argv[pos]) + "'. \n");
    }
    /* At the end of the loop pos is left pointing at the first non-opt argument. */

    vector<string> input_dirs = mapSanitizeDirStrs(convertCharPtArrayToStrVector(argv, pos, argc-1));

    string log_avg = "log_of_avg_values_of_Procedure";

    appendNonDefaultOpts(log_avg);

    log_avg += "_date:" + localTime() + ".txt";

    procedure_tester(input_dirs, max_numb_tests, log_avg);

    return 1;

}

bool getTAInputFormatDir(string val)
{
    if (val == "dir")
        return true;
    if (val == "file")
        return false;
    exit_with_error("Wrong value given to the automata input format.\n");

    return false;
}

int getOutputStatType(string val)
{
    if (val == "human")
        return STD_OUT_HUMAN;
    if (val == "machine")
        return STD_OUT_MACHINE;
    exit_with_error("Wrong value given to the statistical output format.\n");

    return STD_OUT_HUMAN;
}

int parseMinimize(int argc, char* argv[], unsigned int pos)
{

    if (argc < 5)
    {
        printHelp(argv);
        return 0;
    }

    unsigned int max_numb_tests = 0;
    string output_dir = "";
    bool input_format_dir = false;
    int outputStat_type = STD_OUT_HUMAN;
    filename output_filename = "";

    /* Parse opt args. */
    while (isOpt(argv[++pos]))
    {
        if (getOptName(argv[pos]) == "timeout")
            TIMEOUT = boost::lexical_cast<long int>(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "max_numb_tests")
            max_numb_tests = boost::lexical_cast<long int>(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "output")
            output_dir = sanitizeDirStr(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "HGA_strategy")
            set_HGA_strategy(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "HBA_strategy")
            set_HBA_strategy(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "BranchPR")
            setBranchPRdepth(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "input_format")
            input_format_dir = getTAInputFormatDir(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "output_stat_format")
            outputStat_type = getOutputStatType(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "o")
            output_filename = getOptValue(argv[pos]);
        else
            exit_with_error("Error: Wrong option name given '" + getOptName(argv[pos]) + "'. \n");
    }
    /* At the end of the loop pos is left pointing at the first non-opt argument. */

    unsigned int la_dw = boost::lexical_cast<unsigned int>(argv[pos++]);
    unsigned int la_up = boost::lexical_cast<unsigned int>(argv[pos++]);

    string log_avg = "log_of_avg_values_of_heavy(" + std::to_string(la_dw) + "," + std::to_string(la_up) + ")";
    appendNonDefaultOpts(log_avg);
    log_avg += "_date:" + localTime() + ".txt";

    if (input_format_dir)
    {
        vector<string> input_dirs = mapSanitizeDirStrs(convertCharPtArrayToStrVector(argv, pos, argc-1));
        heavy_tester_dir(la_dw, la_up, input_dirs, output_dir, max_numb_tests, outputStat_type, log_avg);
    }
    else
    {
        vector<filename> input_files = convertCharPtArrayToStrVector(argv, pos, argc-1);
        heavy_tester_file(la_dw, la_up, input_files, output_dir, output_filename, max_numb_tests, outputStat_type);
    }

    return 1;

}

int parseComplement(int argc, char* argv[], unsigned int pos)
{

    if (argc < 3)
    {
        printHelp(argv);
        return 0;
    }

    string output_dir = "";
    int outputStat_type = STD_OUT_HUMAN;
    string output_filename = "";

    /* Parse opt args. */
    while (isOpt(argv[++pos]))
    {
        if (getOptName(argv[pos]) == "output")
            output_dir = sanitizeDirStr(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "output_stat_format")
            outputStat_type = getOutputStatType(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "o")
            output_filename = getOptValue(argv[pos]);
        else
            exit_with_error("Error: Wrong option name given '" + getOptName(argv[pos]) + "'. \n");
    }
    /* At the end of the loop pos is left pointing at the first non-opt argument. */

    vector<filename> input_files = convertCharPtArrayToStrVector(argv, pos, argc-1);

    complement_tester_file(input_files, output_dir, output_filename, outputStat_type);

    return 1;

}

int parseMinimizeWithSaturation(int argc, char* argv[], unsigned int pos)
{

    if (argc < 7)
    {
        printHelp(argv);
        return 0;
    }

    unsigned int max_numb_tests = 0;
    string output_dir = "";
    bool input_format_dir = false;
    int outputStat_type = STD_OUT_HUMAN;
    unsigned int max_attempts_sat = 1;
    unsigned int sat_version = 2;

    /* Parse opt args. */
    while (isOpt(argv[++pos]))
    {
        if (getOptName(argv[pos]) == "timeout")
            TIMEOUT = boost::lexical_cast<long int>(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "max_numb_tests")
            max_numb_tests = boost::lexical_cast<long int>(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "output")
            output_dir = sanitizeDirStr(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "output_stat_format")
            outputStat_type = getOutputStatType(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "HGA_strategy")
            set_HGA_strategy(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "HBA_strategy")
            set_HBA_strategy(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "BranchPR")
            setBranchPRdepth(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "input_format")
            input_format_dir = getTAInputFormatDir(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "max_attempts_sat")
            max_attempts_sat = boost::lexical_cast<unsigned int>(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "sat_version")
            sat_version = boost::lexical_cast<unsigned int>(getOptValue(argv[pos]));
        else
            exit_with_error("Error: Wrong option name given '" + getOptName(argv[pos]) + "'. \n");
    }
    /* At the end of the loop pos is left pointing at the first non-opt argument. */

    unsigned int la_dw = boost::lexical_cast<unsigned int>(argv[pos++]);
    unsigned int la_up = boost::lexical_cast<unsigned int>(argv[pos++]);
    unsigned int la_dw_sat = boost::lexical_cast<unsigned int>(argv[pos++]);
    unsigned int la_up_sat = boost::lexical_cast<unsigned int>(argv[pos++]);

    string log_avg = "log_of_avg_values_of_heavy(" + std::to_string(la_dw) + "," + std::to_string(la_up)
            + ")_with_sat" + std::to_string(sat_version) + "("
            + std::to_string(la_dw_sat) + "," + std::to_string(la_up_sat) + ")";

    appendNonDefaultOpts(log_avg);

    log_avg += "_date:" + localTime() + ".txt";

    if (input_format_dir)
    {
        vector<string> input_dirs = mapSanitizeDirStrs(convertCharPtArrayToStrVector(argv, pos, argc-1));
        heavy_with_sat_tester_dir(la_dw, la_up, la_dw_sat, la_up_sat, sat_version, max_attempts_sat, input_dirs, output_dir, max_numb_tests, outputStat_type, log_avg);
    }
    else
    {
        vector<filename> input_files = convertCharPtArrayToStrVector(argv, pos, argc-1);
        heavy_with_sat_tester_file(la_dw, la_up, la_dw_sat, la_up_sat, sat_version, max_attempts_sat, input_files, output_dir, max_numb_tests, outputStat_type);
    }

    return 1;

}

int parseMinimizeAndComplement(int argc, char* argv[], unsigned int pos)
{

    if (argc < 5)
    {
        printHelp(argv);
        return 0;
    }

    unsigned int max_numb_tests = 0;
    string output_dir = "";
    bool input_format_dir = false;
    bool apply_heavy_in_the_end = false, apply_heavy_and_sat_in_the_end = false;

    /* Parse opt args. */
    while (isOpt(argv[++pos]))
    {
        if (getOptName(argv[pos]) == "timeout")
            TIMEOUT = boost::lexical_cast<long int>(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "max_numb_tests")
            max_numb_tests = boost::lexical_cast<long int>(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "output")
            output_dir = sanitizeDirStr(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "HGA_strategy")
            set_HGA_strategy(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "HBA_strategy")
            set_HBA_strategy(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "BranchPR")
            setBranchPRdepth(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "input_format")
            input_format_dir = getTAInputFormatDir(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "applyHeavyInTheEnd")
            apply_heavy_in_the_end = getApplyHeavyInTheEndVal(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "applyHeavyAndSatInTheEnd")
            apply_heavy_and_sat_in_the_end = getApplyHeavyAndSatInTheEndVal(getOptValue(argv[pos]));
        else
            exit_with_error("Error: Wrong option name given '" + getOptName(argv[pos]) + "'. \n");
    }
    /* At the end of the loop pos is left pointing at the first non-opt argument. */

    unsigned int la_dw = boost::lexical_cast<unsigned int>(argv[pos++]);
    unsigned int la_up = boost::lexical_cast<unsigned int>(argv[pos++]);

    if (input_format_dir)
    {
        vector<string> input_dirs = mapSanitizeDirStrs(convertCharPtArrayToStrVector(argv, pos, argc-1));
        complement_after_heavy_tester_dir(la_dw, la_up, input_dirs, output_dir, max_numb_tests, apply_heavy_in_the_end, apply_heavy_and_sat_in_the_end);
    }
    else
    {
        vector<filename> input_files = convertCharPtArrayToStrVector(argv, pos, argc-1);
        string log_avgs = "log_of_avg_values_of_comp_after_heavy(" + std::to_string(la_dw) + "," +
                           std::to_string(la_up) + ")_date:" + localTime() + ".txt";
        complement_after_heavy_tester_file(la_dw, la_up, input_files, output_dir, max_numb_tests, log_avgs, apply_heavy_in_the_end, apply_heavy_and_sat_in_the_end);
    }

    return 1;

}

int parseComputeLaDwSim(int argc, char* argv[], unsigned int pos)
{

    if (argc < 4)
    {
        printHelp(argv);
        return 0;
    }

    unsigned int max_numb_tests = 0;

    /* Parse opt args. */
    while (isOpt(argv[++pos]))
    {
        if (getOptName(argv[pos]) == "timeout")
            TIMEOUT = boost::lexical_cast<long int>(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "max_numb_tests")
            max_numb_tests = boost::lexical_cast<long int>(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "HGA_strategy")
            set_HGA_strategy(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "HBA_strategy")
            set_HBA_strategy(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "BranchPR")
            setBranchPRdepth(getOptValue(argv[pos]));
        else
            exit_with_error("Error: Wrong option name given '" + getOptName(argv[pos]) + "'. \n");
    }
    /* At the end of the loop pos is left pointing at the first non-opt argument. */

    unsigned int la = boost::lexical_cast<unsigned int>(argv[pos++]);

    vector<string> input_dirs = mapSanitizeDirStrs(convertCharPtArrayToStrVector(argv, pos, argc-1));

    compute_la_dw_sim_tester(la, input_dirs, max_numb_tests);

    return 1;

}

int parseCompareMinimizeWithSaturationVersions(int argc, char* argv[], unsigned int pos)
{

    if (argc < 5)
    {
        printHelp(argv);
        return 0;
    }

    unsigned int max_numb_tests = 0;
    unsigned int la_dw_sat = 1, la_up_sat = 1, max_attempts_sat = 1;

    /* Parse opt args. */
    while (isOpt(argv[++pos]))
    {
        if (getOptName(argv[pos]) == "timeout")
            TIMEOUT = boost::lexical_cast<long int>(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "max_numb_tests")
            max_numb_tests = boost::lexical_cast<long int>(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "HGA_strategy")
            set_HGA_strategy(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "HBA_strategy")
            set_HBA_strategy(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "BranchPR")
            setBranchPRdepth(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "la-dw-sat")
            la_dw_sat = boost::lexical_cast<unsigned int>(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "la-up-sat")
            la_up_sat = boost::lexical_cast<unsigned int>(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "max_attempts_sat")
            max_attempts_sat = boost::lexical_cast<unsigned int>(getOptValue(argv[pos]));
        else
            exit_with_error("Error: Wrong option name given '" + getOptName(argv[pos]) + "'. \n");
    }
    /* At the end of the loop pos is left pointing at the first non-opt argument. */

    unsigned int la_dw = boost::lexical_cast<unsigned int>(argv[pos++]);
    unsigned int la_up = boost::lexical_cast<unsigned int>(argv[pos++]);

    vector<string> input_dirs = mapSanitizeDirStrs(convertCharPtArrayToStrVector(argv, pos, argc-1));

    string log_avg = "log_of_avg_values_of_compare_heavy(" + std::to_string(la_dw) + "," + std::to_string(la_up)
            + ")_with_heavy_combined_with_different_versions_of_sat(" + std::to_string(la_dw_sat) + "," + std::to_string(la_up_sat)
            + ")_and_max_attempts_sat=" + std::to_string(max_attempts_sat);

    appendNonDefaultOpts(log_avg);

    log_avg += "_date:" + localTime() + ".txt";

    heavy_with_sat_versionsComparer(la_dw, la_up, la_dw_sat, la_up_sat, max_attempts_sat, input_dirs, max_numb_tests, log_avg);

    return 1;

}

bool getOutputFormatVal(string val)
{
    if (val == "human")
        return true;
    if (val == "machine")
        return false;
    exit_with_error("Wrong value given to the output format value.\n");

    return false;
}

int parseMeasureSize(int argc, char* argv[], unsigned int pos)
{

    if (argc < 3)
    {
        printHelp(argv);
        return 0;
    }

    bool countInitialState = true, countInitialTransitions = true;
    bool output_human = true;

    /* Parse opt args. */
    while (isOpt(argv[++pos]))
    {
        if (getOptName(argv[pos]) == "countInitialState")
            countInitialState = convertStrToBool(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "countInitialTrans")
            countInitialTransitions = convertStrToBool(getOptValue(argv[pos]));
        else if (getOptName(argv[pos]) == "outputFormat")
            output_human = getOutputFormatVal(getOptValue(argv[pos]));
        else
            exit_with_error("Error: Wrong option name given '" + getOptName(argv[pos]) + "'. \n");
    }
    /* At the end of the loop pos is left pointing at the first non-opt argument. */

    string filename = argv[pos];

    measureSizeTA(filename, countInitialState, countInitialTransitions, output_human);

    return 1;

}

int parseMeasureNonDet(int argc, char* argv[], unsigned int pos)
{

    if (argc < 3)
    {
        printHelp(argv);
        return 0;
    }

    bool output_human = true;

    /* Parse opt args. */
    while (isOpt(argv[++pos]))
    {
        if (getOptName(argv[pos]) == "outputFormat")
            output_human = getOutputFormatVal(getOptValue(argv[pos]));
        else
            exit_with_error("Error: Wrong option name given '" + getOptName(argv[pos]) + "'. \n");
    }
    /* At the end of the loop pos is left pointing at the first non-opt argument. */

    string filename = argv[pos];

    measureNonDeterminism_ui(filename, output_human);

    return 1;

}

int parseMeasureTransOverlap(int argc, char* argv[], unsigned int pos)
{

    if (argc < 3)
    {
        printHelp(argv);
        return 0;
    }

    bool output_human = true;

    /* Parse opt args. */
    while (isOpt(argv[++pos]))
    {
        if (getOptName(argv[pos]) == "outputFormat")
            output_human = getOutputFormatVal(getOptValue(argv[pos]));
        else
            exit_with_error("Error: Wrong option name given '" + getOptName(argv[pos]) + "'. \n");
    }
    /* At the end of the loop pos is left pointing at the first non-opt argument. */

    string filename = argv[pos];

    measureTransOverlaps_ui(filename, output_human);

    return 1;

}

int debug_fun(int argc, char* argv[])
{
    vector<filename> input_files = convertCharPtArrayToStrVector(argv, 2, argc-1);

    for (string file : input_files)
    {
        // Do some test here.

        // create the parser for the Timbuk format
        std::unique_ptr<VATA::Parsing::AbstrParser> parser(
            new VATA::Parsing::TimbukParser());

        // create the dictionary for translating state names to internal state numbers
        VATA::AutBase::StateDict stateDict;

        Automaton aut;
        string autStr = VATA::Util::ReadFile(file);
        aut.LoadFromString(*parser, autStr, stateDict);

        // create the serializer for the Timbuk format
        VATA::Serialization::AbstrSerializer* serializer =
            new VATA::Serialization::TimbukSerializer();

        aut = addInitialState(aut);

        VATA::AutBase::StateDict stateDict3;
        aut = getAut(applyHeavyWithSat(wrapAutData(aut,getRanks(parseFromFile(file,stateDict3))),1,1,1,1,1,1));
       // aut = complement(aut);

        aut = removeInitialState(aut);

        // dump the automaton
        autStr = aut.DumpToString(*serializer);

        // create the dictionary for translating state names to internal state numbers
        VATA::AutBase::StateDict stateDict2;

        Automaton aut2;
        std::unique_ptr<VATA::Parsing::AbstrParser> parser2(
            new VATA::Parsing::TimbukParser());
        aut2.LoadFromString(*parser2, autStr, stateDict2);

        VATA::Serialization::AbstrSerializer* serializer2 =
            new VATA::Serialization::TimbukSerializer();
        std::cout << aut2.DumpToString(*serializer2,stateDict2);

    }

    return 1;
}

int main(int argc, char* argv[])
{

    startOPTDefaults();

    if (argc > 1 && argv[1] == procedure_cmd)
        return parseProcedure(argc, argv, 1);
    if (argc > 1 && argv[1] == minimize_cmd)
        return parseMinimize(argc, argv, 1);  
    if (argc > 1 && argv[1] == minimize_with_sat_cmd)
        return parseMinimizeWithSaturation(argc, argv, 1);
    if (argc > 1 && argv[1] == minimize_and_complement_cmd)
        return parseMinimizeAndComplement(argc, argv, 1);
    if (argc > 1 && argv[1] == complement_cmd)
        return parseComplement(argc, argv, 1);
    if (argc > 1 && argv[1] == compute_la_dw_sim_cmd)
        return parseComputeLaDwSim(argc, argv, 1);
    if (argc > 1 && argv[1] == compare_minimize_with_sat_versions_cmd)
        return parseCompareMinimizeWithSaturationVersions(argc, argv, 1);    
    if (argc > 1 && argv[1] == measure_size_cmd)
        return parseMeasureSize(argc, argv, 1);
    if (argc > 1 && argv[1] == measure_non_det_cmd)
        return parseMeasureNonDet(argc, argv, 1);
    if (argc > 1 && argv[1] == measure_trans_overlap_cmd)
        return parseMeasureTransOverlap(argc, argv, 1);
    if (argc > 2 && argv[1] == debug_cmd)
        return debug_fun(argc, argv);
    else
    {
        printHelp(argv);
        return 0;
    }

}
