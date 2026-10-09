
#ifndef _MAIN_HH_
#define _MAIN_HH_


#include <boost/lexical_cast.hpp>

#include <iterator>
using namespace std;

#include "../src/Common/common.hh"
#include "../src/step.hh"
#include "../src/Common/debugging.hh"
#include "../src/Common/automatonHelper.hh"
#include "../src/dw_simulation.hh"
#include "../src/up_simulation.hh"
#include "../src/quotienting.hh"
#include "../src/pruning.hh"

#include "../testers/minimizationTechniques_testers.hh"
#include "../testers/minimizationAndComplement_testers.hh"
#include "../testers/relationsComputation_testers.hh"
#include "../testers/complement_tester.hh"

int pr_depth;

string procedure_cmd = "procedure";
string minimize_cmd = "minimize";
string minimize_with_sat_cmd = "minimize_with_saturation";
string minimize_and_complement_cmd = "minimize_and_complement";
string complement_cmd = "complement";
string compute_la_dw_sim_cmd = "compute_la-dw-sim";
string compare_minimize_with_sat_versions_cmd = "compare_minimize_with_saturation_versions";
string measure_size_cmd = "measure_size";
string measure_non_det_cmd = "measure_non_det";
string measure_trans_overlap_cmd = "measure_trans_overlap";
string debug_cmd = "debug";


#endif
