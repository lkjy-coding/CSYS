#pragma GCC optimize("O2")
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
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <random>
#include <chrono>
//#include <exception>
//#include <cassert>
//#include <stdexcept>
//#include <typeinfo>
//#include <type_traits>
//#include <utility>
//#include <tuple>
//#include <initializer_list>
//#include <memory>
//#include <new>
//#include <thread>
//#include <mutex>
//#include <atomic>
//#include <future>
//#include <bitset>
//#include <climits>
//#include <cfloat>
//#include <cstdint>
//#include <cstdio>
//#include <limits>
//#include <regex>
/*#include <math>*/
#include <filesystem>

//You must keep:iostream filesystem fstream vector string cstring cmath chrono ctime unordered_map cstdlib random

using namespace std;

unordered_map<string, string> config;
unordered_map<string, string> aliases;
vector<string> history;

bool lapClear=false;
bool fileOpreate=true;
bool exitWhenNoAccess=false;
bool limited=false;
bool hexTestOutput=false;
bool easterEggs=true;
bool banThisTime=false;
bool blockCommandsWhileGamesRunning=true;

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

void openUrl(const std::string& url) {
    std::string command;
    int ret = -1;

#if defined(__ANDROID__)
    if (system("which termux-open > /dev/null 2>&1") == 0) {
        command = "termux-open " + url;
    } else {
        command = "am start -a android.intent.action.VIEW -d " + url;
    }
#elif defined(_WIN32) || defined(_WIN64)
    command = "start " + url;
#elif defined(__APPLE__)
    command = "open " + url;
#elif defined(__linux__)
    command = "xdg-open " + url;
#else
    cout << "Error: Unsupported platform." << endl;
    cout << "Please copy this URL manually:" << endl;
    cout << url << endl;
    return;
#endif

    ret = std::system(command.c_str());

    if (ret != 0) {
        cout << "Oops,unable to open automatically." << endl;
        cout << "Please copy this URL and open it in your browser(plz):" << endl;
        cout << url << endl;
    }
}

struct ScheduledTask {
    int id;
    string command;
    chrono::steady_clock::time_point next_execute;
    chrono::seconds interval;
    bool active;
};

vector<ScheduledTask> tasks;
int next_task_id = 1;

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

vector<string> sentences = {
    "Never gonna give you up.",
    "Never gonna let you down.",
    "Never gonna run around and desert you.",
    "Keep moving forward.",
    "We\'re not strangers to love...",
    "awawawawawawawawawa",
    "missingno",
    "asdfghjkl",
    "qwerty",
    "Buy me a coffee :D (kidding)",
    "lkjy did that.",
    "Yeah,not lkjy,it\'s ikjy(awa).",
    // Many people will see my ID as "ikjy" by mistake,so I added it :D
    "We have the best code around the world!--even the bugs are flying...",
    "Chinese BUGs can fly.",
    // zhong guo ren neng fei (awa)
    "Why are you playing \"Haruhikage\"?",
    "Bilibili Cheers (of course)",
    "I am waiting for you on the Shenyang Street!",
    "How much are the watermelons,bro?",
    "Ahahahahaha,here is the Chicken soup!",
    "The truth of the block game!",
    "Potato servers.",
    "Don\'t look behind you...(just kidding don\'t be worried :D)",
    "5,10,9,8,7,6...",
    "Houses are meant to be sold.If they're all sold out,why would I still build them?",
    "You go!",
};

bool mine_running = false;
int mine_rows = 9;
int mine_cols = 9;
int mine_count = 10;
vector<vector<int>> mine_board;      // -1=Bomb, 0-8=num
vector<vector<bool>> mine_revealed;  // true=revealed
vector<vector<bool>> mine_flagged;   // true=flagged
bool mine_game_over = false;
bool mine_won = false;
int mine_revealed_count = 0;
int mine_flagged_count = 0;
time_t mine_start_time;

