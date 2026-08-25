//#include <windows.h>
#include <iostream>
//#include <iomanip>
//#include <sstream>
#include <fstream>
#include <vector>
//#include <deque>
//#include <list>
//#include <forward_list>
//#include <array>
//#include <set>
//#include <map>
//#include <unordered_set>
#include <unordered_map>
//#include <stack>
//#include <queue>
#include <string>
#include <cstring>
//#include <cctype>
#include <algorithm>
//#include <iterator>
//#include <numeric>
//#include <functional>
//#include <cmath>
//#include <cstdlib>
#include <ctime>
#include <random>
#include <chrono>
#include <exception>
#include <cassert>
#include <stdexcept>
#include <typeinfo>
#include <type_traits>
#include <utility>
#include <tuple>
#include <initializer_list>
#include <memory>
#include <new>
#include <thread>
#include <mutex>
#include <atomic>
#include <future>
#include <bitset>
#include <climits>
#include <cfloat>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <regex>
#include <filesystem>

//You must keep:iostream filesystem fstream vector string cstring math chrono ctime unordered_map

using namespace std;

unordered_map<string, string> config;
unordered_map<string, string> aliases;
vector<string> history;

bool lapClear=false;
bool fileOpreate=true;
bool exitWhenNoAccess=false;
bool limited=false;

string getTime(const string& format = "%Y%m%d%H%M%S") {
    auto now = chrono::system_clock::now();
    time_t now_time = chrono::system_clock::to_time_t(now);
    tm* local_time = localtime(&now_time);
    char buffer[100];
    strftime(buffer, sizeof(buffer), format.c_str(), local_time);
    return string(buffer);
}

namespace fs = filesystem;

void listDirectory(const string& path) {
    fs::path target = path.empty() ? fs::current_path() : fs::path(path);

    if (!fs::exists(target)) {
        cout << "Error: Path '" << path << "' does not exist." << endl;
        return;
    }
    if (!fs::is_directory(target)) {
        cout << "Error: '" << path << "' is not a directory." << endl;
        return;
    }

    cout << "=== Directory: " << target.string() << " ===" << endl;

    for (const auto& entry : fs::directory_iterator(target)) {
        string name = entry.path().filename().string();
        if (fs::is_directory(entry.status())) {
            cout << "[DIR]  " << name << endl;
        } else {
            uintmax_t size = fs::file_size(entry);
            cout << "[FILE] " << name << " (" << size << " bytes)" << endl;
        }
    }
    cout << "=== End of directory ===" << endl;
}

void loadConfig() {
    ifstream conf("csys.conf");
    if (!conf) {
        return;
    }
    string line;
    while (getline(conf, line)) {
        if (line.empty() || line[0] == '#') continue;
        size_t eq = line.find('=');
        if (eq != string::npos) {
            string key = line.substr(0, eq);
            string val = line.substr(eq + 1);
        }
    }
    if (config["default_mode"] == "limited") limited = true;
}

void readFile(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cout << "Error: Cannot open file '" << filename << "'" << endl;
        return;
    }
    string line;
    while (getline(file, line)) {
        cout << line << endl;
    }
    file.close();
    cout << "=== End of file ===" << endl;
}

void writeFile(const string& filename) {
    ofstream file(filename);
    if (!file) {
        cout << "Error: Cannot write to file '" << filename << "'" << endl;
        return;
    }
    cout << "Enter new content. Type 'END' on a new line to finish:" << endl;
    string line;
    cin.ignore();
    while (getline(cin, line)) {
        if (line == "END") break;
        file << line << endl;
    }
    file.close();
    cout << "File '" << filename << "' updated successfully." << endl;
}

