
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Header file for a collection of definitions and functions which are of      *
 *  general utility, with no particular scope. The is the 'bottom' header file in   *
 *  this project, ie, any other header file is included, directly or indirectly,    *
 *  by this one.                                                                    *
 * 																					*
 ************************************************************************************/

#ifndef COMMON_HH
#define COMMON_HH


#include <utility>      // std::pair
#include <stddef.h>		/* Allows to use ptr = NULL, for eg. */
#include <string>       /* std::string, std::to_string */
#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>    /* std::out_of_range */
#include <stddef.h>		/* Allows to use ptr = NULL, for eg. */
#include <stdexcept>    /* std::out_of_range */
#include <chrono>
#include <stdlib.h>
#include <stdint.h>

#include <vata/parsing/timbuk_parser.hh>

#include <vata/explicit_tree_aut.hh>	/* The internal automata representation from libvata */

#include <boost/format.hpp>
#include <boost/filesystem.hpp>

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;
using chr_time2 = std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::duration<long int, std::ratio<1l, 1000000000l> > >;
using chr_time = std::chrono::time_point<Clock/*,std::chrono::duration<long int, std::ratio<1l, 1000000000l> >*/>;
using seconds = /*unsigned long int;*/ float;
using mcseconds = float;
/* The chr_time is not currently being used. We prefer seconds since it's easier to
 * type optional parameters in functions (by just setting the default value to 0). */

using namespace std;
using string = std::string;
using std::pair;
using std::vector;
using std::tuple;
using std::set;
using std::unordered_set;
using filename = string;

extern seconds TIMEOUT;

const char kPathSeparator =
#ifdef _WIN32
                            '\\';
#else
                            '/';
#endif

extern TimePoint Epoch;      // 0 seconds since the Epoch
using Time = TimePoint;

// TODO: tidy up the time,second,chr_time declarations

void exit_with_error(const char*);
void exit_with_error(const string);
void printVectorArrayInts(const vector<unsigned int[]> &);
unsigned int vectorUIntsAt(const vector<unsigned int>&, const unsigned int, const string);
void printVectorVectorInts(const vector<vector<unsigned long> > &);
void printVectorInts(const vector<long unsigned int> &);
string vectorVectorIntsToString(const vector<vector<int> >&);
vector<vector<bool> > createBoolMatrix(unsigned int numb_rows, unsigned int numb_columns, bool cell_value);
void writeToFile(string filename, string text, bool overwrite = false);
void outputText(string text, string filename = "");
string localTime();
string localTime2();
Time startTimer();
bool timerNotStarted(Time timer);
seconds elapsedSec(Time start);
int genRandomNumb(int min, int max);
unsigned int unique_id();
unsigned int min(unsigned int a, unsigned int b);
vector<unsigned int> removeDuplicates(vector<unsigned int> vec);
vector<string> convertCharPtArrayToStrVector(char *array[], int start, int end);
string appendStrings(string s1, string s2, string s3 = "", string s4 = "");
bool convertStrToBool(string str);
void createDir(string dir);
void deleteFile(string filename);


#endif // COMMON_HH