void initMineBoard() {
    mine_board.assign(mine_rows, vector<int>(mine_cols, 0));
    mine_revealed.assign(mine_rows, vector<bool>(mine_cols, false));
    mine_flagged.assign(mine_rows, vector<bool>(mine_cols, false));
    mine_revealed_count = 0;
    mine_flagged_count = 0;
    mine_game_over = false;
    mine_won = false;
    mine_start_time = time(nullptr);

    int placed = 0;
    while (placed < mine_count) {
        int r = rand() % mine_rows;
        int c = rand() % mine_cols;
        if (mine_board[r][c] != -1) {
            mine_board[r][c] = -1;
            placed++;
        }
    }

    for (int r = 0; r < mine_rows; r++) {
        for (int c = 0; c < mine_cols; c++) {
            if (mine_board[r][c] == -1) continue;
            int cnt = 0;
            for (int dr = -1; dr <= 1; dr++) {
                for (int dc = -1; dc <= 1; dc++) {
                    int nr = r + dr, nc = c + dc;
                    if (nr >= 0 && nr < mine_rows && nc >= 0 && nc < mine_cols && mine_board[nr][nc] == -1) {
                        cnt++;
                    }
                }
            }
            mine_board[r][c] = cnt;
        }
    }
}

void expandMineCell(int r, int c) {
    if (r < 0 || r >= mine_rows || c < 0 || c >= mine_cols) return;
    if (mine_revealed[r][c] || mine_flagged[r][c]) return;
    if (mine_board[r][c] == -1) return;

    mine_revealed[r][c] = true;
    mine_revealed_count++;

    if (mine_board[r][c] == 0) {
        for (int dr = -1; dr <= 1; dr++) {
            for (int dc = -1; dc <= 1; dc++) {
                expandMineCell(r + dr, c + dc);
            }
        }
    }
}

