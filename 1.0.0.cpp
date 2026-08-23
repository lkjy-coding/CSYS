//#include <windows.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>
#include <deque>
#include <list>
#include <forward_list>
#include <array>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <queue>
#include <string>
#include <cstring>
#include <cctype>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <functional>
#include <cmath>
#include <cstdlib>
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

using namespace std;

string getTime(const string& format = "%Y%m%d%H%M%S") {
    auto now = chrono::system_clock::now();
    time_t now_time = chrono::system_clock::to_time_t(now);
    tm* local_time = localtime(&now_time);
    char buffer[100];
    strftime(buffer, sizeof(buffer), format.c_str(), local_time);
    return string(buffer);
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

int main(){
    cout<<"Please choose a mode."<<endl<<"Type in \"U\" to use unlimited mode,and type in \"L\" to use limited mode."<<endl;
//  cout<<"If you are using Windows device,it\'s recommend to use unlimited mode.If not,it\'s recommend to use limited mode."<<endl;
    string mode;
//  cin >> mode;
//  cin.ignore();
    getline(cin,mode);
    bool limited=false;
    if(mode=="L") limited=true;
    else if(mode=="U") limited=false;
    else{
    cout<<"Unable to get the result.Please check:"<<endl<<"1.Did you type in a wrong letter or extra letters?"<<endl<<"2.Did you type in \"u\" or \"l\"?Only CAPITAL LETTERS allowed."<<endl<<"Then restart and try again."<<endl;
    return 1;
    }
    
    //mode=0;    //Set back.
    string note="Nothing here yet.";
 // unsigned long long a=0,b=0,ans=0;
  //  double resultDivision=0.0;
  //  int countWay=0;
    string command="";
    unsigned long long typedInTimes=0;
    unsigned long long gotTimeCounter=0;
    unsigned long long deletedNoteTimes=0;
    unsigned long long typedLettersNumber=0;
    
    cout << "\033[2J\033[1;1H";
    cout<<"Type in a command to start."<<endl<<"If you need help,type in \"help\"."<<endl<<endl<<"Press Enter to continue."<<endl;
    
    int y=-1;
    
    cin.ignore();
    
    while(y<0){
    cout<<"CSYS/> ";
    
    getline(cin,command);
    cout<<endl;
    
    typedInTimes++;
    typedLettersNumber += command.length();
    
    if(command=="help"){
      cout << R"(
=== CSYS Commands ===

  time          - Show current time
  calc          - Calculator mode
  note          - Show current note
  setnote       - Set/update note
  delnote       - Delete note
  stats         - Show usage statistics
  clear         - Clear screen
  exit          - Exit
  help          - Get help on commands
  file          - Use and change files by commands,some functions will require Unlimited mode
  file read     - Use \"file read \[file name\]\" to show the content
  file change   - Use \"file change \[file name\]\" to change the content,requires Unlimited mode,and might be failed if no access 

Type any command and press Enter.
)" << endl;
     }
    else if (command == "time") {
    cout << getTime() << endl;
    gotTimeCounter++;
}
    else if (command == "note") {
    cout << "Note: " << note << endl;
}
    else if (command == "setnote") {
    cout << "Enter new note: ";
    //cin.ignore();
    getline(cin, note);
    cout << "Note updated." << endl;
}
    else if (command == "delnote") {
    note = "Nothing here yet.";
    deletedNoteTimes++;
    cout << "Note deleted." << endl;
}   

    else if (command.rfind("file read", 0) == 0) {
    string filename = command.substr(10);
    size_t start = filename.find_first_not_of(" \t");
    if (start != string::npos) filename = filename.substr(start);
    else { cout << "Error: No filename specified." << endl; continue; }
    
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
    else { cout << "Error: No filename specified." << endl; continue; }
    
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
    else if(command=="calc"){
        cout<<"First decide which operation to perform. 1,2,3,4,5,6 represent addition, subtraction, multiplication, division, exponentiation, and square root respectively."<<endl;
        int ans1;
        cin>>ans1;
        if(ans1==1){
            cout<<"Okay, now enter two numbers as the two addends."<<endl;
            unsigned long long aaa;
            unsigned long long bbb;
            cin>>aaa>>bbb;
            unsigned long long ans_plus;
            ans_plus=(aaa+bbb);
            cout<<"The answer is: "<<ans_plus<<endl;
        }
        else if(ans1==2){
            cout<<"Okay, now enter two numbers as the minuend and subtrahend."<<endl;
            unsigned long long aaa;
            unsigned long long bbb;
            cin>>aaa>>bbb;
            unsigned long long ans_m;
            ans_m=(aaa-bbb);
            cout<<"The answer is: "<<ans_m<<endl;
        }
        else if(ans1==3){
            cout<<"Okay, now enter two numbers as the two multipliers."<<endl;
            unsigned long long aaa;
            unsigned long long bbb;
            cin>>aaa>>bbb;
            unsigned long long ans_c;
            ans_c=(aaa*bbb);
            cout<<"The answer is: "<<ans_c<<endl;
        }
        else if(ans1==4){
            cout<<"Okay, now enter two numbers as the dividend and divisor."<<endl;
            double aaa;
            double bbb;
            cin>>aaa>>bbb;
            double ans_ch;
            ans_ch=(aaa/bbb*1.0);
            cout<<"The answer is: ";
            printf("%.10f\n",ans_ch);
        }
        else if(ans1==5){
            cout<<"Okay, now enter two numbers as the base and the exponent. (If the final output is 0, it means overflow)"<<endl;
            unsigned long long aaa;
            unsigned long long bbb;
            cin>>aaa>>bbb;
            unsigned long long ans_cf=1;
            if(bbb==0) ans_cf=1;
            else ans_cf=pow(aaa,bbb);
            cout<<"The answer is: "<<ans_cf<<endl;
        }
        else if(ans1==6){
            cout<<"Okay, now enter the first number."<<endl;
            double aaa;
            cin>>aaa;
            double ans_s;
            ans_s=sqrt(aaa);
            cout<<"The answer is: ";
            printf("%.10f\n",ans_s);
        }
        else{
            cout<<"This number doesn't seem to be supported. Try entering any integer between 1 and 6 again?"<<endl;
        }
        cin.ignore();  
    }
    
    else if (command == "clear") {
     cout << "\033[2J\033[1;1H";
    }

//  else if(command=="file") cout<<"\"File\" is not a correct command."
   
    else if(command.empty()) continue;
    else if(command=="exit") break;
    
    else cout<<"\""<<command<<"\""<<" is not a correct command."<<endl;
    }
    
    return 0;
}
