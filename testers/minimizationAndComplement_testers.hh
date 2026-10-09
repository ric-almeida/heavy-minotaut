#ifndef MINIMIZATIONANDCOMPLEMENT_TESTERS_CPP
#define MINIMIZATIONANDCOMPLEMENT_TESTERS_CPP


#include "common.hh"
#include "../src/minimizationTechniques.hh"
#include "../src/Common/statisticalResults.hh"

tuple<AutData, stateDict> complement_after_heavy(const AutData& autData_i, stateDict &sDict, unsigned int la_dw, unsigned int la_up,
                               TestData& testData_h, TestData& testData_hAndc, TestData& testData_hAndcAndh, TestData &testData_hAndcAndhAnds,
                               string log, bool apply_heavy_in_the_end, bool apply_heavy_and_sat_in_the_end);
void complement_after_heavy_tester_file(unsigned int la_dw, unsigned int la_up,
                                   vector<string> input_files, string output_dir, unsigned int numb_tests = 0, string log_avgs = "", bool apply_heavy_in_the_end = false, bool apply_heavy_and_sat_in_the_end = false);
void complement_after_heavy_tester_dir(unsigned int la_dw, unsigned int la_up,
                                   string input_dir, string output_dir = "", unsigned int numb_tests = 0, string log_avgs = "", bool apply_heavy_in_the_end = false, bool apply_heavy_and_sat_in_the_end = false);
void complement_after_heavy_tester_dir(unsigned int la_dw, unsigned int la_up,
                                   vector<string> input_dir, string output_dir, unsigned int numb_tests = 0, bool apply_heavy_in_the_end = false, bool apply_heavy_and_sat_in_the_end = false);

void printSummary_complement_after_heavy_tester(unsigned int la_dw, unsigned int la_up,
                                                       MetaData& metadata, TestData& testData,
                                                       Timeout& timeout);


#endif // MINIMIZATIONANDCOMPLEMENT_TESTERS_CPP