void printMineBoard() {
    cout << "   ";
    for (int c = 0; c < mine_cols; c++) {
        cout << " " << c << " ";
    }
    cout << endl;

    for (int r = 0; r < mine_rows; r++) {
        cout << " " << r << " ";
        for (int c = 0; c < mine_cols; c++) {
            if (mine_flagged[r][c] && !mine_revealed[r][c]) {
                cout << " F ";
            } else if (!mine_revealed[r][c] && !mine_game_over) {
                cout << " O ";
            } else if (mine_revealed[r][c]) {
                if (mine_board[r][c] == -1) {
                    cout << " X ";
                } else if (mine_board[r][c] == 0) {
                    cout << " S ";
                } else {
                    cout << " " << mine_board[r][c] << " ";
                }
            } else if (mine_game_over && mine_board[r][c] == -1 && !mine_flagged[r][c]) {
                cout << " X ";
            } else if (mine_game_over && mine_flagged[r][c] && mine_board[r][c] == -1) {
                cout << " C ";
            } else if (mine_game_over && mine_flagged[r][c] && mine_board[r][c] != -1) {
                cout << " W ";
            }
        }
        cout << endl;
    }
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
    srand(time(nullptr));
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
        permission=2;
    }
    else if(mode=="U"||mode=="u") {
        limited=false;
        permission=3;
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

    unsigned long long max_guesses = 7;
    unsigned long long remaining_guesses = 7;
    bool game_active = false;
    int target_number = 0;
    unsigned long long total_games_played = 0;
    unsigned long long total_games_won = 0;
//  bool game_active = false;
    string game_type = "";
//  unsigned long long max_guesses = 7;
//  unsigned long long remaining_guesses = 7;
//  int target_number = 0;
//  unsigned long long total_games_played = 0;
//  unsigned long long total_games_won = 0;
    bool game_running = false;

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

loop_start:

        cout<<endl;

        cout<<"CSYS:\\> ";

        getline(cin,command);

        if(banThisTime) {
            if(command=="unlock confirm") {
                banThisTime=false;
                cout<<"Unlocked."<<endl;
            }
            else {
                cout<<"System Locked. Access Denied."<<endl;
                continue;
            }
        }

        if(blockCommandsWhileGamesRunning) {
            if (game_running) {
                if (command.rfind("game", 0) != 0) {
                    cout << "A game is in progress. Finish or end it before running other commands." << endl;
                    continue;
                }
            }
        }

        if(hexTestOutput) {
            if(permission>0) {
                cout << endl;
                cout << "DEBUG: command length=" << command.length() << ", hex=";
                for (char c : command) cout << hex << (int)c << " ";
                cout << dec << endl;
            }
            else cout<<"No Access on Debug Tools."<<endl;
        }

        cout<<endl;

        auto now = chrono::steady_clock::now();
        for (auto& task : tasks) {
            if (!task.active) continue;
            if (now >= task.next_execute) {
                cout << "[Task " << task.id << "] Executing: " << task.command << endl;
                if (task.interval.count() > 0) {
                    task.next_execute = now + task.interval;
                } else {
                    task.active = false;
                }
            }
        }

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

time                     - Show current time    
calc                     - Calculator mode    
note                     - Show current note    
setnote                  - Set/update note    
delnote                  - Delete note    
stats                    - Show usage statistics    
clear                    - Clear screen    
exit                     - Exit    
help/?                   - Get help on commands    
file                     - Use and change files by commands,some functions will require Unlimited mode    
 file read               - Use "file read [file name]" to show the content
 file change             - Use "file change [file name]" to change the content,requires Unlimited mode,and might be failed if no access     
dir                      - Use "dir [path]" to show all files and folders under the path    
ver                      - Show the system version
chamod                   - Change the mode you are using
stopwatch                - Stopwatch mode
 stopwatch start         - Start stopwatch
 stopwatch stop          - Stop stopwatch
 stopwatch lap           - Record lap time
 stopwatch reset         - Reset stopwatch
 stopwatch status        - Show stopwatch status
move                     - Move a file,Unlimited mode required
uptime                   - Show CSYS system uptime
copy                     - Copy a file,Unlimited mode required
debug                    - Show the data
settings                 - Show and change settings
:wq                      - Save data and exit,can be controlled
timer                    - Execute a command after N seconds
 repeat                  - Execute a command repeatedly every N seconds
 tasks                   - Show active scheduled tasks
 taskkill                - Kill a scheduled task by ID
format                   - Clear user data
 format confirm          - Use it to confirm your command
 format --keep-settings  - Use it before using "confirm" command when you want to save settings data
sens                     - Show me sentences!
lockon                   - Disable to use the system
unlock confirm           - Enable to use the system again 
game                     - Game system
 game start guess        - Start a number guessing game
 game guess <number>     - Make a guess in the number game
 game hint               - Get a hint for the number game
 game settings           - Adjust max guesses for the number game
 game end                - End current game
 game start mine         - Start a minesweeper game
 mine reveal <r> <c>     - Reveal a cell in minesweeper
 mine flag <r> <c>       - Flag/unflag a cell in minesweeper

Type any command and press Enter.
        )" << endl;
        }

        else if (command.rfind("timer ", 0) == 0) {
            string rest = command.substr(6);
            size_t sep = rest.find(' ');
            if (sep == string::npos) {
                cout << "Usage: timer [seconds] [command]" << endl;
                continue;
            }
            int seconds = stoi(rest.substr(0, sep));
            string cmd = rest.substr(sep + 1);

            ScheduledTask task;
            task.id = next_task_id++;
            task.command = cmd;
            task.interval = chrono::seconds(0);
            task.next_execute = chrono::steady_clock::now() + chrono::seconds(seconds);
            task.active = true;
            tasks.push_back(task);

            cout << "Task " << task.id << " scheduled in " << seconds << " seconds." << endl;
        }

        if (command.rfind("repeat ", 0) == 0) {
            string rest = command.substr(7);
            size_t sep = rest.find(' ');
            if (sep == string::npos) {
                cout << "Usage: repeat [seconds] [command]" << endl;
                continue;
            }
            int seconds = stoi(rest.substr(0, sep));
            string cmd = rest.substr(sep + 1);

            if (seconds <= 0) {
                cout << "Error: seconds must be positive." << endl;
                continue;
            }

            ScheduledTask task;
            task.id = next_task_id++;
            task.command = cmd;
            task.interval = chrono::seconds(seconds);
            task.next_execute = chrono::steady_clock::now() + chrono::seconds(seconds);
            task.active = true;
            tasks.push_back(task);

            cout << "Task " << task.id << " will repeat every " << seconds << " seconds." << endl;
        }

        else if (command == "tasks") {
            if(permission>=2) {
                if (tasks.empty()) {
                    cout << "No active tasks." << endl;
                } else {
                    cout << "=== Active Tasks ===" << endl;
                    for (const auto& task : tasks) {
                        if (!task.active) continue;
                        cout << "[" << task.id << "] ";
                        if (task.interval.count() > 0) {
                            cout << "Repeat every " << task.interval.count() << "s";
                        } else {
                            cout << "One-time";
                        }
                        cout << " | Command: " << task.command << endl;
                    }
                }
            }
            else cout<<"No Access."<<endl;
        }

        else if (command.rfind("game", 0) == 0) {
            if(permission>=2) {
                string rest = command.substr(4);
                size_t start = rest.find_first_not_of(" \t");
                if (start != string::npos) rest = rest.substr(start);

                if (rest == "start guess") {
                    if (game_running) {
                        cout << "A game is already running. Use 'game end' to stop it." << endl;
                        continue;
                    }
                    target_number = rand() % 100 + 1;
                    remaining_guesses = max_guesses;
                    game_type = "guess";
                    game_running = true;
                    total_games_played++;
                    cout << "I'm thinking of a number between 1 and 100." << endl;
                    cout << "You have " << max_guesses << " guesses. Use 'game guess <number>' to guess." << endl;
                    cout << "Use 'game hint' for a hint, or 'game end' to quit." << endl;
                    continue;
                }

                if (!game_running) {
                    if (rest == "end") {
                        cout << "No game is currently running." << endl;
                    } else {
                        cout << "No game is running. Use 'game start guess' to start." << endl;
                    }
                    continue;
                }

                // Only able to run while the game running.
                if (rest == "end") {
                    cout << "Game ended. The number was " << target_number << "." << endl;
                    game_running = false;
                    game_type = "";
                    continue;
                }

                if (rest == "hint") {
                    int low = max(1, target_number - 15);
                    int high = min(100, target_number + 15);
                    cout << "Remaining guesses: " << remaining_guesses << endl;
                    cout << "The number is between " << low << " and " << high << "." << endl;
                    continue;
                }

                if (rest == "settings") {
                    cout << "Current max guesses: " << max_guesses << endl;
                    cout << "Enter new max guesses (1-30, or 0 to cancel): ";
                    string input;
                    getline(cin, input);
                    try {
                        unsigned long long new_val = stoull(input);
                        if (new_val == 0) {
                            cout << "Settings unchanged." << endl;
                        } else if (new_val >= 1 && new_val <= 30) {
                            max_guesses = new_val;
                            cout << "Max guesses set to " << max_guesses << "." << endl;
                        } else {
                            cout << "Please enter a number between 1 and 30." << endl;
                        }
                    } catch (...) {
                        cout << "Invalid input. Please enter a number." << endl;
                    }
                    continue;
                }

                if (rest.rfind("guess ", 0) == 0) {
                    string num_str = rest.substr(6);
                    try {
                        int guess = stoi(num_str);
                        if (guess < 1 || guess > 100) {
                            cout << "Please enter a number between 1 and 100." << endl;
                            continue;
                        }
                        remaining_guesses--;
                        if (guess == target_number) {
                            cout << "You got it! The number was " << target_number << "!" << endl;
                            total_games_won++;
                            cout << "You've won " << total_games_won << " out of " << total_games_played << " games." << endl;
                            game_running = false;
                            game_type = "";
                        } else if (remaining_guesses == 0) {
                            cout << "Out of guesses. The number was " << target_number << "." << endl;
                            game_running = false;
                            game_type = "";
                        } else if (guess < target_number) {
                            cout << "Too low. " << remaining_guesses << " guesses left." << endl;
                        } else {
                            cout << "Too high. " << remaining_guesses << " guesses left." << endl;
                        }
                    } catch (...) {
                        cout << "Invalid guess. Use 'game guess <number>'." << endl;
                    }
                    continue;
                }
                else if (rest == "start mine") {
                    if (game_running) {
                        cout << "A game is already running. Use 'game end' to stop it." << endl;
                        continue;
                    }
                    // 地图设置
                    cout << "Enter rows (5-20, default 9): ";
                    string input;
                    getline(cin, input);
                    if (!input.empty()) {
                        try {
                            int val = stoi(input);
                            if (val >= 5 && val <= 20) mine_rows = val;
                            else cout << "Invalid, using default 9." << endl;
                        } catch (...) {
                            cout << "Invalid, using default 9." << endl;
                        }
                    }

                    cout << "Enter cols (5-20, default 9): ";
                    getline(cin, input);
                    if (!input.empty()) {
                        try {
                            int val = stoi(input);
                            if (val >= 5 && val <= 20) mine_cols = val;
                            else cout << "Invalid, using default 9." << endl;
                        } catch (...) {
                            cout << "Invalid, using default 9." << endl;
                        }
                    }

                    int max_mines = mine_rows * mine_cols - 3;
                    cout << "Enter number of mines (1-" << max_mines << ", default 10): ";
                    getline(cin, input);
                    if (!input.empty()) {
                        try {
                            int val = stoi(input);
                            if (val >= 1 && val <= max_mines) mine_count = val;
                            else cout << "Invalid, using default 10." << endl;
                        } catch (...) {
                            cout << "Invalid, using default 10." << endl;
                        }
                    }

                    initMineBoard();
                    game_running = true;
                    game_type = "mine";
                    cout << "Minesweeper started! " << mine_rows << "x" << mine_cols << ", " << mine_count << " mines." << endl;
                    cout << "Commands: mine reveal <r> <c>, mine flag <r> <c>, mine end" << endl;
                    printMineBoard();
                    continue;
                }

                if (game_type == "mine") {
                    if (rest == "reveal" || rest.rfind("reveal ", 0) == 0) {
                        if (mine_game_over) {
                            cout << "Game is over. Start a new one with 'game start mine'." << endl;
                            continue;
                        }
                        string args = rest.substr(rest.find(' ') + 1);
                        size_t sp = args.find(' ');
                        if (sp == string::npos) {
                            cout << "Usage: mine reveal <row> <col>" << endl;
                            continue;
                        }
                        try {
                            int r = stoi(args.substr(0, sp));
                            int c = stoi(args.substr(sp + 1));
                            if (r < 0 || r >= mine_rows || c < 0 || c >= mine_cols) {
                                cout << "Out of bounds." << endl;
                                continue;
                            }
                            if (mine_revealed[r][c]) {
                                cout << "Cell already revealed." << endl;
                                continue;
                            }
                            if (mine_flagged[r][c]) {
                                cout << "Cell is flagged. Unflag it first." << endl;
                                continue;
                            }
                            if (mine_board[r][c] == -1) {
                                mine_game_over = true;
                                cout << "BOOM! You hit a mine." << endl;
                                printMineBoard();
                                game_running = false;
                                game_type = "";
                                continue;
                            }
                            expandMineCell(r, c);
                            printMineBoard();
                            int safe_cells = mine_rows * mine_cols - mine_count;
                            if (mine_revealed_count == safe_cells) {
                                mine_game_over = true;
                                mine_won = true;
                                cout << "You win! All safe cells revealed." << endl;
                                game_running = false;
                                game_type = "";
                            }
                        } catch (...) {
                            cout << "Invalid input. Usage: mine reveal <row> <col>" << endl;
                        }
                        continue;
                    }

                    if (rest == "flag" || rest.rfind("flag ", 0) == 0) {
                        if (mine_game_over) {
                            cout << "Game is over. Start a new one with 'game start mine'." << endl;
                            continue;
                        }
                        string args = rest.substr(rest.find(' ') + 1);
                        size_t sp = args.find(' ');
                        if (sp == string::npos) {
                            cout << "Usage: mine flag <row> <col>" << endl;
                            continue;
                        }
                        try {
                            int r = stoi(args.substr(0, sp));
                            int c = stoi(args.substr(sp + 1));
                            if (r < 0 || r >= mine_rows || c < 0 || c >= mine_cols) {
                                cout << "Out of bounds." << endl;
                                continue;
                            }
                            if (mine_revealed[r][c]) {
                                cout << "Cell already revealed." << endl;
                                continue;
                            }
                            mine_flagged[r][c] = !mine_flagged[r][c];
                            mine_flagged_count += mine_flagged[r][c] ? 1 : -1;
                            cout << "Cell " << r << "," << c << (mine_flagged[r][c] ? " flagged." : " unflagged.") << endl;
                            printMineBoard();
                        } catch (...) {
                            cout << "Invalid input. Usage: mine flag <row> <col>" << endl;
                        }
                        continue;
                    }

                    if (rest == "end") {
                        cout << "Game ended." << endl;
                        game_running = false;
                        game_type = "";
                        continue;
                    }
                }
                cout << "Invalid game command. Available: game guess <num>, game hint, game end, game settings." << endl;
            }
            else cout<<"No Access."<<endl;
        }
        else if (command.rfind("taskkill ", 0) == 0) {
            string arg = command.substr(9);
            if (arg == "all") {
                for (auto& task : tasks) task.active = false;
                cout << "All tasks killed." << endl;
            } else {
                int id = stoi(arg);
                bool found = false;
                for (auto& task : tasks) {
                    if (task.id == id) {
                        task.active = false;
                        cout << "Task " << id << " killed." << endl;
                        found = true;
                        break;
                    }
                }
                if (!found) cout << "Task " << id << " not found." << endl;
            }
        }

        else if(command=="lockon") {
            cout<<"The system will clear the screen and disable the commands.You can type in \"unlock confirm\" to enable again."<<endl;
            cout<<"To confirm,type in \"Y\"."<<endl;
            string comuuse="";
            getline(cin,comuuse);
            if(comuuse=="Y") {
                banThisTime=true;
                cout << "\033[2J\033[1;1H";
            }
        }

        else if(command=="unlock confirm") {
            if(!banThisTime) /*cout<<"Already unlocked."<<endl;*/ continue;
        }

        else if (command=="nevergonnagiveyouup"||command=="rickroll") {
            if(easterEggs) openUrl("https://b23.tv/Wmr5AIm");
            else cout << "This command requires access to show the easter eggs." << endl;
        }

        else if (command == "sens") {
            if(easterEggs) {
                if (sentences.empty()) {
                    cout << "No sentences available." << endl;
                } else {
                    int idx = rand() % sentences.size();
                    cout << sentences[idx] << endl;
                }
            }
            else cout << "This command requires access to show the easter eggs." << endl;
        }

        else if (command.rfind("format", 0) == 0) {
            string rest = command.substr(6);
            size_t start = rest.find_first_not_of(" \t");
            if (start != string::npos) rest = rest.substr(start);

            if (rest == "confirm") {
                cout << "Are you sure? This will delete all your data. (Y/N): ";
                string confirm;
                getline(cin, confirm);
                if (confirm != "y" && confirm != "Y") {
                    cout << "Format cancelled." << endl;
                    continue;
                }

                note = "Nothing here yet.";

                history.clear();

                aliases.clear();

                lapClear = false;
                fileOpreate = true;
                exitWhenNoAccess = false;
                hexTestOutput = false;

                typedInTimes = 0;
                gotTimeCounter = 0;
                deletedNoteTimes = 0;
                typedLettersNumber = 0;


                for (auto& task : tasks) task.active = false;
                tasks.clear();
                next_task_id = 1;

                cout << "CSYS has been formatted. All data cleared." << endl;
            } else if (rest == "--keep-settings") {
                note = "Nothing here yet.";
                history.clear();
                typedInTimes = 0;
                gotTimeCounter = 0;
                deletedNoteTimes = 0;
                typedLettersNumber = 0;
                cout << "Notes and history cleared. Settings preserved." << endl;
            } else if (rest.empty() || rest == "help") {
                cout << "Usage: format confirm        - Clear ALL user data" << endl;
                cout << "       format --keep-settings - Clear notes and history only" << endl;
                cout << "       format help            - Show this help" << endl;
            } else {
                cout << "Unknown option. Use 'format help' for usage." << endl;
            }
        }

        else if (command.rfind("time", 0)==0) {
            cout << getTime("%Y-%m-%d %H:%M:%S") << endl;
            gotTimeCounter++;
        }

        else if (command.rfind("chamod", 0)==0) {
            if(!limited) {
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
          CSYS Ver 6.0.0
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
            if(permission>=2) {
                cout << "=== CSYS Data ===" << endl;
                cout << "Commands typed:                 " << typedInTimes << endl;
                cout << "Letters typed:                  " << typedLettersNumber << endl;
                cout << "Time checked:                   " << gotTimeCounter << endl;
                cout << "Notes deleted:                  " << deletedNoteTimes << endl;
                cout << "Mode:                           " << (limited ? "Limited" : "Unlimited") << endl;
                cout << "Permission:                     " << permission << endl;
                cout << "Version:                        6.0.0" << endl;
                cout << "License:                        MIT License" << endl;
                cout << "exitWhenNoAccess:               " << (exitWhenNoAccess ? "true" : "false") << endl;
                cout << "fileOpreate:                    " << (fileOpreate ? "true" : "false") << endl;
                cout << "hexTestOutput:                  " << (hexTestOutput ? "true" : "false") << endl;
                cout << "easterEggs:                     " << (easterEggs ? "false" : "true") << endl;
                cout << "blockCommandsWhileGamesRunning: " << (blockCommandsWhileGamesRunning ? "true" : "false") << endl;
            }
            else cout<<"No Access."<<endl;
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
            if(permission>=2) {
                int yy=-1;
                while(yy<0) {
                    //cout<<"Please select an opinion:"
                    cout << R"(
        ========================================
          Please select an option:
          [L]Lap settings->When you stopped a stopwatch,will the system clear the recorded lap?
          [F]File opreate->Will the system create or change a file while saving and exiting?
          [W]":wq" settings->Do you want the ":wq" command exit without saving when no access?
          [H]Hex test->Will the system show the hex content when type in a command?
          [E]Easter eggs->Do you want to see the easter eggs in the system?
          [G]Game settings->Will the system refuse any common commands while games running?
          [P]Permission settings->Change your permission.
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

                    else if(command_set=="W"||command_set=="w") {
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
                    else if(command_set=="H"||command_set=="h") {
                        cout << R"(
        ========================================
          Please select an option:
          [Y]Yes->I want the system to show me the hex conten
          [N]No->I don't want the system to show me the hex content.
          [Q]Quit->Just quit.
        ========================================
    )" << endl;
                        getline(cin,comuse);
                        if(comuse=="Y"||comuse=="y") {
                            hexTestOutput=true;
                            cout<<"The system will show you the hex content."<<endl;
                        }
                        else if(comuse=="N"||comuse=="n") {
                            hexTestOutput=false;
                            cout<<"The system won\'t show you the hex content."<<endl;
                        }
                        else continue;
                    }

                    else if(command_set=="E"||command_set=="e") {
                        cout << R"(
        ========================================
          Please select an option:
          [Y]Yes->I want to see the easter eggs.
          [N]No->I don't want to see the easter eggs.
          [Q]Quit->Just quit.
        ========================================
    )" << endl;
                        getline(cin,comuse);
                        if(comuse=="Y"||comuse=="y") {
                            easterEggs=true;
                            cout<<"The system will show you the easter eggs."<<endl;
                        }
                        else if(comuse=="N"||comuse=="n") {
                            easterEggs=false;
                            cout<<"The system won\'t show you the easter eggs."<<endl;
                        }
                        else continue;
                    }

                    else if(command_set=="G"||command_set=="g") {
                        cout << R"(
        ========================================
          Please select an option:
          [Y]Yes->The system will refuse any common commands while games running.
          [N]No->The system won't refuse any common commands while games running.
          [Q]Quit->Just quit.
        ========================================
    )" << endl;
                        getline(cin,comuse);
                        if(comuse=="Y"||comuse=="y") {
                            blockCommandsWhileGamesRunning=true;
                            cout<<"The system will refuse any common commands while games running."<<endl;
                        }
                        else if(comuse=="N"||comuse=="n") {
                            blockCommandsWhileGamesRunning=false;
                            cout<<"The system won\'t refuse any common commands while games running."<<endl;
                        }
                        else continue;
                    }

                    else if(command_set=="P"||command_set=="p") {
                        if(permission<3) cout<<"No Access."<<endl;
                        else {
                            cout << R"(
        ========================================
          Please select an option:
          [P]Plus->Make my permission level higher.
          [M]Minus->Make my permission level lower.
          [Q]Quit->Just quit.
        ========================================
    )" << endl;
                            cout<<"Current permission level:"<<permission<<endl;
                            getline(cin,comuse);
                            if(comuse=="P"||comuse=="p") {
                                permission++;  // Designed
                                cout<<"Current permission level:"<<permission<<endl;
                            }
                            else if(comuse=="M"||comuse=="m") {
                                permission--;
                                cout<<"Current permission level:"<<permission<<endl;
                            }
                            else continue;
                        }
                    }

                    else break;

                }
            }
            else cout<<"No Access."<<endl;
        }

        else if (command.rfind("stopwatch", 0) == 0) {
            if(permission>=1) {
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
                        if(lapClear) {
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
            else cout<<"No Access."<<endl;
        }

        else if (command.rfind("dir", 0) == 0) {
            if(permission>=2) {
                string path = command.substr(3);
                size_t start = path.find_first_not_of(" \t");
                if (start != string::npos) path = path.substr(start);
                listDirectory(path);
            }
            else cout<<"No Access."<<endl;
        }

        else if (command.rfind("move", 0) == 0) {
            if (permission==2) {
                cout << "Error: 'move' requires Unlimited mode." << endl;
                continue;
            }

            else if(permission<2) cout<<"No Access."<<endl;

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
                cout << "Error: " << e.what() << endl;
            }

        }
        //else if(command=="file") cout<<"\"File\" is not a correct command."

        else if(command.empty()) continue;

        else if(command=="exit") {
            if(easterEggs) {
                random_device rd;
                mt19937 gen(rd());
                uniform_int_distribution<int> dist(0, 100);
                //int chance=0;
                int chance = dist(gen);
                if(chance >= 23 && chance <= 34) {
                    cout << "User is not the Impostor." << endl << "2 Impostors remaining." << endl;
                } else if(chance >= 36 && chance <= 45) {
                    cout << "User is one of the Impostors." << endl << "1 Impostor remaining." << endl;
                }
            }
            break;
        }

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
            if(fileOpreate) {
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

            if(fileOpreate)  return 0;  //Ran command.
            else if(exitWhenNoAccess) return 0;
            else goto loop_start;
        }

        else cout<<"\""<<command<<"\""<<" is not a correct command."<<endl;

        cout<<endl;
    }

    return 0;
}

//}

//Hill C++, 55 yuan for a ticket.

//Tunemah!!!!!

//Never gonna give you up,and never let you down.

//Games!Games!!We need Games!!!
