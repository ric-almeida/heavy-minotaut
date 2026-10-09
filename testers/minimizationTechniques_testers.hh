
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

#ifndef minimizationTechniques_testers_H
#define minimizationTechniques_testers_H


#include "../src/minimizationTechniques.hh"

#include "common.hh"
#include "../src/dw_simulation.hh"
#include "../src/up_simulation.hh"
#include "../src/quotienting.hh"
#include "../src/pruning.hh"
#include "../src/saturation.hh"
#include "../src/Common/statisticalResults.hh"

void minimizationTechniques_testers();
void simple_Heavy_tester(unsigned int la_dw, unsigned int la_up, string input_dir, string output_dir);
void call_heavy_tester(unsigned int la_dw, unsigned int la_up, string dir, string log="");
void procedure_tester(vector<string> input_dirs, unsigned int numb_tests, string log_avgs_ = "");

void heavy_tester_file(unsigned int la_dw, unsigned int la_up,
                       vector<filename> input_files,
                       string output_dir = "", filename output_filename = "",
                       unsigned int max_numb_tests = 0, int outputStat_type = STD_OUT_HUMAN);

void heavy_tester_dir(unsigned int la_dw, unsigned int la_up, string input_dir,
                      string output_dir = "", unsigned int numb_tests = 0,
                      int outputStat_type = NO_OUT, string log_avgs = "");

void heavy_tester_dir(unsigned int la_dw, unsigned int la_up, vector<string> input_dirs,
                      string output_dir = "", unsigned int numb_tests = 0,
                      int outputStat_type = NO_OUT, string log_avgs_ = "");

void heavy_with_sat_tester_file(unsigned int la_dw, unsigned int la_up,
                                unsigned int la_dw_sat, unsigned int la_up_sat, unsigned int sat_version,
                                int max_attempts_sat, vector<filename> input_files, string output_dir = "",
                                unsigned int max_numb_tests = 0, int outputStat_type = NO_OUT);

void heavy_with_sat_tester_dir(unsigned int la_dw, unsigned int la_up,
                               unsigned int la_dw_sat, unsigned int la_up_sat,
                               unsigned int sat_version, unsigned int max_attempts_sat,
                               vector<string> input_dirs, string output_dir = "",
                               unsigned int numb_tests = 0, int outputStat_type = NO_OUT, string log_avgs_ = "");

void heavy_with_sat_versionsComparer(unsigned int la_dw, unsigned int la_up,
                                     unsigned int la_dw_sat, unsigned int la_up_sat, unsigned int max_attempts_sat,
                                     const vector<string> &input_dirs, unsigned int numb_tests = 0, string log_avgs_ = "");

#endif // minimizationTechniques_testers_H
