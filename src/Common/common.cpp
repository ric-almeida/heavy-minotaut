
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Implementation file for a collection of definitions and functions which     *
 *  are of general utility, with no particular scope.                               *
 * 																					*
 ************************************************************************************/

#include "common.hh"

TimePoint Epoch(std::chrono::seconds(0));      // 0 seconds since the Epoch


void exit_with_error(const char* msg)
{
	cerr << msg << endl;
	exit(EXIT_FAILURE);
}

void exit_with_error(const string msg)
{
    const char* c = msg.c_str();
    exit_with_error(c);
}

/* Functions that try to return the element at a certain position of a given vector
 * and throw an exception if a out_of_range error occurs. */

unsigned int vectorUIntsAt(const vector<unsigned int>& vec, const unsigned int pos, const string strct_name) {
	unsigned int result;
	
	try {
		result = vec.at(pos);
	}
	catch (const std::out_of_range& oor) {
		std::cerr << "Out of Range error: " << oor.what();
		std::cerr << " when acessing the position " << std::to_string(pos) <<
		" from " << strct_name << " which has size " << std::to_string(vec.size());
		exit_with_error("");
	}
	
	return result;
}

Time time_now()
{
    return Clock::now();
}

Time startTimer()
{
    return time_now();
}

bool timerNotStarted(Time timer)
{
    return timer == Epoch;
}

seconds elapsedSec(Time start)
{
    return (std::chrono::duration_cast<std::chrono::microseconds>(time_now()-start).count() / (float) 1000000);
}

int genRandomNumb(int min, int max)
{
    srand (time(NULL));
    int number = rand() % (max-min+1) + min;
    return number;
}

/* Generates an "unique" (random) id between 1 and 1000 */
unsigned int unique_id()
{
    return genRandomNumb(1, 1000);
}

unsigned int min(unsigned int a, unsigned int b)
{
    return (a<b ? a : b);
}

vector<string> convertCharPtArrayToStrVector(char* array[], int start, int end)
{
    vector<string> vec;

    for (int i=start; i<=end; i++)
        vec.push_back(array[i]);

    return vec;
}


/* String and IO functions */

string vectorIntsToString(const vector<long unsigned int> & vec)
{
    string str;
    for (unsigned int j=0; j<vec.size(); j++)
        str += std::to_string(vec.at(j)) + ",";
    return str;
}

void printVectorInts(const vector<long unsigned int> & vec)
{
    std::cout << vectorIntsToString(vec) << "\n";
}

void printVectorArrayInts(const vector<unsigned int[2]>& vec)
{
	for (unsigned int j=0; j<vec.size(); j++){
        for (unsigned int i=0; i<5; i++)
			std::cout << (vec.at(j))[i] << ",";
		std::cout << "\n";
	}
}

string vectorVectorIntsToString(const vector<vector<long unsigned int> >& vec)
{
	string str;
	
	for (unsigned int i=0; i<vec.size(); i++){
        vector<long unsigned int> subvec = vec.at(i);
		for (unsigned int j=0; j<subvec.size(); j++)
			str += std::to_string(subvec.at(j)) + ",";
		str += "\n";
	}
	
	return str;
}

void printVectorVectorInts(const vector<vector<long unsigned int> >& vec)
{
	std::cout << vectorVectorIntsToString(vec) << "\n";
}

vector<vector<bool> > createBoolMatrix(unsigned int numb_rows, unsigned int numb_columns, bool cell_value)
{
    vector<bool> column(numb_rows,cell_value);
    vector<vector<bool> > matrix(numb_columns,column);

    return matrix;
}

vector<unsigned int> removeDuplicates(vector<unsigned int> vec)
{
    sort( vec.begin(), vec.end() );
    vec.erase( unique( vec.begin(), vec.end() ), vec.end() );

    return vec;
}

string localTime()
{

    char s[1000];

    time_t t = time(NULL);
    struct tm * p = localtime(&t);

    strftime(s, 1000, "%a_(%Y-%b-%d-%T)", p);

    return s;
}

string localTime2()
{

    char s[1000];

    time_t t = time(NULL);
    struct tm * p = localtime(&t);

    strftime(s, 1000, "%Y_%b_%d_%T", p);

    return s;
}

bool convertStrToBool(string str)
{
    if (str == "true" || str == "True" || str == "TRUE")
        return true;
    if (str == "false" || str == "False" || str == "FALSE")
        return false;

    exit_with_error("Wrong parameter given to the function convertStrToBool.");

    return false;

}

void writeToFile(string filename, string text, bool overwrite)
{

    std::ofstream out;
    // std::ios::app is the open mode "append" meaning
    // new data will be written to the end of the file.
    out.open(filename, overwrite ? std::ios::trunc : std::ios::app);
    out << text;

}

void outputText(string text, string filename)
{

    if (filename == "")
    {
        std::cout << text << std::flush;
    }
    else
        writeToFile(filename, text);

}

string appendStrings(string s1, string s2, string s3, string s4)
{
    std::stringstream ss;
    ss << s1 << s2 << s3 << s4;
    return ss.str();
}

/* Creates the full directory specified if it doesn't exist already. */
void createDir(string dir)
{
    const char* path_char = dir.c_str();
    boost::filesystem::path path(path_char);
    boost::system::error_code returnedError;
    boost::filesystem::create_directories(path, returnedError);

    if (returnedError)
        exit_with_error("Error creating directory " + dir);
}

/* Deletes the file specified from the filesystem. */
void deleteFile(string filename)
{
    boost::system::error_code ec;
    bool exists = boost::filesystem::remove(filename,ec);

    if (ec)
        outputText("Error when attempting to delete file " + filename + ".\n");
    if (!exists) outputText("Warning: attempted to remove a file which did not exist - "
                            + filename + ".\n");
}
