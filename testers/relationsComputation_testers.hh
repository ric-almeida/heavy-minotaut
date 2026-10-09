#ifndef RELATIONSCOMPUTATION_TESTERS_HH
#define RELATIONSCOMPUTATION_TESTERS_HH

#include "common.hh"
#include "../src/dw_simulation.hh"
#include "../src/Common/statisticalResults.hh"

void compute_la_dw_sim_tester(unsigned int la, vector<string> input_dirs,
                              unsigned int numb_tests = 0, string log_avgs_ = "");


#endif // RELATIONSCOMPUTATION_TESTERS_HH