int main() {
    cout<<"Please choose a mode."<<endl<<"Type in \"U\" to use unlimited mode,and type in \"L\" to use limited mode."<<endl;
    //cout<<"If you are using Windows device,it\'s recommend to use unlimited mode.If not,it\'s recommend to use limited mode."<<endl;
    string mode;
    //cin >> mode;
    //cin.ignore();
    getline(cin,mode);
    int permission=0;
    //bool limited=false;
    if(mode=="L"||mode=="l") {
        limited=true;
        permission=1;
    }
    else if(mode=="U"||mode=="u") {
        limited=false;
        permission=2;
    }
    else {
        cout<<"Unable to get the result.Please check:"<<endl<<"1.Did you type in a wrong letter?"<<endl<<"2.Did you type in extra letters?"<<endl<<"Then restart and try again."<<endl;
        return 1;
    }

    //mode=0;    //Set back.
    string note="Nothing here yet.";
    //  unsigned long long a=0,b=0,ans=0;
    //  double resultDivision=0.0;
    //  int countWay=0;
    string command="";
    unsigned long long typedInTimes=0;
    unsigned long long gotTimeCounter=0;
    unsigned long long deletedNoteTimes=0;
    unsigned long long typedLettersNumber=0;

    chrono::steady_clock::time_point sw_start;
    chrono::steady_clock::time_point sw_lap;
    bool sw_running = false;
    vector<double> lap_times;

    auto program_start = chrono::steady_clock::now();

    cout << "\033[2J\033[1;1H";
    cout<<"You are in ";
    if(limited==true) cout<<"Limited mode, some functions are unable to use.";
    else cout<<"Unlimited mode, all functions are able to use.";
    cout<<endl;
    cout<<"Type in a command to start."<<endl<<"If you need help,type in \"help\" or \"?\"."<<endl<<endl;

    int y=-1;

    //cin.ignore();

    while(y<0) {
        cout<<"CSYS:\\> ";

        getline(cin,command);
        cout<<endl;

        typedInTimes++;
        typedLettersNumber += command.length();
        if (aliases.count(command)) {
            command = aliases[command];
        }

        if (!command.empty()) {
            history.push_back(command);
        }

        if(command.rfind("help", 0)==0||command.rfind("?", 0)==0) {
            cout << R"(
=== CSYS Commands ===

time             - Show current time    
calc             - Calculator mode    
note             - Show current note    
setnote          - Set/update note    
delnote          - Delete note    
stats            - Show usage statistics    
clear            - Clear screen    
exit             - Exit    
help/?           - Get help on commands    
file             - Use and change files by commands,some functions will require Unlimited mode    
 file read       - Use "file read [file name]" to show the content
 file change     - Use "file change [file name]" to change the content,requires Unlimited mode,and might be failed if no access     
dir              - Use "dir [path]" to show all files and folders under the path    
ver              - Show the system version
chamod           - Change the mode you are using
stopwatch        - Stopwatch mode
 stopwatch start - Start stopwatch
 stopwatch stop  - Stop stopwatch
 stopwatch lap   - Record lap time
 stopwatch reset - Reset stopwatch
 stopwatch status- Show stopwatch status
move             - Move a file,Unlimited mode required
uptime           - Show CSYS system uptime
copy             - Copy a file,Unlimited mode required
debug            - Show the data
settings         - Show and change settings
:wq              - Save data and exit,can be controlled

Type any command and press Enter.
        )" << endl;
        }

        else if (command.rfind("time", 0)==0) {
            cout << getTime("%Y-%m-%d %H:%M:%S") << endl;
            gotTimeCounter++;
        }

        else if (command.rfind("chamod", 0)==0) {
            if(limited==false) {
                limited=true;
                cout<<"Now you are in Limited mode, some functions are unable to use.";
            }
            else {
                limited=false;
                cout<<"Now you are in Unlimited mode, all functions are able to use.";
            }
            cout<<endl;
        }

        else if (command.rfind("ver", 0)==0) {
            cout << R"(
        ========================================
          CSYS Ver 4.0.0
          Made by lkjy(lkjy-coding)
          Follow the MIT License
          Open sourced
          Powered by C++
          (Also be able to use <bits/stdc++.h>
          to compile with less time)
        ========================================
    )" << endl;
        }

        else if (command.rfind("note", 0)==0) {
            cout << "Note: " << note << endl;
        }

        else if (command.rfind("setnote", 0)==0) {
            cout << "Enter new note: ";
            //cin.ignore();
            getline(cin, note);
            cout << "Note updated." << endl;
        }

        else if (command.rfind("delnote", 0)==0) {
            note = "Nothing here yet.";
            deletedNoteTimes++;
            cout << "Note deleted." << endl;
        }

        else if (command.rfind("file read", 0) == 0) {
            string filename = command.substr(10);
            size_t start = filename.find_first_not_of(" \t");
            if (start != string::npos) filename = filename.substr(start);
            else {
                cout << "Error: No filename specified." << endl;
                continue;
            }

            readFile(filename);
        }
        else if (command.rfind("file change", 0) == 0) {
            if (limited) {
                cout << "Error: 'file change' requires Unlimited mode." << endl;
                continue;
            }
            string filename = command.substr(12);
            size_t start = filename.find_first_not_of(" \t");
            if (start != string::npos) filename = filename.substr(start);
            else {
                cout << "Error: No filename specified." << endl;
                continue;
            }

            writeFile(filename);
        }
        else if (command == "stats") {
            cout << "=== CSYS Statistics ===" << endl;
            cout << "Commands typed:  " << typedInTimes << endl;
            cout << "Letters typed:   " << typedLettersNumber << endl;
            cout << "Time checked:    " << gotTimeCounter << endl;
            cout << "Notes deleted:   " << deletedNoteTimes << endl;
            cout << "Mode:            " << (limited ? "Limited" : "Unlimited") << endl;
        }
        else if (command == "debug") {
            cout << "=== CSYS Data ===" << endl;
            cout << "Commands typed:  " << typedInTimes << endl;
            cout << "Letters typed:   " << typedLettersNumber << endl;
            cout << "Time checked:    " << gotTimeCounter << endl;
            cout << "Notes deleted:   " << deletedNoteTimes << endl;
            cout << "Mode:            " << (limited ? "Limited" : "Unlimited") << endl;
            cout << "Permission:      " << permission << endl;
            cout << "Version:         4.0.0" << endl;
            cout << "License:         MIT License" << endl;
        }
        else if(command=="calc") {
            cout<<"First decide which operation to perform. 1,2,3,4,5,6 represent addition, subtraction, multiplication, division, exponentiation, and square root respectively."<<endl;
            int ans1;
            cin>>ans1;
            if(ans1==1) {
                cout<<"Okay, now enter two numbers as the two addends."<<endl;
                unsigned long long aaa;
                unsigned long long bbb;
                cin>>aaa>>bbb;
                unsigned long long ans_plus;
                ans_plus=(aaa+bbb);
                cout<<"The answer is: "<<ans_plus<<endl;
            }
            else if(ans1==2) {
                cout<<"Okay, now enter two numbers as the minuend and subtrahend."<<endl;
                unsigned long long aaa;
                unsigned long long bbb;
                cin>>aaa>>bbb;
                unsigned long long ans_m;
                ans_m=(aaa-bbb);
                cout<<"The answer is: "<<ans_m<<endl;
            }
            else if(ans1==3) {
                cout<<"Okay, now enter two numbers as the two multipliers."<<endl;
                unsigned long long aaa;
                unsigned long long bbb;
                cin>>aaa>>bbb;
                unsigned long long ans_c;
                ans_c=(aaa*bbb);
                cout<<"The answer is: "<<ans_c<<endl;
            }
            else if(ans1==4) {
                cout<<"Okay, now enter two numbers as the dividend and divisor."<<endl;
                double aaa;
                double bbb;
                cin>>aaa>>bbb;
                double ans_ch;
                ans_ch=(aaa/bbb*1.0);
                cout<<"The answer is: ";
                printf("%.10f\n",ans_ch);
            }
            else if(ans1==5) {
                cout<<"Okay, now enter two numbers as the base and the exponent. (If the final output is 0, it means overflow)"<<endl;
                unsigned long long aaa;
                unsigned long long bbb;
                cin>>aaa>>bbb;
                unsigned long long ans_cf=1;
                if(bbb==0) ans_cf=1;
                else ans_cf=pow(aaa,bbb);
                cout<<"The answer is: "<<ans_cf<<endl;
            }
            else if(ans1==6) {
                cout<<"Okay, now enter the first number."<<endl;
                double aaa;
                cin>>aaa;
                double ans_s;
                ans_s=sqrt(aaa);
                cout<<"The answer is: ";
                printf("%.10f\n",ans_s);
            }
            else {
                cout<<"This number doesn't seem to be supported. Try entering any integer between 1 and 6 again?"<<endl;
            }
            cin.ignore();
        }

        else if (command.rfind("settings", 0) == 0) {
            int yy=-1;
            while(yy<0) {
                //cout<<"Please select an opinion:"
                cout << R"(
        ========================================
          Please select an option:
          [L]Lap settings->When you stopped a stopwatch,will the system clear the recorded lap?
          [F]File Opreate->Will the system create or change a file while saving and exiting?
          [W]":wq" settings->Do you want the ":wq" command exit without saving when no access?
          [Q]Quit->Just quit.
        ========================================
    )" << endl;
                string command_set="";
                string comuse="";
                getline(cin,command_set);
                if(command_set=="L"||command_set=="l") {
                    cout << R"(
        ========================================
          Please select an option:
          [Y]Yes->I want to clear the recorded lap when stop a stopwatch.
          [N]No->I don't want to clear the recorded lap when stop a stopwatch.
          [Q]Quit->Just quit.
        ========================================
    )" << endl;
                    getline(cin,comuse);
                    if(comuse=="Y"||comuse=="y") {
                        lapClear=true;
                        cout<<"The system will clear the recorded lap."<<endl;
                    }
                    else if(comuse=="N"||comuse=="n") {
                        lapClear=false;
                        cout<<"The system won\'t clear the recorded lap."<<endl;
                    }
                    else continue;
                }
                else if(command_set=="F"||command_set=="f") {
                    cout << R"(
        ========================================
          Please select an option:
          [Y]Yes->I want the system to create and change the files.
          [N]No->I don't want the system to create and change the files.
          [Q]Quit->Just quit.
        ========================================
    )" << endl;
                    getline(cin,comuse);
                    if(comuse=="Y"||comuse=="y") {
                        fileOpreate=true;
                        cout<<"The system will create and change the files."<<endl;
                    }
                    else if(comuse=="N"||comuse=="n") {
                        fileOpreate=false;
                        cout<<"The system won\'t create or change the files."<<endl;
                    }
                    else continue;
                }

                if(command_set=="W"||command_set=="w") {
                    cout << R"(
        ========================================
          Please select an option:
          [Y]Yes->I want the command to exit when no access to save data.
          [N]No->I don't want the command to exit when no access to save data.
          [Q]Quit->Just quit.
        ========================================
    )" << endl;
                    getline(cin,comuse);
                    if(comuse=="Y"||comuse=="y") {
                        exitWhenNoAccess=true;
                        cout<<"The command will exit when no access to save data."<<endl;
                    }
                    else if(comuse=="N"||comuse=="n") {
                        exitWhenNoAccess=false;
                        cout<<"The command won\'t exit when no access to save data."<<endl;
                    }
                    else continue;
                }

                else break;

            }
        }

        else if (command.rfind("stopwatch", 0) == 0) {
            string sub = command.substr(10);
            size_t start = sub.find_first_not_of(" \t");
            if (start != string::npos) sub = sub.substr(start);

            if (sub == "start") {
                if (sw_running) {
                    cout << "Stopwatch already running." << endl;
                } else {
                    sw_start = chrono::steady_clock::now();
                    sw_lap = sw_start;
                    sw_running = true;
                    if(lapClear==true) {
                        lap_times.clear();
                    }
                    cout << "Stopwatch started." << endl;
                }
            }
            else if (sub == "stop") {
                if (!sw_running) {
                    cout << "Stopwatch not running." << endl;
                } else {
                    auto now = chrono::steady_clock::now();
                    double total = chrono::duration<double>(now - sw_start).count();
                    sw_running = false;
                    cout << "Stopwatch stopped. Total time: " << total << " seconds" << endl;
                    if (!lap_times.empty()) {
                        cout << "Laps recorded: " << lap_times.size() << endl;
                    }
                }
            }
            else if (sub == "lap") {
                if (!sw_running) {
                    cout << "Stopwatch not running." << endl;
                } else {
                    auto now = chrono::steady_clock::now();
                    double lap = chrono::duration<double>(now - sw_lap).count();
                    lap_times.push_back(lap);
                    sw_lap = now;
                    cout << "Lap " << lap_times.size() << ": " << lap << " seconds" << endl;
                }
            }
            else if (sub == "reset") {
                sw_running = false;
                //if(lapClear==true) {
                lap_times.clear();
                //}
                cout << "Stopwatch reset." << endl;
            }
            else if (sub == "status") {
                if (sw_running) {
                    auto now = chrono::steady_clock::now();
                    double elapsed = chrono::duration<double>(now - sw_start).count();
                    cout << "Stopwatch is running. Elapsed: " << elapsed << " seconds" << endl;
                    cout << "Laps: " << lap_times.size() << endl;
                } else {
                    cout << "Stopwatch is stopped." << endl;
                    cout << "Laps recorded: " << lap_times.size() << endl;
                }
            }
            else {
                cout << "Usage: stopwatch [start|stop|lap|reset|status]" << endl;
            }
        }

        else if (command.rfind("dir", 0) == 0) {
            string path = command.substr(3);
            size_t start = path.find_first_not_of(" \t");
            if (start != string::npos) path = path.substr(start);
            listDirectory(path);
        }
        else if (command.rfind("move", 0) == 0) {
            if (limited) {
                cout << "Error: 'move' requires Unlimited mode." << endl;
                continue;
            }

            string rest = command.substr(4);
            size_t start = rest.find_first_not_of(" \t");

            if (start == string::npos) {
                cout << "Error: No arguments specified." << endl;
                cout << "Usage: move [source] [destination]" << endl;
                continue;
            }

            rest = rest.substr(start);

            size_t sep = rest.find_first_of(" \t");
            if (sep == string::npos) {
                cout << "Error: Missing destination." << endl;
                cout << "Usage: move [source] [destination]" << endl;
                continue;
            }

            string src = rest.substr(0, sep);
            string dst = rest.substr(sep + 1);

            src.erase(0, src.find_first_not_of(" \t"));
            src.erase(src.find_last_not_of(" \t") + 1);
            dst.erase(0, dst.find_first_not_of(" \t"));
            dst.erase(dst.find_last_not_of(" \t") + 1);

            if (src.empty() || dst.empty()) {
                cout << "Error: Invalid arguments." << endl;
                cout << "Usage: move [source] [destination]" << endl;
                continue;
            }

            try {
                fs::path src_path(src);
                fs::path dst_path(dst);

                if (!fs::exists(src_path)) {
                    cout << "Error: Source '" << src << "' does not exist." << endl;
                    continue;
                }

                if (fs::exists(dst_path)) {
                    cout << "Warning: Destination already exists. Overwrite? (Y/N): ";
                    string confirm;
                    getline(cin, confirm);
                    if (confirm != "y" && confirm != "Y") {
                        cout << "Move cancelled." << endl;
                        continue;
                    }
                }

                fs::rename(src_path, dst_path);
                cout << "Moved '" << src << "' -> '" << dst << "'" << endl;
            } catch (const exception& e) {
                cout << "Error: " << e.what() << endl;
            }
        }

        else if (command == "uptime") {
            auto now = chrono::steady_clock::now();
            auto elapsed = chrono::duration_cast<chrono::seconds>(now - program_start).count();

            long long days = elapsed / 86400;
            long long hours = (elapsed % 86400) / 3600;
            long long minutes = (elapsed % 3600) / 60;
            long long seconds = elapsed % 60;

            cout << "CSYS uptime: ";
            if (days > 0) cout << days << "d ";
            if (hours > 0 || days > 0) cout << hours << "h ";
            if (minutes > 0 || hours > 0 || days > 0) cout << minutes << "m ";
            cout << seconds << "s" << endl;
        }

        else if (command == "clear") {
            cout << "\033[2J\033[1;1H";
        }

        else if (command.rfind("copy", 0) == 0) {
            if (limited) {
                cout << "Error: 'copy' requires Unlimited mode. You should use the \"chamod\" command." << endl;
                continue;
            }

            string rest = command.substr(4);
            size_t start = rest.find_first_not_of(" \t");
            if (start == string::npos) {
                cout << "Error: No arguments specified. Check it!" << endl;
                cout << "Usage: copy [source] [destination]" << endl;
                continue;
            }
            rest = rest.substr(start);

            size_t sep = rest.find_first_of(" \t");
            if (sep == string::npos) {
                cout << "Error: Missing destination. You\'d better to choose one." << endl;
                cout << "Usage: copy [source] [destination]" << endl;
                continue;
            }

            string src = rest.substr(0, sep);
            string dst = rest.substr(sep + 1);

            src.erase(0, src.find_first_not_of(" \t"));
            src.erase(src.find_last_not_of(" \t") + 1);
            dst.erase(0, dst.find_first_not_of(" \t"));
            dst.erase(dst.find_last_not_of(" \t") + 1);

            if (src.empty() || dst.empty()) {
                cout << "Error: Invalid arguments. Is it designed?" << endl;
                cout << "Usage: copy [source] [destination]" << endl;
                continue;
            }

            try {
                fs::path src_path(src);
                fs::path dst_path(dst);

                if (!fs::exists(src_path)) {
                    cout << "Error: Source '" << src << "' does not exist." << endl;
                    continue;
                }
                if (!fs::is_regular_file(src_path)) {
                    cout << "Error: Source is not a regular file." << endl;
                    continue;
                }
                if (fs::exists(dst_path)) {
                    cout << "Warning: Destination already exists. Overwrite? (Y/N): ";
                    string confirm;
                    getline(cin, confirm);
                    if (confirm != "y" && confirm != "Y") {
                        cout << "Copy cancelled." << endl;
                        continue;
                    }
                }

                fs::copy(src_path, dst_path, fs::copy_options::overwrite_existing);
                cout << "Copied '" << src << "' -> '" << dst << "'" << endl;
            } catch (const exception& e) {
                cout << "Error: "{} << e.what() << endl;
            }
        }

        //else if(command=="file") cout<<"\"File\" is not a correct command."

        else if(command.empty()) continue;
        else if(command=="exit") break;

        else if (command.rfind("unalias ", 0) == 0) {
            string alias_name = command.substr(8);
            size_t start = alias_name.find_first_not_of(" \t");
            if (start == string::npos) {
                cout << "Error: No alias name specified." << endl;
                cout << "Usage: unalias [alias_name]" << endl;
                continue;
            }
            alias_name = alias_name.substr(start);
            alias_name.erase(alias_name.find_last_not_of(" \t") + 1);

            if (aliases.erase(alias_name)) {
                cout << "Alias '" << alias_name << "' removed." << endl;
            } else {
                cout << "Error: Alias '" << alias_name << "' does not exist." << endl;
            }
        }
        else if (command == "unalias") {
            cout << "Error: No alias name specified." << endl;
            cout << "Usage: unalias [alias_name]" << endl;
        }
        else if (command.rfind("alias ", 0) == 0) {
            string rest = command.substr(6);
            size_t sep = rest.find(' ');
            if (sep == string::npos) {
                cout << "Error: Invalid alias format. Usage: alias [name] [command]" << endl;
                continue;
            }
            string alias_name = rest.substr(0, sep);
            string alias_cmd = rest.substr(sep + 1);
            // 去除首尾空格
            alias_name.erase(0, alias_name.find_first_not_of(" \t"));
            alias_name.erase(alias_name.find_last_not_of(" \t") + 1);
            alias_cmd.erase(0, alias_cmd.find_first_not_of(" \t"));
            alias_cmd.erase(alias_cmd.find_last_not_of(" \t") + 1);

            if (alias_name.empty() || alias_cmd.empty()) {
                cout << "Error: Invalid alias format. Usage: alias [name] [command]" << endl;
                continue;
            }

            aliases[alias_name] = alias_cmd;
            cout << "Alias '" << alias_name << "' set to '" << alias_cmd << "'" << endl;
        }

        else if (command == "history") {
            for (size_t i = 0; i < history.size(); ++i) {
                cout << i+1 << "  " << history[i] << endl;
            }
        }

        else if(command==":wq") {
            if (note != "Nothing here yet."&&fileOpreate==true) {
                ofstream file("csys_note.txt");
                if (file) {
                    file << note;
                    file.close();
                    cout << "Note saved to csys_note.txt" << endl;
                }
            }
            else cout<<"No access."<<endl;
            if(fileOpreate==true) {
                ofstream hist("csys_history.txt");
                if (hist) {
                    for (const auto& cmd : history) {
                        hist << cmd << endl;
                    }
                    hist.close();
                    cout << "History saved to csys_history.txt (" << history.size() << " commands)" << endl;
                }
            }
            else cout<<"No access."<<endl;

            if(fileOpreate==true)  return 0;  //Ran command.
            else if(exitWhenNoAccess==true) return 0;
            else break;
        }

        else cout<<"\""<<command<<"\""<<" is not a correct command."<<endl;

        cout<<endl;
    }

    return 0;
}

//Hill C++, 55 yuan for a ticket.

//Tunemah!!!
