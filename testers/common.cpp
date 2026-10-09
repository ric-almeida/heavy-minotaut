
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Implementation file defining auxiliary functions for the execution of       *
 * tests. It includes functions that list files and folders in the                  *
 * filesystem.                                                                      *
 * 																					*
 ************************************************************************************/

#include "common.hh"

bool isADir(const char* dir)
{
    return (opendir(dir)) != NULL;
}

/* Experimental */
bool isADir(const string dir)
{
    const char* dir_ch = dir.c_str();

    return (opendir(dir_ch)) != NULL;
}

string sanitizeDirStr(string dir_str)
{
    std::size_t found = dir_str.find_last_of(kPathSeparator);
    if (found == dir_str.length()-1)
        return dir_str;
    else
        return (dir_str + kPathSeparator);

}

vector<string> mapSanitizeDirStrs(vector<string> dirs)
{
    for (unsigned int i=0; i<dirs.size(); i++)
        dirs[i] = sanitizeDirStr(dirs[i]);

    return dirs;
}

/* Gets the names of all files at the root of the directory given. */
vector<filename> getAllFilenames(const string directory_str)
{
    vector<filename> all_filenames;

    const char* directory = directory_str.c_str();

    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir (directory)) != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            std::string buf(directory);
            buf.append(ent->d_name);

            std::string dir(directory);

            if (buf != dir+"."
                    && buf != dir+".."                  // Exclude the parent directory from the list.
                    && buf.at(buf.size()-1) != '~'      // Exclude temp files ~.
                    && buf != dir+"README.txt"
                    && !isADir(buf)
                    && VATA::Util::ReadFile(buf)!="")   // Exclude files with wrong syntax.
            {
                all_filenames.push_back(buf);
            }
        }
        closedir (dir);
    }
    else
    {
        /* If it could not open directory */
        perror ("error opening dir.");
    }

    return all_filenames;
}

vector<filename> getAllFilenames(const vector<string> &dirs)
{
    vector<filename> all_filenames;

    for (const string directory : dirs)
    {
        vector<filename> aux = getAllFilenames(directory);
        all_filenames.insert(all_filenames.end(), aux.begin(), aux.end());
    }

    return all_filenames;
}

vector<string> getAllSubfolders(const string folder_str)
{

    DIR *dir;
    struct dirent *ent;
    char* name;
    vector<string> subfolders;
    const char* folder = folder_str.c_str();

    if ((dir = opendir (folder)) != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            name = ent->d_name;

            if (name[0] == '.')     /* This ensures "." and ".." are not */
                continue;           /* added to the subfolders vector.   */

            if (ent->d_type == DT_DIR)
            {
                string name_str(name);
                subfolders.push_back(name_str);
            }
        }
    }

    return subfolders;
}

/* Gets the filenames of all files at the root of the directory given and
 * in every subdirectory. */
vector<string> getAllSubFilenames(string directory_str)
{

    directory_str = sanitizeDirStr(directory_str);
    vector<string> all_dirs = {directory_str};

    /* Get all immediate subfolders */
    vector<string> all_subdfolders = getAllSubfolders(directory_str);
    for (const string subfolder : all_subdfolders)
    {
        string subdir = sanitizeDirStr(directory_str + subfolder);
        all_dirs.push_back(subdir);
    }

    return getAllFilenames(all_dirs);

}

string appendTimbukFilename(string filename, string sufix)
{
    return filename.insert((filename.find(".timbuk")==string::npos) ? filename.size() : filename.find(".timbuk"), sufix);
}

string getLocalFilename(const string filename)
{
    std::size_t sep_pos = filename.find_last_of(kPathSeparator);

    if (sep_pos == string::npos)
    {
        /* It's already a local name. */
        return filename;
    }

    if (sep_pos == filename.length()-1)
    {
        /* It's a folder. */
        std::size_t sep_pos_ = filename.find_last_of(kPathSeparator, sep_pos-1);
        return filename.substr(sep_pos_+1, sep_pos-sep_pos_-1);
    }
    else
    {
        /* It's either a folder or a file. */
        return filename.substr(sep_pos+1);
    }

}

