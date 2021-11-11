#include <iostream>
#include <windows.h>
#include <winbase.h>
#include <tchar.h>
#include <fstream>
#include <stdio.h>

#include "file_directory.h"

using namespace std;

string str_replace(string src, string tgt, string rpc)
{
    if(tgt.length()==0)
        return src;
    unsigned long found_inc = rpc.length() > 0 ? rpc.length() : 1;
    size_t found = 0;
    found = src.find(tgt);
    while( found != string::npos && found < src.length())
    {
        src = src.substr(0,found) + rpc + src.substr(found + tgt.length());
        found = src.find(tgt,found+found_inc);
    }
    return src;
}

inline string ltrim(string l_string)
{
    if(l_string.find_first_not_of(" ") != string::npos)
        return l_string.substr(l_string.find_first_not_of(" "));
    return "";
}

inline string rtrim(string src)
{
    if(src.length()==0)
        return "";

    unsigned long i = 0;
    for(i = src.length()-1; ; i--)
    {
        if(src.substr(i,1).compare(" ") != 0 || i == 0)
            break;
    }
    return src.substr(0,i+1);
}

inline string trim(string t_string)
{
    return ltrim(rtrim(t_string));
}

int main(int argc, char *argv[])
{
    string dp = argv[0];

    #ifdef RC_WINDOWS
    string path_separator = "\\";
    #else
    string path_separator = "/";
    #endif // RC_WINDOWS

    int parent_dir_index = dp.find_last_of(path_separator);

    dp = (parent_dir_index != string::npos) ? dp.substr(0, parent_dir_index) : ".";
    dp = rc_absFilePath(dp);

    string rom_path = "";
    string emu_path = "";
    string extra_flags = "";
    string launch_cmd = "";

    string line = "";
    string config_file_path = dp + "\\config.txt";
    ifstream config_file(config_file_path.c_str());
    if (config_file.is_open())
    {
        while ( getline (config_file,line) )
        {
            if(line.substr(0, 4).compare("ROM:")==0)
            {
                line = trim(line.substr(4));
                //cout << "ISO located at: (" << line << ")" << endl;
                rom_path = line;
            }
            else if(line.substr(0,4).compare("EMU:")==0)
            {
                line = trim(line.substr(4));
                //cout << "Emulator located at: (" << line << ")" << endl;
                emu_path = line;
            }
            else if(line.substr(0,4).compare("CMD:")==0)
            {
                line = trim(line.substr(4));
                line = str_replace(line, "${EMU}", emu_path);
                line = str_replace(line, "${ROM}", rom_path);
                line = str_replace(line, "${FLG}", extra_flags);
                launch_cmd = line;
            }
            else if(line.substr(0,4).compare("FLG:")==0)
            {
                line = trim(line.substr(4));
                //cout << "Flags: " << line << endl;
                extra_flags = line;
            }
        }
        config_file.close();
    }
    else
    {
        cout << "could not open: " << config_file_path << endl;
        return 0;
    }

    system(launch_cmd.c_str());

    //cout << "cmd: " << launch_cmd << endl << endl;

    return 0;
}
