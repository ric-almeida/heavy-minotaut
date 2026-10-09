
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Header file declaring automata minimization procedures. It includes         *
 *  the Heavy algorithm (see README.txt/Publications).                              *
 * 																					*
 ************************************************************************************/

#ifndef UP_SIMULATION_TESTER_H
#define UP_SIMULATION_TESTER_H


#include "../src/dw_simulation.hh"
#include "../src/up_simulation.hh"
#include "../src/combined_relation.hh"
#include "../src/quotienting.hh"
#include "../src/pruning.hh"
#include "../src/saturation.hh"
#include "../src/Common/statisticalResults.hh"

/* Encodings */
extern const unsigned int DW_LA_1ST_TIME         ;
extern const unsigned int UP_LA_WITH_ID_1ST_TIME ;
extern const unsigned int STRICT_UP_1_WITH_ID    ;
extern const unsigned int DW_LA_2ND_TIME         ;
extern const unsigned int UP_LA_WITH_ID_2ND_TIME ;
extern const unsigned int DW_1                   ;
extern const unsigned int UP_LA_WITH_DW_1        ;
extern const unsigned int numbRelations          ;

extern const unsigned int QUOT_WITH_DW_LA             ;
extern const unsigned int QUOT_WITH_UP_LA_WITH_ID_1ST ;
extern const unsigned int QUOT_WITH_UP_LA_WITH_ID_2ND ;
extern const unsigned int numbQuotientings            ;

extern const unsigned int PRUN_WITH_P_ID_AND_DW_LA            ;
extern const unsigned int PRUN_WITH_P_UP_1_WITH_ID_AND_DW_LA  ;
extern const unsigned int PRUN_WITH_P_UP_LA_WITH_DW_1_AND_DW_1;
extern const unsigned int numbPrunings                        ;


AutData removeDeadStates(const AutData &autData);

AutData heavy(const AutData &autData, unsigned int la_dw,
                     unsigned int la_up, Time& timeout_start = Epoch,
                     TestData& testData = emptyTestData,
                     string log_humanread_filename = "");
AutData heavyGradual(const AutData &autData_i, unsigned int la_dw, unsigned int la_dw_max,
                       unsigned int greatest_symbol,
                       TestData& testData = emptyTestData, Time& timeout_start = Epoch, string log_humanread_filename = "");
void applyProcedure(const AutData& autData_i,   MetaData& metaData,
                    TestData& testData_ru,
                    TestData& testData_quot,    Timeout& timeout_quot,
                    TestData& testData_qAndP,   Timeout& timeout_qAndP,
                    TestData& testData_heavy11, Timeout& timeout_heavy11,
                    TestData& testData_heavy24, Timeout& timeout_heavy24,
                    string log, bool ignore_i, bool ignore_red);
AutData applyHeavy(const AutData &autData_i, MetaData& metaData,
                unsigned int la_dw, unsigned int la_up,
                TestData& testData_heavy, Timeout& timeout_heavy, int outputStat_type = NO_OUT, string log = "");
AutData applyHeavyWithSat(const AutData& autData_i,
                          unsigned int la_dw, unsigned int la_up,
                          unsigned int la_dw_sat, unsigned int la_up_sat, unsigned int sat_version, unsigned int max_sat_attempts,
                          MetaData& metaData = emptyMetaData, TestData& testData_heavy = emptyTestData, Timeout& timeout_heavy = emptyTimeout,
                          int outputStat_type = NO_OUT, string log = "");

void applyQuotCombinedPreorder(const AutData& autData, MetaData& metaData,
                               unsigned int la_dw, unsigned int la_up,
                               TestData& testData, Timeout& timeout,
                               string log_humanread_filename,
                               string log_machread_filename,
                               string log_machread_times_filename);

AutData applyMinimizationSequence(const AutData& autData_i,
                                  unsigned int la_dw, unsigned int la_up,
                                  unsigned int greatest_state  = 0,
                                  unsigned int greatest_symbol = 0,
                                  Time& timeout_start = Epoch,
                                  TestData& testData = emptyTestData,
                                  string log_humanread_filename = "");

void compareHeavyWithSatVersions(const AutData& autData_i, MetaData& metaData,
                                 unsigned int la_dw, unsigned int la_up,
                                 unsigned int la_dw_sat, unsigned int la_up_sat, unsigned int max_sat_attempts,
                                 TestData& testData_h, Timeout& timeout_h,
                                 TestData& testData_v1, Timeout& timeout_v1,
                                 TestData& testData_v1withLoop, Timeout& timeout_v1withLoop,
                                 TestData& testData_v3, Timeout& timeout_v3,
                                 TestData& testData_v2withLoop, Timeout& timeout_v2withLoop, TestData &testData_v2withLoop2, Timeout &timeout_v2withLoop2,
                                 string log);


#endif // UP_SIMULATION_TESTER_H
