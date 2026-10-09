
#include "complement_tester.hh"

void complement_tester_file(vector<filename> input_files, string output_dir, string output_filename, int outputStat_type)
{
    for (string file : input_files)
    {
        stateDict sDict;
        const AutData autData_i = parseFromFile(file, sDict);

        float sec;
        tuple<AutData,stateDict> tup_c = complement(autData_i, sec);
        AutData autData_c = std::get<0>(tup_c);
        sDict = std::get<1>(tup_c);

        /* Output resulting automaton (optional). */
        if (!output_dir.empty())
        {
            string aut_c_filename;
            if (!output_filename.empty())
                aut_c_filename = appendTimbukFilename(sanitizeDirStr(output_dir) + output_filename,
                                    "");
            else
                aut_c_filename = appendTimbukFilename(sanitizeDirStr(output_dir) + getLocalFilename(file),
                                    "_complemented");
            saveAutToFile(autData_c, sDict, aut_c_filename);
        }

        if (outputStat_type == STD_OUT_HUMAN)
        {
            outputText("\tAutomaton "  + getLocalFilename(file) + ":\t");
            outputText("Q_i: "         + std::to_string(getNumbUsedStates(autData_i)) + "\t");
            outputText("Trans_i: "     + std::to_string(getNumbTransitions(autData_i)) + "\t");
            outputText("TransDens_i: " + convert2DecStr(getTransitionDensity(autData_i)) + "\t");
            outputText("Q_c: "     + std::to_string(getNumbUsedStates(autData_c)) + "\t");
            outputText("Trans_c: " + std::to_string(getNumbTransitions(autData_c)) + "\t");
            outputText("TD_c: "    + convert2DecStr(getTransitionDensity(autData_c)) + "\t");
            outputText("Time_c: "  + convert2DecStr(sec) + "s\t");
            outputText("\n");
        }
        else
        {
            outputText(std::to_string(getNumbUsedStates(autData_i)) + "\t");
            outputText(std::to_string(getNumbTransitions(autData_i)) + "\t");
            outputText(convert2DecStr(getTransitionDensity(autData_i)) + "\t");
            outputText(std::to_string(getNumbUsedStates(autData_c)) + "\t");
            outputText(std::to_string(getNumbTransitions(autData_c)) + "\t");
            outputText(convert2DecStr(getTransitionDensity(autData_c)) + "\t");
            outputText(convert2DecStr(sec) + "\t");
        }
    }
}
