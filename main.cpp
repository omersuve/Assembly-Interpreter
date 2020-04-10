#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <sstream>
#include "cmath"
#include <algorithm>

using namespace std;

int hex2dec(string hex);
bool areDigit(string s);
void editStr(string &s);
void twoParameters(string inst, string first, string sec);
template  <typename regtype1, typename regtype2>  void xor_reg(regtype1 *first, regtype2 *sec)  ;
template  <typename regtype1, typename regtype2>  void shl_reg(regtype1 *first, regtype2 *sec)  ;
template  <typename regtype1, typename regtype2>  void shr_reg(regtype1 *first, regtype2 *sec)  ;
template  <typename regtype1, typename regtype2>  void rcl_reg(regtype1 *first, regtype2 *sec)  ;
template  <typename regtype1, typename regtype2>  void rcr_reg(regtype1 *first, regtype2 *sec)  ;
template  <typename regtype1, typename regtype2>  void and_reg(regtype1 *first, regtype2 *sec)  ;
template  <typename regtype1, typename regtype2>  void or_reg(regtype1 *first, regtype2 *sec)  ;
template  <typename regtype1, typename regtype2>  void cmp_reg(regtype1 *first, regtype2 *sec)  ;
template  <typename regtype1, typename regtype2>  void add_reg(regtype1 *first, regtype2 *sec)  ;
template  <typename regtype1, typename regtype2>  void sub_reg(regtype1 *first, regtype2 *sec)  ;
template  <typename regtype1, typename regtype2>  void mov_reg(regtype1 *first, regtype2 *sec)  ;
template <class datatype> void mul_reg(datatype y);
template <class datatype> void div_reg(datatype y);
template <class regtype> void xorFunc(regtype *first, string sec, bool isReg);
template <class regtype> void shlFunc(regtype *first, string sec, bool isReg);
template <class regtype> void shrFunc(regtype *first, string sec, bool isReg);
template <class regtype> void rclFunc(regtype *first, string sec, bool isReg);
template <class regtype> void rcrFunc(regtype *first, string sec, bool isReg);
template <class regtype> void andFunc(regtype *first, string sec, bool isReg);
template <class regtype> void orFunc(regtype *first, string sec, bool isReg);
template <class regtype> void cmpFunc(regtype *first, string sec, bool isReg);
template <class regtype> void addFunc(regtype *first, string sec, bool isReg);
template <class regtype> void subFunc(regtype *first, string sec, bool isReg);
template <class regtype> void movFunc(regtype *first, string sec, bool isReg);

// global variables ( memory, registers and flags )
unsigned char memory[2<<15];    // 64K memory
vector<string> lines;
map <string, int> vars;
map <string, int> labels;

unsigned short ax = 0 ;
unsigned short bx = 0 ;
unsigned short cx = 0 ;
unsigned short dx = 0 ;
unsigned short di = 0 ;
unsigned short bp = 0 ;
unsigned short si = 0 ;
unsigned short sp = (2<<15)-2 ;

bool     zf       ;              // zero flag
bool     sf       ;              // sign flag
bool     af       ;              // auxillary flag
bool     of       ;              // overflow flag
bool     cf       ;              // carry flag

bool isError = false;

// initialize pointers
unsigned short *pax = &ax ;
unsigned short *pbx = &bx ;
unsigned short *pcx = &cx ;
unsigned short *pdx = &dx ;
unsigned short *pdi = &di ;
unsigned short *psp = &sp ;
unsigned short *psi = &si ;
unsigned short *pbp = &bp ;

// note that x86 uses little endian, that is, least significat byte is stored in lowest byte
unsigned char *pah = (unsigned char *) ( ( (unsigned char *) &ax) + 1) ;
unsigned char *pal = (unsigned char *) &ax  ;
unsigned char *pbh = (unsigned char *) ( ( (unsigned char *) &bx) + 1) ;
unsigned char *pbl = (unsigned char *) &bx  ;
unsigned char *pch = (unsigned char *) ( ( (unsigned char *) &cx) + 1) ;
unsigned char *pcl = (unsigned char *) &cx  ;
unsigned char *pdh = (unsigned char *) ( ( (unsigned char *) &dx) + 1) ;
unsigned char *pdl = (unsigned char *) &dx  ;

map <string, unsigned short*> regw;
map <string, unsigned char*> regb;

int main(int argc, char* argv[]) {

    regw.insert(pair<string, unsigned short *>("ax", pax));
    regw.insert(pair<string, unsigned short *>("bx", pbx));
    regw.insert(pair<string, unsigned short *>("cx", pcx));
    regw.insert(pair<string, unsigned short *>("dx", pdx));
    regw.insert(pair<string, unsigned short *>("di", pdi));
    regw.insert(pair<string, unsigned short *>("sp", psp));
    regw.insert(pair<string, unsigned short *>("si", psi));
    regw.insert(pair<string, unsigned short *>("bp", pbp));
    regb.insert(pair<string, unsigned char *>("ah", pah));
    regb.insert(pair<string, unsigned char *>("al", pal));
    regb.insert(pair<string, unsigned char *>("bh", pbh));
    regb.insert(pair<string, unsigned char *>("bl", pbl));
    regb.insert(pair<string, unsigned char *>("ch", pch));
    regb.insert(pair<string, unsigned char *>("cl", pcl));
    regb.insert(pair<string, unsigned char *>("dh", pdh));
    regb.insert(pair<string, unsigned char *>("dl", pdl));

/*
    *pbx = 289;
    cf = 0;

    shrFunc(pax, "1");

    const int SHIFT = 15;
    const unsigned MASK = 1 << SHIFT;

    for ( int i = 1; i <= SHIFT + 1; i++ )
    {
        cout << ( *pax & MASK ? '1' : '0' );
        *pax <<= 1;
        if ( i % 8 == 0 )
            cout << ' ';
    }

    cout << endl;
    cout << "cf:" << cf << endl;
*/

    vector<string> codelines;
    int memoryIdx = 0;
    ifstream infile(argv[1]);
    string line;
    while (getline(infile, line)) lines.push_back(line);
    bool cont = false;
    for (int i = 0; i < lines.size(); i++) {
        editStr(lines[i]);
        if (lines[i] == "code segment") {
            cont = true;
            continue;
        } else if (lines[i] == "code ends") cont = false;
        if (cont) codelines.push_back(lines[i]);
    }
    int i = 0;
    //todo yanlış instructionları tespit et
    while (codelines[i] != "int 20h") {
        stringstream check(codelines[i]);
        string instruction;
        getline(check, instruction, ' ');
        if (instruction == "mov") memoryIdx += 6;
        else if (instruction == "add") memoryIdx += 6;
        else if (instruction == "sub") memoryIdx += 6;
        else if (instruction == "dec") memoryIdx += 6;
        else if (instruction == "inc") memoryIdx += 6;
        else if (instruction == "mul") memoryIdx += 6;
        else if (instruction == "div") memoryIdx += 6;
        else if (instruction == "xor") memoryIdx += 6;
        else if (instruction == "or") memoryIdx += 6;
        else if (instruction == "and") memoryIdx += 6;
        else if (instruction == "not") memoryIdx += 6;
        else if (instruction == "rcl") memoryIdx += 6;
        else if (instruction == "rcr") memoryIdx += 6;
        else if (instruction == "shl") memoryIdx += 6;
        else if (instruction == "shr") memoryIdx += 6;
        else if (instruction == "push") memoryIdx += 6;
        else if (instruction == "pop") memoryIdx += 6;
        else if (instruction == "nop") memoryIdx += 6;
        else if (instruction == "cmp") memoryIdx += 6;
        else if (instruction == "jz") memoryIdx += 6;
        else if (instruction == "jnz") memoryIdx += 6;
        else if (instruction == "je") memoryIdx += 6;
        else if (instruction == "jne") memoryIdx += 6;
        else if (instruction == "ja") memoryIdx += 6;
        else if (instruction == "jae") memoryIdx += 6;
        else if (instruction == "jb") memoryIdx += 6;
        else if (instruction == "jbe") memoryIdx += 6;
        else if (instruction == "jnae") memoryIdx += 6;
        else if (instruction == "jnb") memoryIdx += 6;
        else if (instruction == "jnbe") memoryIdx += 6;
        else if (instruction == "jnc") memoryIdx += 6;
        else if (instruction == "jc") memoryIdx += 6;
        else if (codelines[i] == "int 21h") memoryIdx += 6;
        else {
            if (codelines[i].at(codelines[i].size() - 1) == ':') {
                string tmp = codelines[i].substr(0, codelines[i].size() - 1);
                labels.insert(pair<string, int>(tmp, i));
            } else {
                cout << "ERROR2" << endl;
                isError = true;
                return 0;
            }
        }
        i++;
    }
    memoryIdx += 6;
    //VARİABLE OKUMA
    for (int j = i + 1; j < codelines.size(); j++) {
        string var;
        string type;
        string info;
        stringstream check1(codelines[j]);
        getline(check1, var, ' ');
        getline(check1, type, ' ');
        getline(check1, info, ' ');
        if (type == "db") {
            unsigned char data;
            int tmp;
            if (info.at(0) == '\'') data = info.at(1);
            else if (info.at(info.size() - 1) == 'h' || info.at(0) == '0') {
                if(info.at(info.size() - 1) == 'h' || info.at(info.size() - 1) == 'd') info = info.substr(0, info.size() - 1);
                tmp = hex2dec(info);
                if (tmp > 255) {
                    cout << "ERROR3: false var" << endl;
                    isError = true;
                    return 0;
                }
                data = (unsigned char) tmp;
            } else if (info.at(info.size() - 1) == 'd') {
                info = info.substr(0, info.size() - 1);
                tmp = stoi(info);
                if (tmp > 255) {
                    cout << "ERROR3: false var" << endl;
                    isError = true;
                    return 0;
                }
                data = (unsigned char) tmp;
            } else if (areDigit(info)) {
                tmp = stoi(info);
                if (tmp > 255) {
                    cout << "ERROR3: false var" << endl;
                    isError = true;
                    return 0;
                }
                data = (unsigned char) tmp;
            } else {
                cout << "ERROR3: false var" << endl;
                isError = true;
                return 0;
            }
            vars.insert(pair<string, int>(var + "1", memoryIdx));
            memory[memoryIdx] = data;
            memoryIdx++;
        } else if (type == "dw") {
            unsigned char data1;
            unsigned char data2;
            int tmp;
            if (info.at(info.size() - 1) == 'h' || info.at(0) == '0') {
                if(info.at(info.size() - 1) == 'h' || info.at(info.size() - 1) == 'd') info = info.substr(0, info.size() - 1);
                tmp = hex2dec(info);
                if (tmp > 65535) {
                    cout << "ERROR3: false var" << endl;
                    isError = true;
                    return 0;
                }
                data1 = (unsigned char) (tmp / 256);
                data2 = (unsigned char) (tmp % 256);
            } else if (info.at(info.size() - 1) == 'd') {
                info = info.substr(0, info.size() - 1);
                tmp = stoi(info);
                if (tmp > 65535) {
                    cout << "ERROR3: false var" << endl;
                    isError = true;
                    return 0;
                }
                data1 = (unsigned char) (tmp / 256);
                data2 = (unsigned char) (tmp % 256);
            } else if (areDigit(info)) {
                tmp = stoi(info);
                if (tmp > 65535) {
                    cout << "ERROR3: false var" << endl;
                    isError = true;
                    return 0;
                }
                data1 = (unsigned char) (tmp / 256);
                data2 = (unsigned char) (tmp % 256);
            } else {
                cout << "ERROR3: false var" << endl;
                isError = true;
                return 0;
            }
            vars.insert(pair<string, int>(var + "2", memoryIdx));
            memory[memoryIdx + 1] = data1;
            memory[memoryIdx] = data2;
            memoryIdx += 2;
        } else {
            cout << "ERROR3" << endl;
            isError = true;
            break;
        }
    }

    //todo bazılarında 'a' durumunu incelemedin!!
    //ASIL KOD BURADAN BAŞLIYOR
    for (int i = 0; i < codelines.size(); i++) {
        if (isError) break;
        stringstream check1(codelines[i]);
        string instruction;
        string first;
        string sec;
        getline(check1, instruction, ' ');
        getline(check1, first, ' ');
        getline(check1, sec, ' ');
        if (instruction == "mov") {
            bool isOffset = false;
            if (sec == "offset") {
                getline(check1, sec, ' ');
                isOffset = true;
            }
            if (isOffset) {
                if (vars.count((sec + "1")) || vars.count((sec + "2"))) {
                    auto it = vars.find(sec + "1");
                    if (it == vars.end()) it = vars.find(sec + "2");
                    if (first.find('[') != string::npos) {
                        char desType = first.at(0);
                        if (desType == '[') first = first.substr(1, first.size() - 2);
                        else first = first.substr(2, first.size() - 3);
                        auto it1 = regw.find(first);
                        auto it2 = regb.find(first);
                        if (it1 == regw.end()) {
                            if (it2 == regb.end()) {
                                if (first.at(first.size() - 1) == 'h' || first.at(0) == '0') {
                                    if(first.at(first.size() - 1) == 'h' || first.at(first.size() - 1) == 'd') first = first.substr(0, first.size() - 1);
                                    if (hex2dec(first) > 65535) {
                                        cout << "ERROR25" << endl;
                                        isError = true;
                                        return 0;
                                    }
                                    memory[hex2dec(first)] = it->second;
                                } else {
                                    if (first.at(first.size() - 1) == 'd') first = first.substr(0, first.size() - 1);
                                    if (stoi(first) > 65535) {
                                        cout << "ERROR25" << endl;
                                        isError = true;
                                        return 0;
                                    }
                                    memory[stoi(first)] = it->second;
                                }
                            } else memory[*it2->second] = it->second;
                        } else memory[*it1->second] = it->second;
                    } else {
                        if (vars.count((first + "1")) || vars.count((first + "2"))) {
                            auto it0 = vars.find(first + "1");
                            if (it0 == vars.end()) it0 = vars.find(first + "2");
                            memory[it0->second] = it->second;
                        } else {
                            auto it1 = regw.find(first);
                            auto it2 = regb.find(first);
                            if (it1 == regw.end()) {
                                if (it2 == regb.end()) {
                                    cout << "ERROR25" << endl;
                                    isError = true;
                                    return 0;
                                } else *it2->second = it->second;
                            } else *it1->second = it->second;
                        }
                    }
                } else {
                    cout << "Key-value pair not present in map" << endl;
                }
            } else {
                twoParameters("mov", first, sec);
            }
        } else if (instruction == "add") {
            twoParameters("add", first, sec);
        } else if (instruction == "sub") {
            twoParameters("sub", first, sec);
        } else if (instruction == "inc") {
            if (first.find('[') != string::npos) {
                first = first.substr(1, first.size() - 2);
                auto it1 = regw.find(first);
                auto it2 = regb.find(first);
                if (it1 == regw.end()) {
                    if (it2 == regb.end()) {
                        if (first.at(first.size() - 1) == 'h' || first.at(0) == '0') {
                            if(first.at(first.size() - 1) == 'h' || first.at(first.size() - 1) == 'd') first = first.substr(0, first.size() - 1);
                            if (hex2dec(first) > 65535) {
                                cout << "ERROR25" << endl;
                                isError = true;
                                return 0;
                            }
                            memory[hex2dec(first)]++;
                        } else {
                            if (first.at(first.size() - 1) == 'd') first = first.substr(0, first.size() - 1);
                            if (stoi(first) > 65535) {
                                cout << "ERROR25" << endl;
                                isError = true;
                                return 0;
                            }
                            memory[stoi(first)]++;
                        }
                    } else memory[*it2->second]++;
                } else memory[*it1->second]++;
            } else {
                auto it1 = regw.find(first);
                auto it2 = regb.find(first);
                if (it1 == regw.end()) (*it2->second)++;
                else (*it1->second)++;
            }
        } else if (instruction == "dec") {
            if (first.find('[') != string::npos) {
                first = first.substr(1, first.size() - 2);
                auto it1 = regw.find(first);
                auto it2 = regb.find(first);
                if (it1 == regw.end()) {
                    if (it2 == regb.end()) {
                        if (first.at(first.size() - 1) == 'h' || first.at(0) == '0') {
                            if(first.at(first.size() - 1) == 'h' || first.at(first.size() - 1) == 'd') first = first.substr(0, first.size() - 1);
                            if (hex2dec(first) > 65535) {
                                cout << "ERROR25" << endl;
                                isError = true;
                                return 0;
                            }
                            memory[hex2dec(first)]--;
                        } else {
                            if (first.at(first.size() - 1) == 'd') first = first.substr(0, first.size() - 1);
                            if (stoi(first) > 65535) {
                                cout << "ERROR25" << endl;
                                isError = true;
                                return 0;
                            }
                            memory[stoi(first)]--;
                        }
                    } else memory[*it2->second]--;
                } else memory[*it1->second]--;
            } else {
                auto it1 = regw.find(first);
                auto it2 = regb.find(first);
                if (it1 == regw.end()) (*it2->second)--;
                else (*it1->second)--;
            }
        } else if (instruction == "mul") {
            if (first == "ax") {
                mul_reg(*pax);
            } else if (first == "bx") {
                mul_reg(*pbx);
            } else if (first == "cx") {
                mul_reg(*pcx);
            } else if (first == "dx") {
                mul_reg(*pdx);
            } else if (first == "di") {
                mul_reg(*pdi);
            } else if (first == "sp") {
                mul_reg(*psp);
            } else if (first == "si") {
                mul_reg(*psi);
            } else if (first == "bp") {
                mul_reg(*pbp);
            } else if (first == "ah") {
                mul_reg(*pah);
            } else if (first == "al") {
                mul_reg(*pal);
            } else if (first == "bh") {
                mul_reg(*pbh);
            } else if (first == "bl") {
                mul_reg(*pbl);
            } else if (first == "ch") {
                mul_reg(*pch);
            } else if (first == "cl") {
                mul_reg(*pcl);
            } else if (first == "dh") {
                mul_reg(*pdh);
            } else if (first == "dl") {
                mul_reg(*pdl);
            } else {
                if (first.at(0) == '[') first = first.substr(1, first.size() - 2);
                else {
                    first = first.substr(2, first.size() - 3);
                }
                if (first.at(first.size() - 1) == 'h' || first.at(0) == '0') {
                    if(first.at(first.size() - 1) == 'h' || first.at(first.size() - 1) == 'd') first = first.substr(0, first.size() - 1);
                    if ((int) hex2dec(first) > 65535) {
                        cout << "ERROR4" << endl;
                        isError = true;
                        return 0;
                    }
                    unsigned char *pnum = &memory[hex2dec(first)];
                    mul_reg(*pnum);
                } else {
                    if (first.at(first.size() - 1) == 'd') {
                        first = first.substr(0, first.size() - 1);
                    }
                    if (stoi(first) > 65535) {
                        cout << "ERROR5" << endl;
                        isError = true;
                        return 0;
                    }
                    unsigned char *pnum = &memory[stoi(first)];
                    mul_reg(*pnum);
                }
            }
        } else if (instruction == "div") {
            if (first == "ax") {
                div_reg(*pax);
            } else if (first == "bx") {
                div_reg(*pbx);
            } else if (first == "cx") {
                div_reg(*pcx);
            } else if (first == "dx") {
                div_reg(*pdx);
            } else if (first == "di") {
                div_reg(*pdi);
            } else if (first == "sp") {
                div_reg(*psp);
            } else if (first == "si") {
                div_reg(*psi);
            } else if (first == "bp") {
                div_reg(*pbp);
            } else if (first == "ah") {
                div_reg(*pah);
            } else if (first == "al") {
                div_reg(*pal);
            } else if (first == "bh") {
                div_reg(*pbh);
            } else if (first == "bl") {
                div_reg(*pbl);
            } else if (first == "ch") {
                div_reg(*pch);
            } else if (first == "cl") {
                div_reg(*pcl);
            } else if (first == "dh") {
                div_reg(*pdh);
            } else if (first == "dl") {
                div_reg(*pdl);
            } else {
                if (first.at(0) == '[') first = first.substr(1, first.size() - 2);
                else {
                    first = first.substr(2, first.size() - 3);
                }
                if (first.at(first.size() - 1) == 'h' || first.at(0) == '0') {
                    if(first.at(first.size() - 1) == 'h' || first.at(first.size() - 1) == 'd') first = first.substr(0, first.size() - 1);
                    if ((int) hex2dec(first) > 65535) {
                        cout << "ERROR6" << endl;
                        isError = true;
                        return 0;
                    }
                    unsigned char *pnum = &memory[hex2dec(first)];
                    div_reg(*pnum);
                } else {
                    if (first.at(first.size() - 1) == 'd') {
                        first = first.substr(0, first.size() - 1);
                    }
                    if (stoi(first) > 65535) {
                        cout << "ERROR7" << endl;
                        isError = true;
                        return 0;
                    }
                    unsigned char *pnum = &memory[stoi(first)];
                    div_reg(*pnum);
                }
            }
            if (isError) return 0;
        } else if (instruction == "xor") {
            twoParameters("xor", first, sec);
        } else if (instruction == "or") {
            twoParameters("or", first, sec);
        } else if (instruction == "and") {
            twoParameters("and", first, sec);
        } else if (instruction == "not") {
            if (first.find('[') != string::npos) {
                first = first.substr(1, first.size() - 2);
                auto it1 = regw.find(first);
                auto it2 = regb.find(first);
                if (it1 == regw.end()) {
                    if (it2 == regb.end()) {
                        if (first.at(first.size() - 1) == 'h' || first.at(0) == '0') {
                            if(first.at(first.size() - 1) == 'h' || first.at(first.size() - 1) == 'd') first = first.substr(0, first.size() - 1);
                            if (hex2dec(first) > 65535) {
                                cout << "ERROR25" << endl;
                                isError = true;
                                return 0;
                            }
                            memory[hex2dec(first)] = ~memory[hex2dec(first)];
                        } else if (first.at(first.size() - 1) == 'd' || areDigit(first)) {
                            if (first.at(first.size() - 1) == 'd') first = first.substr(0, first.size() - 1);
                            if (stoi(first) > 65535) {
                                cout << "ERROR25" << endl;
                                isError = true;
                                return 0;
                            }
                            memory[stoi(first)] = ~memory[stoi(first)];
                        } else {
                            cout << "ERROR25" << endl;
                            isError = true;
                            return 0;
                        }
                    } else memory[*it2->second] = ~memory[*it2->second];
                } else memory[*it1->second] = ~memory[*it1->second];
            } else {
                auto it1 = regw.find(first);
                auto it2 = regb.find(first);
                if (it1 == regw.end()) *it2->second = ~*it2->second;
                else *it1->second = ~*it1->second;
            }
        } else if (instruction == "rcl") {
            twoParameters("rcl", first, sec);
        } else if (instruction == "rcr") {
            twoParameters("rcr", first, sec);
        } else if (instruction == "shl") {
            twoParameters("shl", first, sec);
        } else if (instruction == "shr") {
            twoParameters("shr", first, sec);
        } else if (instruction == "push") {
            if(first == "ax"){
                memory[sp] = ax;
                sp -= 2;
            } else if(first == "bx"){
                memory[sp] = bx;
                sp -= 2;
            } else if(first == "cx"){
                memory[sp] = cx;
                sp -= 2;
            } else if(first == "dx"){
                memory[sp] = dx;
                sp -= 2;
            } else if(first == "di"){
                memory[sp] = di;
                sp -= 2;
            } else if(first == "sp"){
                memory[sp] = sp;
                sp -= 2;
            } else if(first == "si"){
                memory[sp] = si;
                sp -= 2;
            } else if(first == "bp"){
                memory[sp] = bp;
                sp -= 2;
            } else {
                cout << "ERROR 93" << endl;
                isError = true;
                return 0;
            }
        } else if (instruction == "pop") {
            if(first == "ax"){
                sp += 2;
                *pax = memory[sp];
            } else if(first == "bx"){
                sp += 2;
                *pbx = memory[sp];
            } else if(first == "cx"){
                sp += 2;
                *pcx = memory[sp];
            } else if(first == "dx"){
                sp += 2;
                *pdx = memory[sp];
            } else if(first == "di"){
                sp += 2;
                *pdi = memory[sp];
            } else if(first == "sp"){
                sp += 2;
                *psp = memory[sp];
            } else if(first == "si"){
                sp += 2;
                *psi = memory[sp];
            } else if(first == "bp"){
                sp += 2;
                *pbp = memory[sp];
            } else {
                cout << "ERROR 94" << endl;
                isError = true;
                return 0;
            }
        } else if (instruction == "nop");
        else if (instruction == "cmp") {
            twoParameters("cmp", first, sec);
        } else if (instruction == "jnae") {
            auto it = labels.find(first);
            if (it == labels.end()) {
                cout << "ERROR26: no such label" << endl;
                isError = true;
                return 0;
            }
            if (cf == 1) {
                i = it->second;
                continue;
            }
        } else if (instruction == "jnbe") {
            auto it = labels.find(first);
            if (it == labels.end()) {
                cout << "ERROR26: no such label" << endl;
                isError = true;
                return 0;
            }
            if (cf == 0 && zf == 0) {
                i = it->second;
                continue;
            }
        } else if (instruction == "jae") {
            auto it = labels.find(first);
            if (it == labels.end()) {
                cout << "ERROR26: no such label" << endl;
                isError = true;
                return 0;
            }
            if (cf == 0) {
                i = it->second;
                continue;
            }
        } else if (instruction == "jbe") {
            auto it = labels.find(first);
            if (it == labels.end()) {
                cout << "ERROR26: no such label" << endl;
                isError = true;
                return 0;
            }
            if (cf == 1 || zf == 1) {
                i = it->second;
                continue;
            }
        } else if (instruction == "jnb") {
            auto it = labels.find(first);
            if (it == labels.end()) {
                cout << "ERROR26: no such label" << endl;
                isError = true;
                return 0;
            }
            if (cf == 0) {
                i = it->second;
                continue;
            }
        } else if (instruction == "jnc") {
            auto it = labels.find(first);
            if (it == labels.end()) {
                cout << "ERROR26: no such label" << endl;
                isError = true;
                return 0;
            }
            if (cf == 0) {
                i = it->second;
                continue;
            }
        } else if (instruction == "jne") {
            auto it = labels.find(first);
            if (it == labels.end()) {
                cout << "ERROR26: no such label" << endl;
                isError = true;
                return 0;
            }
            if (zf == 0) {
                i = it->second;
                continue;
            }
        } else if (instruction == "jnz") {
            auto it = labels.find(first);
            if (it == labels.end()) {
                cout << "ERROR26: no such label" << endl;
                isError = true;
                return 0;
            }
            if (zf == 0) {
                i = it->second;
                continue;
            }
        } else if (instruction == "ja") {
            auto it = labels.find(first);
            if (it == labels.end()) {
                cout << "ERROR26: no such label" << endl;
                isError = true;
                return 0;
            }
            if (cf == 0 && zf == 0) {
                i = it->second;
                continue;
            }
        } else if (instruction == "jb") {
            auto it = labels.find(first);
            if (it == labels.end()) {
                cout << "ERROR26: no such label" << endl;
                isError = true;
                return 0;
            }
            if (cf == 1) {
                i = it->second;
                continue;
            }
        } else if (instruction == "jc") {
            auto it = labels.find(first);
            if (it == labels.end()) {
                cout << "ERROR26: no such label" << endl;
                isError = true;
                return 0;
            }
            if (cf == 1) {
                i = it->second;
                continue;
            }
        } else if (instruction == "je") {
            auto it = labels.find(first);
            if (it == labels.end()) {
                cout << "ERROR26: no such label" << endl;
                isError = true;
                return 0;
            }
            if (zf == 1) {
                i = it->second;
                continue;
            }
        } else if (instruction == "jz") {
            auto it = labels.find(first);
            if (it == labels.end()) {
                cout << "ERROR26: no such label" << endl;
                isError = true;
                return 0;
            }
            if (zf == 1) {
                i = it->second;
                continue;
            }
        } else if (codelines[i] == "int 21h") {
            if (*pah == 1) {
                unsigned char tmp;
                cin >> tmp;
                *pal = tmp;
            } else if (*pah == 2) cout << (*pdl);
            else {
                cout << "ERROR27: undefined operation" << endl;
                isError = true;
                return 0;
            }
        }
    }
}

void twoParameters(string inst, string first, string sec) {
    if(first.find('[') != string::npos) {
        if(first.at(0) != '[') first = first.substr(2, first.size()-3);
        else first = first.substr(1, first.size()-2);
        auto it1 = regw.find(first);
        auto it2 = regb.find(first);
        if(it1 == regw.end()) {
            if(it2 == regb.end()) {
                if (first.at(first.size() - 1) == 'h' || first.at(0) == '0') {
                    if(first.at(first.size() - 1) == 'h' || first.at(first.size() - 1) == 'd') first = first.substr(0, first.size() - 1);
                    if(hex2dec(first) > 65535) {
                        cout << "ERROR25" << endl;
                        isError = true;
                        return;
                    }
                    unsigned char *ptmp = &memory[hex2dec(first)];
                    if(inst == "xor") xorFunc(ptmp, sec, false);
                    else if(inst == "and") andFunc(ptmp, sec, false);
                    else if(inst == "or") orFunc(ptmp, sec, false);
                    else if(inst == "cmp") cmpFunc(ptmp, sec, false);
                    else if(inst == "add") addFunc(ptmp, sec, false);
                    else if(inst == "sub") subFunc(ptmp, sec, false);
                    else if(inst == "mov") movFunc(ptmp, sec, false);
                    else if(inst == "shl") shlFunc(ptmp, sec, false);
                    else if(inst == "shr") shrFunc(ptmp, sec, false);
                    else if(inst == "rcl") rclFunc(ptmp, sec, false);
                    else if(inst == "rcr") rcrFunc(ptmp, sec, false);
                }
                else if(first.at(first.size()-1) == 'd' || areDigit(first)) {
                    if(first.at(first.size()-1) == 'd') first = first.substr(0, first.size()-1);
                    if(stoi(first) > 65535) {
                        cout << "ERROR25" << endl;
                        isError = true;
                        return;
                    }
                    unsigned char *ptmp = &memory[stoi(first)];
                    if(inst == "xor") xorFunc(ptmp, sec, false);
                    else if(inst == "and") andFunc(ptmp, sec, false);
                    else if(inst == "or") orFunc(ptmp, sec, false);
                    else if(inst == "cmp") cmpFunc(ptmp, sec, false);
                    else if(inst == "add") addFunc(ptmp, sec, false);
                    else if(inst == "sub") subFunc(ptmp, sec, false);
                    else if(inst == "mov") movFunc(ptmp, sec, false);
                    else if(inst == "shl") shlFunc(ptmp, sec, false);
                    else if(inst == "shr") shrFunc(ptmp, sec, false);
                    else if(inst == "rcl") rclFunc(ptmp, sec, false);
                    else if(inst == "rcr") rcrFunc(ptmp, sec, false);
                }
                else {
                    cout << "ERROR25" << endl;
                    isError = true;
                    return;
                }
            }
            else {
                if(inst == "xor") xorFunc(it2->second, sec, true);
                else if(inst == "and") andFunc(it2->second, sec, true);
                else if(inst == "or") orFunc(it2->second, sec, true);
                else if(inst == "cmp") cmpFunc(it2->second, sec, true);
                else if(inst == "add") addFunc(it2->second, sec, true);
                else if(inst == "sub") subFunc(it2->second, sec, true);
                else if(inst == "mov") movFunc(it2->second, sec, true);
                else if(inst == "shl") shlFunc(it2->second, sec, true);
                else if(inst == "shr") shrFunc(it2->second, sec, true);
                else if(inst == "rcl") rclFunc(it2->second, sec, true);
                else if(inst == "rcr") rcrFunc(it2->second, sec, true);
            }
        }
        else {
            if(inst == "xor") xorFunc(it1->second, sec, true);
            else if(inst == "and") andFunc(it1->second, sec, true);
            else if(inst == "or") orFunc(it1->second, sec, true);
            else if(inst == "cmp") cmpFunc(it1->second, sec, true);
            else if(inst == "add") addFunc(it1->second, sec, true);
            else if(inst == "sub") subFunc(it1->second, sec, true);
            else if(inst == "mov") movFunc(it1->second, sec, true);
            else if(inst == "shl") shlFunc(it1->second, sec, true);
            else if(inst == "shr") shrFunc(it1->second, sec, true);
            else if(inst == "rcl") rclFunc(it1->second, sec, true);
            else if(inst == "rcr") rcrFunc(it1->second, sec, true);
        }
    }
    else if(sec.find('[') != string::npos) {
        unsigned char type = 0;
        if (sec.at(0) != '[') {
            type = sec.at(0);
            sec = sec.substr(2, sec.size() - 3);
        } else sec = sec.substr(1, sec.size() - 2);
        auto it1 = regw.find(first);
        auto it2 = regb.find(first);
        if (it1 == regw.end()) {
            if (it2 == regb.end()) {
                cout << "ERROR25" << endl;
                isError = true;
                return;
            }
            if (type == 'w') {
                cout << "ERROR25" << endl;
                isError = true;
                return;
            }
            if (sec.at(sec.size() - 1) == 'h' || sec.at(0) == '0') {
                if(sec.at(sec.size() - 1) == 'h' || sec.at(sec.size() - 1) == 'd') sec = sec.substr(0, sec.size() - 1);
                if (hex2dec(sec) > 65535) {
                    cout << "ERROR25" << endl;
                    isError = true;
                    return;
                }
                unsigned char *ptmp = &memory[hex2dec(sec)];
                if (inst == "xor") xor_reg(it2->second, ptmp);
                else if (inst == "and") and_reg(it2->second, ptmp);
                else if (inst == "or") or_reg(it2->second, ptmp);
                else if (inst == "cmp") cmp_reg(it2->second, ptmp);
                else if (inst == "add") add_reg(it2->second, ptmp);
                else if (inst == "sub") sub_reg(it2->second, ptmp);
                else if (inst == "mov") mov_reg(it2->second, ptmp);
            }
            else if (sec.at(0) == '\'') {
                unsigned char tmp = sec.at(1);
                unsigned char *ptmp = &tmp;
                if (inst == "xor") xor_reg(it2->second, ptmp);
                else if (inst == "and") and_reg(it2->second, ptmp);
                else if (inst == "or") or_reg(it2->second, ptmp);
                else if (inst == "cmp") cmp_reg(it2->second, ptmp);
                else if (inst == "add") add_reg(it2->second, ptmp);
                else if (inst == "sub") sub_reg(it2->second, ptmp);
                else if (inst == "mov") mov_reg(it2->second, ptmp);
            }
            else if (sec.at(sec.size() - 1) == 'd' || areDigit(sec)) {
                if (sec.at(sec.size() - 1) == 'd') sec = sec.substr(0, sec.size() - 1);
                if (stoi(sec) > 65535) {
                    cout << "ERROR25" << endl;
                    isError = true;
                    return;
                }
                unsigned char *ptmp = &memory[stoi(sec)];
                if (inst == "xor") xor_reg(it2->second, ptmp);
                else if (inst == "and") and_reg(it2->second, ptmp);
                else if (inst == "or") or_reg(it2->second, ptmp);
                else if (inst == "cmp") cmp_reg(it2->second, ptmp);
                else if (inst == "add") add_reg(it2->second, ptmp);
                else if (inst == "sub") sub_reg(it2->second, ptmp);
                else if (inst == "mov") mov_reg(it2->second, ptmp);
            }
            else {
                auto it3 = regw.find(sec);
                auto it4 = regb.find(sec);
                if (it3 == regw.end()) {
                    if (it4 == regb.end()) {
                        cout << "ERROR25" << endl;
                        isError = true;
                        return;
                    }
                    unsigned char tmp = memory[*it4->second];
                    unsigned char *ptmp = &tmp;
                    if (inst == "xor") xor_reg(it2->second, ptmp);
                    else if (inst == "and") and_reg(it2->second, ptmp);
                    else if (inst == "or") or_reg(it2->second, ptmp);
                    else if (inst == "cmp") cmp_reg(it2->second, ptmp);
                    else if (inst == "add") add_reg(it2->second, ptmp);
                    else if (inst == "sub") sub_reg(it2->second, ptmp);
                    else if (inst == "mov") mov_reg(it2->second, ptmp);
                }
                else {
                    unsigned char tmp = memory[*it3->second];
                    unsigned char *ptmp = &tmp;
                    if (inst == "xor") xor_reg(it2->second, ptmp);
                    else if (inst == "and") and_reg(it2->second, ptmp);
                    else if (inst == "or") or_reg(it2->second, ptmp);
                    else if (inst == "cmp") cmp_reg(it2->second, ptmp);
                    else if (inst == "add") add_reg(it2->second, ptmp);
                    else if (inst == "sub") sub_reg(it2->second, ptmp);
                    else if (inst == "mov") mov_reg(it2->second, ptmp);
                }
            }
        }
        else {
            if (type == 'b') {
                cout << "ERROR25" << endl;
                isError = true;
                return;
            }
            if (sec.at(sec.size() - 1) == 'h' || sec.at(0) == '0') {
                if(sec.at(sec.size() - 1) == 'h' || sec.at(sec.size() - 1) == 'd') sec = sec.substr(0, sec.size() - 1);
                if (hex2dec(sec) > 65535) {
                    cout << "ERROR25" << endl;
                    isError = true;
                    return;
                }
                unsigned short tmp = memory[hex2dec(sec)+1]*256 + memory[hex2dec(sec)];
                unsigned short *ptmp = &tmp;
                if (inst == "xor") xor_reg(it1->second, ptmp);
                else if (inst == "and") and_reg(it1->second, ptmp);
                else if (inst == "or") or_reg(it1->second, ptmp);
                else if (inst == "cmp") cmp_reg(it1->second, ptmp);
                else if (inst == "add") add_reg(it1->second, ptmp);
                else if (inst == "sub") sub_reg(it1->second, ptmp);
                else if (inst == "mov") mov_reg(it1->second, ptmp);
            }
            else if (sec.at(0) == '\'') {
                unsigned char tmp = sec.at(1);
                unsigned char *ptmp = &tmp;
                if (inst == "xor") xor_reg(it1->second, ptmp);
                else if (inst == "and") and_reg(it1->second, ptmp);
                else if (inst == "or") or_reg(it1->second, ptmp);
                else if (inst == "cmp") cmp_reg(it1->second, ptmp);
                else if (inst == "add") add_reg(it1->second, ptmp);
                else if (inst == "sub") sub_reg(it1->second, ptmp);
                else if (inst == "mov") mov_reg(it1->second, ptmp);
            }
            else if (sec.at(sec.size() - 1) == 'd' || areDigit(sec)) {
                if (sec.at(sec.size() - 1) == 'd') sec = sec.substr(0, sec.size() - 1);
                if (stoi(sec) > 65535) {
                    cout << "ERROR25" << endl;
                    isError = true;
                    return;
                }
                unsigned short tmp = memory[stoi(sec)+1]*256 + memory[stoi(sec)];
                unsigned short *ptmp = &tmp;
                if (inst == "xor") xor_reg(it1->second, ptmp);
                else if (inst == "and") and_reg(it1->second, ptmp);
                else if (inst == "or") or_reg(it1->second, ptmp);
                else if (inst == "cmp") cmp_reg(it1->second, ptmp);
                else if (inst == "add") add_reg(it1->second, ptmp);
                else if (inst == "sub") sub_reg(it1->second, ptmp);
                else if (inst == "mov") mov_reg(it1->second, ptmp);
            }
            else {
                auto it3 = regw.find(sec);
                auto it4 = regb.find(sec);
                if (it3 == regw.end()) {
                    if (it4 == regb.end()) {
                        cout << "ERROR25" << endl;
                        isError = true;
                        return;
                    }
                    unsigned short tmp = memory[(*it4->second)+1]*256 + memory[*it4->second];
                    unsigned short *ptmp = &tmp;
                    if (inst == "xor") xor_reg(it1->second, ptmp);
                    else if (inst == "and") and_reg(it1->second, ptmp);
                    else if (inst == "or") or_reg(it1->second, ptmp);
                    else if (inst == "cmp") cmp_reg(it1->second, ptmp);
                    else if (inst == "add") add_reg(it1->second, ptmp);
                    else if (inst == "sub") sub_reg(it1->second, ptmp);
                    else if (inst == "mov") mov_reg(it1->second, ptmp);
                }
                else {
                    unsigned short tmp = memory[(*it3->second)+1]*256 + memory[*it3->second];
                    unsigned short *ptmp = &tmp;
                    if (inst == "xor") xor_reg(it1->second, ptmp);
                    else if (inst == "and") and_reg(it1->second, ptmp);
                    else if (inst == "or") or_reg(it1->second, ptmp);
                    else if (inst == "cmp") cmp_reg(it1->second, ptmp);
                    else if (inst == "add") add_reg(it1->second, ptmp);
                    else if (inst == "sub") sub_reg(it1->second, ptmp);
                    else if (inst == "mov") mov_reg(it1->second, ptmp);
                }
            }
        }
    }
    else {
        auto it1 = regw.find(first);
        auto it2 = regb.find(first);
        if(it1 == regw.end()) {
            if(it2 == regb.end()) {
                if (vars.count((first + "1")) || vars.count((first + "2"))) {
                    auto it = vars.find(first + "1");
                    if (it == vars.end()) {
                        it = vars.find(first + "2");
                        unsigned char *pmem1 = &memory[it->second+1];
                        unsigned char *pmem2 = &memory[it->second];
                        auto it3 = regw.find(sec);
                        auto it4 = regb.find(sec);
                        if(it4 != regb.end()) {
                            if(it4->second == pcl) {
                                //todo
                            }
                            cout << "ERROR25" << endl;
                            isError = true;
                            return;
                        }
                        if(it3 == regw.end()) {
                            if (sec.at(sec.size() - 1) == 'h' || sec.at(0) == '0') {
                                if(sec.at(sec.size() - 1) == 'h' || sec.at(sec.size() - 1) == 'd') sec = sec.substr(0, sec.size() - 1);
                                if(hex2dec(sec) > 65535) {
                                    cout << "ERROR25" << endl;
                                    isError = true;
                                    return;
                                }
                                unsigned char tmp1 = hex2dec(sec) / 256;
                                unsigned char tmp2 = hex2dec(sec) % 256;
                                unsigned char *ptmp1 = &tmp1;
                                unsigned char *ptmp2 = &tmp2;
                                if(inst == "xor") {
                                    xor_reg(pmem1, ptmp1);
                                    xor_reg(pmem2, ptmp2);
                                }
                                else if(inst == "and") {
                                    and_reg(pmem1, ptmp1);
                                    and_reg(pmem2, ptmp2);
                                }
                                else if(inst == "or") {
                                    or_reg(pmem1, ptmp1);
                                    or_reg(pmem2, ptmp2);
                                }
                                else if(inst == "cmp") {
                                    cmp_reg(pmem1, ptmp1);
                                    cmp_reg(pmem2, ptmp2);
                                }
                                else if(inst == "add") {
                                    add_reg(pmem1, ptmp1);
                                    add_reg(pmem2, ptmp2);
                                }
                                else if(inst == "sub") {
                                    sub_reg(pmem1, ptmp1);
                                    sub_reg(pmem2, ptmp2);
                                }
                                else if(inst == "mov") {
                                    mov_reg(pmem1, ptmp1);
                                    mov_reg(pmem2, ptmp2);
                                }
                                else if(inst == "shl") {
                                    shl_reg(pmem1, ptmp1);
                                    shl_reg(pmem2, ptmp2);
                                }
                                else if(inst == "shr") {
                                    shr_reg(pmem1, ptmp1);
                                    shr_reg(pmem2, ptmp2);
                                }
                                else if(inst == "rcl") {
                                    rcl_reg(pmem1, ptmp1);
                                    rcl_reg(pmem2, ptmp2);
                                }
                                else if(inst == "rcr") {
                                    rcr_reg(pmem1, ptmp1);
                                    rcr_reg(pmem2, ptmp2);
                                }
                                else {
                                    cout << "ERROR25" << endl;
                                    isError = true;
                                    return;
                                }
                            }
                            else if (sec.at(0) == '\'') {
                                unsigned char tmp1 = 0;
                                unsigned char tmp2 = sec.at(1);
                                unsigned char *ptmp1 = &tmp1;
                                unsigned char *ptmp2 = &tmp2;
                                if(inst == "xor") {
                                    xor_reg(pmem1, ptmp1);
                                    xor_reg(pmem2, ptmp2);
                                }
                                else if(inst == "and") {
                                    and_reg(pmem1, ptmp1);
                                    and_reg(pmem2, ptmp2);
                                }
                                else if(inst == "or") {
                                    or_reg(pmem1, ptmp1);
                                    or_reg(pmem2, ptmp2);
                                }
                                else if(inst == "cmp") {
                                    cmp_reg(pmem1, ptmp1);
                                    cmp_reg(pmem2, ptmp2);
                                }
                                else if(inst == "add") {
                                    add_reg(pmem1, ptmp1);
                                    add_reg(pmem2, ptmp2);
                                }
                                else if(inst == "sub") {
                                    sub_reg(pmem1, ptmp1);
                                    sub_reg(pmem2, ptmp2);
                                }
                                else if(inst == "mov") {
                                    mov_reg(pmem1, ptmp1);
                                    mov_reg(pmem2, ptmp2);
                                }
                                else {
                                    cout << "ERROR25" << endl;
                                    isError = true;
                                    return;
                                }
                            }
                            else if(sec.at(sec.size()-1) == 'd'|| areDigit(sec)) {
                                if(sec.at(sec.size()-1) == 'd') sec = sec.substr(0,sec.size()-1);
                                unsigned char tmp1 = stoi(sec) / 256;
                                unsigned char tmp2 = stoi(sec) % 256;
                                unsigned char *ptmp1 = &tmp1;
                                unsigned char *ptmp2 = &tmp2;
                                if(inst == "xor") {
                                    xor_reg(pmem1, ptmp1);
                                    xor_reg(pmem2, ptmp2);
                                }
                                else if(inst == "and") {
                                    and_reg(pmem1, ptmp1);
                                    and_reg(pmem2, ptmp2);
                                }
                                else if(inst == "or") {
                                    or_reg(pmem1, ptmp1);
                                    or_reg(pmem2, ptmp2);
                                }
                                else if(inst == "cmp") {
                                    cmp_reg(pmem1, ptmp1);
                                    cmp_reg(pmem2, ptmp2);
                                }
                                else if(inst == "add") {
                                    add_reg(pmem1, ptmp1);
                                    add_reg(pmem2, ptmp2);
                                }
                                else if(inst == "sub") {
                                    sub_reg(pmem1, ptmp1);
                                    sub_reg(pmem2, ptmp2);
                                }
                                else if(inst == "mov") {
                                    mov_reg(pmem1, ptmp1);
                                    mov_reg(pmem2, ptmp2);
                                }
                                else if(inst == "shl") {
                                    shl_reg(pmem1, ptmp1);
                                    shl_reg(pmem2, ptmp2);
                                }
                                else if(inst == "shr") {
                                    shr_reg(pmem1, ptmp1);
                                    shr_reg(pmem2, ptmp2);
                                }
                                else if(inst == "rcl") {
                                    rcl_reg(pmem1, ptmp1);
                                    rcl_reg(pmem2, ptmp2);
                                }
                                else if(inst == "rcr") {
                                    rcr_reg(pmem1, ptmp1);
                                    rcr_reg(pmem2, ptmp2);
                                }
                                else {
                                    cout << "ERROR25" << endl;
                                    isError = true;
                                    return;
                                }
                            }
                            else {
                                cout << "ERROR25" << endl;
                                isError = true;
                                return;
                            }
                        }
                        else {
                            unsigned char tmp1 = (*it3->second) / 256;
                            unsigned char tmp2 = (*it3->second) % 256;
                            unsigned char *ptmp1 = &tmp1;
                            unsigned char *ptmp2 = &tmp2;
                            if(inst == "xor") {
                                xor_reg(pmem1, ptmp1);
                                xor_reg(pmem2, ptmp2);
                            }
                            else if(inst == "and") {
                                and_reg(pmem1, ptmp1);
                                and_reg(pmem2, ptmp2);
                            }
                            else if(inst == "or") {
                                or_reg(pmem1, ptmp1);
                                or_reg(pmem2, ptmp2);
                            }
                            else if(inst == "cmp") {
                                cmp_reg(pmem1, ptmp1);
                                cmp_reg(pmem2, ptmp2);
                            }
                            else if(inst == "add") {
                                add_reg(pmem1, ptmp1);
                                add_reg(pmem2, ptmp2);
                            }
                            else if(inst == "sub") {
                                sub_reg(pmem1, ptmp1);
                                sub_reg(pmem2, ptmp2);
                            }
                            else if(inst == "mov") {
                                mov_reg(pmem1, ptmp1);
                                mov_reg(pmem2, ptmp2);
                            }
                            else {
                                cout << "ERROR87" << endl;
                                isError = true;
                                return;
                            }
                        }
                    }
                    else {
                        unsigned char *pmem = &memory[it->second];
                        auto it3 = regw.find(sec);
                        auto it4 = regb.find(sec);
                        if(it3 != regw.end()) {
                            cout << "ERROR25" << endl;
                            isError = true;
                            return;
                        }
                        if(it4 == regb.end()) {
                            if (sec.at(sec.size() - 1) == 'h' || sec.at(0) == '0') {
                                if(sec.at(sec.size() - 1) == 'h' || sec.at(sec.size() - 1) == 'd') sec = sec.substr(0, sec.size() - 1);
                                if(hex2dec(sec) > 255) {
                                    cout << "ERROR25" << endl;
                                    isError = true;
                                    return;
                                }
                                unsigned char tmp = hex2dec(sec);
                                unsigned char *ptmp = &tmp;
                                if(inst == "xor") xor_reg(pmem, ptmp);
                                else if(inst == "and") and_reg(pmem, ptmp);
                                else if(inst == "or") or_reg(pmem, ptmp);
                                else if(inst == "cmp") cmp_reg(pmem, ptmp);
                                else if(inst == "add") add_reg(pmem, ptmp);
                                else if(inst == "sub") sub_reg(pmem, ptmp);
                                else if(inst == "mov") mov_reg(pmem, ptmp);
                                else if(inst == "shl") shl_reg(pmem, ptmp);
                                else if(inst == "shr") shr_reg(pmem, ptmp);
                                else if(inst == "rcl") rcl_reg(pmem, ptmp);
                                else if(inst == "rcr") rcr_reg(pmem, ptmp);
                            }
                            else if (sec.at(0) == '\'') {
                                unsigned char tmp = sec.at(1);
                                unsigned char *ptmp = &tmp;
                                if(inst == "xor") xor_reg(pmem, ptmp);
                                else if(inst == "and") and_reg(pmem, ptmp);
                                else if(inst == "or") or_reg(pmem, ptmp);
                                else if(inst == "cmp") cmp_reg(pmem, ptmp);
                                else if(inst == "add") add_reg(pmem, ptmp);
                                else if(inst == "sub") sub_reg(pmem, ptmp);
                                else if(inst == "mov") mov_reg(pmem, ptmp);
                            }
                            else if(sec.at(sec.size()-1) == 'd'|| areDigit(sec)) {
                                if(sec.at(sec.size()-1) == 'd') sec = sec.substr(0,sec.size()-1);
                                unsigned char tmp = stoi(sec);
                                unsigned char *ptmp = &tmp;
                                if(inst == "xor") xor_reg(pmem, ptmp);
                                else if(inst == "and") and_reg(pmem, ptmp);
                                else if(inst == "or") or_reg(pmem, ptmp);
                                else if(inst == "cmp") cmp_reg(pmem, ptmp);
                                else if(inst == "add") add_reg(pmem, ptmp);
                                else if(inst == "sub") sub_reg(pmem, ptmp);
                                else if(inst == "mov") mov_reg(pmem, ptmp);
                                else if(inst == "shl") shl_reg(pmem, ptmp);
                                else if(inst == "shr") shr_reg(pmem, ptmp);
                                else if(inst == "rcl") rcl_reg(pmem, ptmp);
                                else if(inst == "rcr") rcr_reg(pmem, ptmp);
                            }
                            else {
                                cout << "ERROR25" << endl;
                                isError = true;
                                return;
                            }
                        }
                        else {
                            unsigned char tmp = *it4->second;
                            unsigned char *ptmp = &tmp;
                            if(inst == "xor") xor_reg(pmem, ptmp);
                            else if(inst == "and") and_reg(pmem, ptmp);
                            else if(inst == "or") or_reg(pmem, ptmp);
                            else if(inst == "cmp") cmp_reg(pmem, ptmp);
                            else if(inst == "add") add_reg(pmem, ptmp);
                            else if(inst == "sub") sub_reg(pmem, ptmp);
                            else if(inst == "mov") mov_reg(pmem, ptmp);
                            else if(it4->second == pcl) {
                                if(inst == "shl") shl_reg(pmem, pcl);
                                else if(inst == "shr") shr_reg(pmem, pcl);
                                else if(inst == "rcl") rcl_reg(pmem, pcl);
                                else if(inst == "rcr") rcr_reg(pmem, pcl);
                                else {
                                    cout << "ERROR25" << endl;
                                    isError = true;
                                    return;
                                }
                            }
                            else {
                                cout << "ERROR25" << endl;
                                isError = true;
                                return;
                            }
                        }
                    }
                }
                else {
                    cout << "ERROR25" << endl;
                    isError = true;
                    return;
                }
            }
            else {
                if(inst == "xor") xorFunc(it2->second, sec, true);
                else if(inst == "and") andFunc(it2->second, sec, true);
                else if(inst == "or") orFunc(it2->second, sec, true);
                else if(inst == "cmp") cmpFunc(it2->second, sec, true);
                else if(inst == "add") addFunc(it2->second, sec, true);
                else if(inst == "sub") subFunc(it2->second, sec, true);
                else if(inst == "mov") movFunc(it2->second, sec, true);
                else if(inst == "shl") shlFunc(it2->second, sec, true);
                else if(inst == "shr") shrFunc(it2->second, sec, true);
                else if(inst == "rcl") rclFunc(it2->second, sec, true);
                else if(inst == "rcr") rcrFunc(it2->second, sec, true);
            }
        }
        else {
            if(inst == "xor") xorFunc(it1->second, sec, true);
            else if(inst == "and") andFunc(it1->second, sec, true);
            else if(inst == "or") orFunc(it1->second, sec, true);
            else if(inst == "cmp") cmpFunc(it1->second, sec, true);
            else if(inst == "add") addFunc(it1->second, sec, true);
            else if(inst == "sub") subFunc(it1->second, sec, true);
            else if(inst == "mov") movFunc(it1->second, sec, true);
            else if(inst == "shl") shlFunc(it1->second, sec, true);
            else if(inst == "shr") shrFunc(it1->second, sec, true);
            else if(inst == "rcl") rclFunc(it1->second, sec, true);
            else if(inst == "rcr") rcrFunc(it1->second, sec, true);
        }
    }
}

template  <typename regtype1, typename regtype2>
void rcr_reg(regtype1 *first, regtype2 *sec){
    int count = *sec;
    *first = (*first >> (count-1)|(*first << (sizeof(*first)*8 - (count-1))));
    cf = (*first & 1) != 0;
    *first = (*first >> 1|(*first << (sizeof(*first)*8 - 1)));
}

template  <typename regtype>
void rcrFunc(regtype *first, string sec, bool isReg){
    if(sec == "cl"){
        rcr_reg(first, pcl);
    }
    else if (sec.at(sec.size() - 1) == 'h' || sec.at(0) == '0') {
        if(sec.at(sec.size() - 1) == 'h' || sec.at(sec.size() - 1) == 'd') sec = sec.substr(0, sec.size() - 1);
        auto temp = hex2dec(sec);
        auto *ptmp = &temp;
        rcr_reg(first, ptmp);
    } else if(sec.at(sec.size()-1) == 'd'){
        sec = sec.substr(0, sec.size()-1);
        auto temp = stoi(sec);
        auto *ptmp = &temp;
        rcr_reg(first, ptmp);
    } else if(areDigit(sec)){
        auto temp = stoi(sec);
        auto *ptmp = &temp;
        rcr_reg(first, ptmp);
    } else{
        cout << "ERROR52" << endl;
        isError = true;
        return;
    }
}


template  <typename regtype1, typename regtype2>
void rcl_reg(regtype1 *first, regtype2 *sec){
    //cf yi bitlere yapıştır.
    int count = *sec;
    for(int i = 0; i < count; i++){
        bool oldcf = cf;
        cf = (*first & (1 << (sizeof(*first)*8 - 1))) != 0;
        *first = *first << 1;
        *first ^= ((-1)*oldcf ^ *first) & (1UL << 0);
    }
}

template  <typename regtype>
void rclFunc(regtype *first, string sec, bool isReg){
    if(sec == "cl"){
        rcl_reg(first, pcl);
    }
    else if (sec.at(sec.size() - 1) == 'h' || sec.at(0) == '0') {
        if(sec.at(sec.size() - 1) == 'h' || sec.at(sec.size() - 1) == 'd') sec = sec.substr(0, sec.size() - 1);
        auto temp = hex2dec(sec);
        auto *ptmp = &temp;
        rcl_reg(first, ptmp);
    } else if(sec.at(sec.size()-1) == 'd'){
        sec = sec.substr(0, sec.size()-1);
        auto temp = stoi(sec);
        auto *ptmp = &temp;
        rcl_reg(first, ptmp);
    } else if(areDigit(sec)){
        auto temp = stoi(sec);
        auto *ptmp = &temp;
        rcl_reg(first, ptmp);
    } else{
        cout << "ERROR62" << endl;
        isError = true;
        return;
    }
}

template  <typename regtype1, typename regtype2>
void shr_reg(regtype1 *first, regtype2 *sec){
    int count = *sec;
    *first = *first >> (count-1);
    cf = (*first & 1) != 0;
    *first = *first >> 1;
}

template  <typename regtype>
void shrFunc(regtype *first, string sec, bool isReg){
    if(sec == "cl"){
        shr_reg(first, pcl);
    }
    else if (sec.at(sec.size() - 1) == 'h' || sec.at(0) == '0') {
        if(sec.at(sec.size() - 1) == 'h' || sec.at(sec.size() - 1) == 'd') sec = sec.substr(0, sec.size() - 1);
        auto temp = hex2dec(sec);
        auto *ptmp = &temp;
        shr_reg(first, ptmp);
    } else if(sec.at(sec.size()-1) == 'd'){
        sec = sec.substr(0, sec.size()-1);
        auto temp = stoi(sec);
        auto *ptmp = &temp;
        shr_reg(first, ptmp);
    } else if(areDigit(sec)){
        auto temp = stoi(sec);
        auto *ptmp = &temp;
        shr_reg(first, ptmp);
    } else{
        cout << "ERROR72" << endl;
        isError = true;
        return;
    }
}


template  <typename regtype1, typename regtype2>
void shl_reg(regtype1 *first, regtype2 *sec){
    int count = *sec;
    *first = *first << (count-1);
    cf = (*first & (1 << (sizeof(*first)*4 - 1))) != 0;
    *first = *first << 1;
}

template  <typename regtype>
void shlFunc(regtype *first, string sec, bool isReg){
    if(sec == "cl"){
        shl_reg(first, pcl);
    }
    else if (sec.at(sec.size() - 1) == 'h' || sec.at(0) == '0') {
        if(sec.at(sec.size() - 1) == 'h' || sec.at(sec.size() - 1) == 'd') sec = sec.substr(0, sec.size() - 1);
        auto temp = hex2dec(sec);
        auto *ptmp = &temp;
        shl_reg(first, ptmp);
    } else if(sec.at(sec.size()-1) == 'd'|| areDigit(sec)) {
        if(sec.at(sec.size()-1) == 'd') sec = sec.substr(0,sec.size()-1);
        auto temp = stoi(sec);
        auto *ptmp = &temp;
        shl_reg(first, ptmp);
    } else{
        cout << "ERROR82" << endl;
        isError = true;
        return;
    }
}

template  <typename regtype1, typename regtype2>
void add_reg(regtype1 *first, regtype2 *sec) {
    int tmp = *first + *sec;
    if(tmp >= pow(256, sizeof(*first))) cf = 1;
    else cf = 0;
    *first += *sec;
    if(*first == 0) zf = 1;
    else zf = 0;
}

template <class regtype>
void addFunc(regtype *first, string sec, bool isReg){
    if (vars.count((sec + "1")) || vars.count((sec + "2"))) {
        auto it = vars.find(sec + "1");
        if (it == vars.end()) {
            it = vars.find(sec + "2");
            if(sizeof(*first) == 1) {
                cout << "ERROR32" << endl;
                isError = true;
                return;
            } else {
                auto tmp = (unsigned short) (memory[it->second+1]*256 + memory[it->second]);
                auto *ptmp = &tmp;
                add_reg(first, ptmp);
            }
        } else {
            if(sizeof(*first) == 2) {
                auto tmp = (unsigned short) (memory[it->second+1]*256 + memory[it->second]);
                auto *ptmp = &tmp;
                add_reg(first, ptmp);
            } else {
                auto *ptmp = &memory[it->second];
                add_reg(first, ptmp);
            }
        }
    } else if (sec == "ax") {
        add_reg(first, pax);
    } else if (sec == "bx") {
        add_reg(first, pbx);
    } else if (sec == "cx") {
        add_reg(first, pcx);
    } else if (sec == "dx") {
        add_reg(first, pdx);
    } else if (sec == "di") {
        add_reg(first, pdi);
    } else if (sec == "sp") {
        add_reg(first, psp);
    } else if (sec == "si") {
        add_reg(first, psi);
    } else if (sec == "bp") {
        add_reg(first, pbp);
    } else if (sec == "ah") {
        add_reg(first, pah);
    } else if (sec == "al") {
        add_reg(first, pal);
    } else if (sec == "bh") {
        add_reg(first, pbh);
    } else if (sec == "bl") {
        add_reg(first, pbl);
    } else if (sec == "ch") {
        add_reg(first, pch);
    } else if (sec == "cl") {
        add_reg(first, pcl);
    } else if (sec == "dh") {
        add_reg(first, pdh);
    } else if (sec == "dl") {
        add_reg(first, pdl);
    }
    else if (sec.at(sec.size() - 1) == 'h' || sec.at(0) == '0') {
        if(sec.at(sec.size() - 1) == 'h' || sec.at(sec.size() - 1) == 'd') sec = sec.substr(0, sec.size() - 1);
        int tmp = hex2dec(sec);
        if(sizeof(*first) == 1 && isReg && tmp > 255) {
            cout << "ERROR32" << endl;
            isError = true;
            return;
        }else {
            if(sizeof(*first) == 1) {
                auto temp = (unsigned char) hex2dec(sec);
                auto *ptmp = &temp;
                add_reg(first, ptmp);
            }
            else {
                auto temp = (unsigned short) hex2dec(sec);
                auto *ptmp = &temp;
                add_reg(first, ptmp);
            }
        }
    }
    else if (sec.at(sec.size() - 1) == '\'') {
        unsigned char temp = sec.at(1);
        if(sizeof(*first) == 1) {
            auto *ptmp = &temp;
            add_reg(first, ptmp);
        }
        else {
            auto tmp = (unsigned short) temp;
            auto *ptmp = &tmp;
            add_reg(first, ptmp);
        }
    }
    else if(sec.at(sec.size()-1) == 'd'|| areDigit(sec)) {
        if(sec.at(sec.size()-1) == 'd') sec = sec.substr(0,sec.size()-1);
        if(sizeof(*first) == 1 && isReg && isReg && stoi(sec) > 255) {
            cout << "ERROR32" << endl;
            isError = true;
            return;
        }
        else {
            if(sizeof(*first) == 1) {
                auto temp = (unsigned char) stoi(sec);
                auto *ptmp = &temp;
                add_reg(first, ptmp);
            }
            else {
                auto temp = (unsigned short) stoi(sec);
                auto *ptmp = &temp;
                add_reg(first, ptmp);
            }
        }
    }
    else {
        cout << "ERROR32" << endl;
        isError = true;
        return;
    }
}

template  <typename regtype1, typename regtype2>
void sub_reg(regtype1 *first, regtype2 *sec) {
    if(*sec > *first) cf = 1;
    else cf = 0;
    *first -= *sec;
    if(*first == 0) zf = 1;
    else zf = 0;
}

template <class regtype>
void subFunc(regtype *first, string sec, bool isReg){
    if (vars.count((sec + "1")) || vars.count((sec + "2"))) {
        auto it = vars.find(sec + "1");
        if (it == vars.end()) {
            it = vars.find(sec + "2");
            if(sizeof(*first) == 1) {
                cout << "ERROR32" << endl;
                isError = true;
                return;
            } else {
                auto tmp = (unsigned short) (memory[it->second+1]*256 + memory[it->second]);
                auto *ptmp = &tmp;
                sub_reg(first, ptmp);
            }
        } else {
            if(sizeof(*first) == 2) {
                auto tmp = (unsigned short) memory[it->second];
                auto *ptmp = &tmp;
                sub_reg(first, ptmp);
            } else {
                auto *ptmp = &memory[it->second];
                sub_reg(first, ptmp);
            }
        }
    } else if (sec == "ax") {
        sub_reg(first, pax);
    } else if (sec == "bx") {
        sub_reg(first, pbx);
    } else if (sec == "cx") {
        sub_reg(first, pcx);
    } else if (sec == "dx") {
        sub_reg(first, pdx);
    } else if (sec == "di") {
        sub_reg(first, pdi);
    } else if (sec == "sp") {
        sub_reg(first, psp);
    } else if (sec == "si") {
        sub_reg(first, psi);
    } else if (sec == "bp") {
        sub_reg(first, pbp);
    } else if (sec == "ah") {
        sub_reg(first, pah);
    } else if (sec == "al") {
        sub_reg(first, pal);
    } else if (sec == "bh") {
        sub_reg(first, pbh);
    } else if (sec == "bl") {
        sub_reg(first, pbl);
    } else if (sec == "ch") {
        sub_reg(first, pch);
    } else if (sec == "cl") {
        sub_reg(first, pcl);
    } else if (sec == "dh") {
        sub_reg(first, pdh);
    } else if (sec == "dl") {
        sub_reg(first, pdl);
    }
    else if (sec.at(sec.size() - 1) == 'h' || sec.at(0) == '0') {
        if(sec.at(sec.size() - 1) == 'h' || sec.at(sec.size() - 1) == 'd') sec = sec.substr(0, sec.size() - 1);
        int tmp = hex2dec(sec);
        if(sizeof(*first) == 1 && isReg && tmp > 255) {
            cout << "ERROR32" << endl;
            isError = true;
            return;
        }else {
            if(sizeof(*first) == 1) {
                auto temp = (unsigned char) hex2dec(sec);
                auto *ptmp = &temp;
                sub_reg(first, ptmp);
            }
            else {
                auto temp = (unsigned short) hex2dec(sec);
                auto *ptmp = &temp;
                sub_reg(first, ptmp);
            }
        }
    }
    else if (sec.at(sec.size() - 1) == '\'') {
        unsigned char temp = sec.at(1);
        if(sizeof(*first) == 1) {
            auto *ptmp = &temp;
            sub_reg(first, ptmp);
        }
        else {
            auto tmp = (unsigned short) temp;
            auto *ptmp = &tmp;
            sub_reg(first, ptmp);
        }
    }
    else if(sec.at(sec.size()-1) == 'd'|| areDigit(sec)) {
        if(sec.at(sec.size()-1) == 'd') sec = sec.substr(0,sec.size()-1);
        if(sizeof(*first) == 1 && isReg && stoi(sec) > 255) {
            cout << "ERROR32" << endl;
            isError = true;
            return;
        }
        else {
            if(sizeof(*first) == 1) {
                auto temp = (unsigned char) stoi(sec);
                auto *ptmp = &temp;
                sub_reg(first, ptmp);
            }
            else {
                auto temp = (unsigned short) stoi(sec);
                auto *ptmp = &temp;
                sub_reg(first, ptmp);
            }
        }
    }
    else {
        cout << "ERROR32" << endl;
        isError = true;
        return;
    }
}

template  <typename regtype1, typename regtype2>
void mov_reg(regtype1 *first, regtype2 *sec) {
    *first = *sec;
}

template <class regtype>
void movFunc(regtype *first, string sec, bool isReg){
    if (vars.count((sec + "1")) || vars.count((sec + "2"))) {
        auto it = vars.find(sec + "1");
        if (it == vars.end()) {
            it = vars.find(sec + "2");
            if(sizeof(*first) == 1) {
                cout << "ERROR22" << endl;
                isError = true;
                return;
            } else {
                *first = (unsigned short) (memory[it->second+1]*256 + memory[it->second]);
            }
        } else {
            if(sizeof(*first) == 2) {
                *first = (unsigned short) memory[it->second];
            } else {
                *first = memory[it->second];
            }
        }
    } else if (sec == "ax") {
        mov_reg(first, pax);
    } else if (sec == "bx") {
        mov_reg(first, pbx);
    } else if (sec == "cx") {
        mov_reg(first, pcx);
    } else if (sec == "dx") {
        mov_reg(first, pdx);
    } else if (sec == "di") {
        mov_reg(first, pdi);
    } else if (sec == "sp") {
        mov_reg(first, psp);
    } else if (sec == "si") {
        mov_reg(first, psi);
    } else if (sec == "bp") {
        mov_reg(first, pbp);
    } else if (sec == "ah") {
        mov_reg(first, pah);
    } else if (sec == "al") {
        mov_reg(first, pal);
    } else if (sec == "bh") {
        mov_reg(first, pbh);
    } else if (sec == "bl") {
        mov_reg(first, pbl);
    } else if (sec == "ch") {
        mov_reg(first, pch);
    } else if (sec == "cl") {
        mov_reg(first, pcl);
    } else if (sec == "dh") {
        mov_reg(first, pdh);
    } else if (sec == "dl") {
        mov_reg(first, pdl);
    }
    else if (sec.at(sec.size() - 1) == 'h' || sec.at(0) == '0') {
        if(sec.at(sec.size() - 1) == 'h' || sec.at(sec.size() - 1) == 'd') sec = sec.substr(0, sec.size() - 1);
        int tmp = hex2dec(sec);
        if(sizeof(*first) == 1 && isReg && tmp > 255) {
            cout << "ERROR22" << endl;
            isError = true;
            return;
        }else {
            if(sizeof(*first) == 1) *first = (unsigned char) hex2dec(sec);
            else *first = (unsigned short) hex2dec(sec);
        }
    }
    else if (sec.at(sec.size() - 1) == '\'') {
        unsigned char temp = sec.at(1);
        if(sizeof(*first) == 1) *first = temp;
        else *first = (unsigned short) temp;
    }
    else if(sec.at(sec.size()-1) == 'd'|| areDigit(sec)) {
        if(sec.at(sec.size()-1) == 'd') sec = sec.substr(0,sec.size()-1);
        if(sizeof(*first) == 1 && isReg && stoi(sec) > 255) {
            cout << "ERROR22" << endl;
            isError = true;
            return;
        }
        else {
            if(sizeof(*first) == 1) *first = (unsigned char) stoi(sec);
            else *first = (unsigned short) stoi(sec);
        }
    }
    else {
        cout << "ERROR22" << endl;
        isError = true;
        return;
    }
}

template  <typename regtype1, typename regtype2>
void xor_reg(regtype1 *first, regtype2 *sec) {
    if(sizeof(*first) != sizeof(*sec)) {
        cout << "ERROR23" << endl;
        isError = true;
        return;
    }
    cf = 0;
    *first = *first ^ *sec;
    if(*first == 0) zf = 1;
    else zf = 0;
}

template <class regtype>
void xorFunc(regtype *first, string sec, bool isReg){
    //todo [sec] durumunu kontrol et
    if (vars.count((sec + "1")) || vars.count((sec + "2"))) {
        auto it = vars.find(sec + "1");
        if (it == vars.end()) {
            it = vars.find(sec + "2");
            if(sizeof(*first) == 1) {
                cout << "ERROR32" << endl;
                isError = true;
                return;
            } else {
                auto tmp = (unsigned short) (memory[it->second+1]*256 + memory[it->second]);
                auto *ptmp = &tmp;
                xor_reg(first, ptmp);
            }
        } else {
            if(sizeof(*first) == 2) {
                auto tmp = (unsigned short) memory[it->second];
                auto *ptmp = &tmp;
                xor_reg(first, ptmp);
            } else {
                auto *ptmp = &memory[it->second];
                xor_reg(first, ptmp);
            }
        }
    } else if (sec == "ax") {
        xor_reg(first, pax);
    } else if (sec == "bx") {
        xor_reg(first, pbx);
    } else if (sec == "cx") {
        xor_reg(first, pcx);
    } else if (sec == "dx") {
        xor_reg(first, pdx);
    } else if (sec == "di") {
        xor_reg(first, pdi);
    } else if (sec == "sp") {
        xor_reg(first, psp);
    } else if (sec == "si") {
        xor_reg(first, psi);
    } else if (sec == "bp") {
        xor_reg(first, pbp);
    } else if (sec == "ah") {
        xor_reg(first, pah);
    } else if (sec == "al") {
        xor_reg(first, pal);
    } else if (sec == "bh") {
        xor_reg(first, pbh);
    } else if (sec == "bl") {
        xor_reg(first, pbl);
    } else if (sec == "ch") {
        xor_reg(first, pch);
    } else if (sec == "cl") {
        xor_reg(first, pcl);
    } else if (sec == "dh") {
        xor_reg(first, pdh);
    } else if (sec == "dl") {
        xor_reg(first, pdl);
    }
    else if (sec.at(sec.size() - 1) == 'h' || sec.at(0) == '0') {
        if(sec.at(sec.size() - 1) == 'h' || sec.at(sec.size() - 1) == 'd') sec = sec.substr(0, sec.size() - 1);
        int tmp = hex2dec(sec);
        if(sizeof(*first) == 1 && isReg && tmp > 255) {
            cout << "ERROR32" << endl;
            isError = true;
            return;
        }else {
            if(sizeof(*first) == 1) {
                auto temp = (unsigned char) hex2dec(sec);
                auto *ptmp = &temp;
                xor_reg(first, ptmp);
            }
            else {
                auto temp = (unsigned short) hex2dec(sec);
                auto *ptmp = &temp;
                xor_reg(first, ptmp);
            }
        }
    }
    else if (sec.at(sec.size() - 1) == '\'') {
        unsigned char temp = sec.at(1);
        if(sizeof(*first) == 1) {
            auto *ptmp = &temp;
            xor_reg(first, ptmp);
        }
        else {
            auto tmp = (unsigned short) temp;
            auto *ptmp = &tmp;
            xor_reg(first, ptmp);
        }
    }
    else if(sec.at(sec.size()-1) == 'd'|| areDigit(sec)) {
        if(sec.at(sec.size()-1) == 'd') sec = sec.substr(0,sec.size()-1);
        if(sizeof(*first) == 1 && isReg && stoi(sec) > 255) {
            cout << "ERROR32" << endl;
            isError = true;
            return;
        }
        else {
            if(sizeof(*first) == 1) {
                auto temp = (unsigned char) stoi(sec);
                auto *ptmp = &temp;
                xor_reg(first, ptmp);
            }
            else {
                auto temp = (unsigned short) stoi(sec);
                auto *ptmp = &temp;
                xor_reg(first, ptmp);
            }
        }
    }
    else {
        cout << "ERROR32" << endl;
        isError = true;
        return;
    }
}

template  <typename regtype1, typename regtype2>
void and_reg(regtype1 *first, regtype2 *sec) {
    if(sizeof(*first) != sizeof(*sec)) {
        cout << "ERROR23" << endl;
        isError = true;
        return;
    }
    cf = 0;
    *first = *first & *sec;
    if(*first == 0) zf = 1;
    else zf = 0;
}

template <class regtype>
void andFunc(regtype *first, string sec, bool isReg){
    //todo [sec] durumunu kontrol et
    if (vars.count((sec + "1")) || vars.count((sec + "2"))) {
        auto it = vars.find(sec + "1");
        if (it == vars.end()) {
            it = vars.find(sec + "2");
            if(sizeof(*first) == 1) {
                cout << "ERROR32" << endl;
                isError = true;
                return;
            } else {
                auto tmp = (unsigned short) (memory[it->second+1]*256 + memory[it->second]);
                auto *ptmp = &tmp;
                and_reg(first, ptmp);
            }
        } else {
            if(sizeof(*first) == 2) {
                auto tmp = (unsigned short) memory[it->second];
                auto *ptmp = &tmp;
                and_reg(first, ptmp);
            } else {
                auto *ptmp = &memory[it->second];
                and_reg(first, ptmp);
            }
        }
    } else if (sec == "ax") {
        and_reg(first, pax);
    } else if (sec == "bx") {
        and_reg(first, pbx);
    } else if (sec == "cx") {
        and_reg(first, pcx);
    } else if (sec == "dx") {
        and_reg(first, pdx);
    } else if (sec == "di") {
        and_reg(first, pdi);
    } else if (sec == "sp") {
        and_reg(first, psp);
    } else if (sec == "si") {
        and_reg(first, psi);
    } else if (sec == "bp") {
        and_reg(first, pbp);
    } else if (sec == "ah") {
        and_reg(first, pah);
    } else if (sec == "al") {
        and_reg(first, pal);
    } else if (sec == "bh") {
        and_reg(first, pbh);
    } else if (sec == "bl") {
        and_reg(first, pbl);
    } else if (sec == "ch") {
        and_reg(first, pch);
    } else if (sec == "cl") {
        and_reg(first, pcl);
    } else if (sec == "dh") {
        and_reg(first, pdh);
    } else if (sec == "dl") {
        and_reg(first, pdl);
    }
    else if (sec.at(sec.size() - 1) == 'h' || sec.at(0) == '0') {
        if(sec.at(sec.size() - 1) == 'h' || sec.at(sec.size() - 1) == 'd') sec = sec.substr(0, sec.size() - 1);
        int tmp = hex2dec(sec);
        if(sizeof(*first) == 1 && isReg && tmp > 255) {
            cout << "ERROR32" << endl;
            isError = true;
            return;
        }else {
            if(sizeof(*first) == 1) {
                auto temp = (unsigned char) hex2dec(sec);
                auto *ptmp = &temp;
                and_reg(first, ptmp);
            }
            else {
                auto temp = (unsigned short) hex2dec(sec);
                auto *ptmp = &temp;
                and_reg(first, ptmp);
            }
        }
    }
    else if (sec.at(sec.size() - 1) == '\'') {
        unsigned char temp = sec.at(1);
        if(sizeof(*first) == 1) {
            auto *ptmp = &temp;
            and_reg(first, ptmp);
        }
        else {
            auto tmp = (unsigned short) temp;
            auto *ptmp = &tmp;
            and_reg(first, ptmp);
        }
    }
    else if(sec.at(sec.size()-1) == 'd'|| areDigit(sec)) {
        if(sec.at(sec.size()-1) == 'd') sec = sec.substr(0,sec.size()-1);
        if(sizeof(*first) == 1 && isReg && stoi(sec) > 255) {
            cout << "ERROR32" << endl;
            isError = true;
            return;
        }
        else {
            if(sizeof(*first) == 1) {
                auto temp = (unsigned char) stoi(sec);
                auto *ptmp = &temp;
                and_reg(first, ptmp);
            }
            else {
                auto temp = (unsigned short) stoi(sec);
                auto *ptmp = &temp;
                and_reg(first, ptmp);
            }
        }
    }
    else {
        cout << "ERROR32" << endl;
        isError = true;
        return;
    }
}

template  <typename regtype1, typename regtype2>
void or_reg(regtype1 *first, regtype2 *sec) {
    if(sizeof(*first) != sizeof(*sec)) {
        cout << "ERROR23" << endl;
        isError = true;
        return;
    }
    cf = 0;
    *first = *first | *sec;
    if(*first == 0) zf = 1;
    else zf = 0;
}

template <class regtype>
void orFunc(regtype *first, string sec, bool isReg){
    //todo [sec] durumunu kontrol et
    if (vars.count((sec + "1")) || vars.count((sec + "2"))) {
        auto it = vars.find(sec + "1");
        if (it == vars.end()) {
            it = vars.find(sec + "2");
            if(sizeof(*first) == 1) {
                cout << "ERROR32" << endl;
                isError = true;
                return;
            } else {
                auto tmp = (unsigned short) (memory[it->second+1]*256 + memory[it->second]);
                auto *ptmp = &tmp;
                or_reg(first, ptmp);
            }
        } else {
            if(sizeof(*first) == 2) {
                auto tmp = (unsigned short) memory[it->second];
                auto *ptmp = &tmp;
                or_reg(first, ptmp);
            } else {
                auto *ptmp = &memory[it->second];
                or_reg(first, ptmp);
            }
        }
    } else if (sec == "ax") {
        or_reg(first, pax);
    } else if (sec == "bx") {
        or_reg(first, pbx);
    } else if (sec == "cx") {
        or_reg(first, pcx);
    } else if (sec == "dx") {
        or_reg(first, pdx);
    } else if (sec == "di") {
        or_reg(first, pdi);
    } else if (sec == "sp") {
        or_reg(first, psp);
    } else if (sec == "si") {
        or_reg(first, psi);
    } else if (sec == "bp") {
        or_reg(first, pbp);
    } else if (sec == "ah") {
        or_reg(first, pah);
    } else if (sec == "al") {
        or_reg(first, pal);
    } else if (sec == "bh") {
        or_reg(first, pbh);
    } else if (sec == "bl") {
        or_reg(first, pbl);
    } else if (sec == "ch") {
        or_reg(first, pch);
    } else if (sec == "cl") {
        or_reg(first, pcl);
    } else if (sec == "dh") {
        or_reg(first, pdh);
    } else if (sec == "dl") {
        or_reg(first, pdl);
    }
    else if (sec.at(sec.size() - 1) == 'h' || sec.at(0) == '0') {
        if(sec.at(sec.size() - 1) == 'h' || sec.at(sec.size() - 1) == 'd') sec = sec.substr(0, sec.size() - 1);
        int tmp = hex2dec(sec);
        if(sizeof(*first) == 1 && isReg && tmp > 255) {
            cout << "ERROR32" << endl;
            isError = true;
            return;
        }else {
            if(sizeof(*first) == 1) {
                auto temp = (unsigned char) hex2dec(sec);
                auto *ptmp = &temp;
                or_reg(first, ptmp);
            }
            else {
                auto temp = (unsigned short) hex2dec(sec);
                auto *ptmp = &temp;
                or_reg(first, ptmp);
            }
        }
    }
    else if (sec.at(sec.size() - 1) == '\'') {
        unsigned char temp = sec.at(1);
        if(sizeof(*first) == 1) {
            auto *ptmp = &temp;
            or_reg(first, ptmp);
        }
        else {
            auto tmp = (unsigned short) temp;
            auto *ptmp = &tmp;
            or_reg(first, ptmp);
        }
    }
    else if(sec.at(sec.size()-1) == 'd'|| areDigit(sec)) {
        if(sec.at(sec.size()-1) == 'd') sec = sec.substr(0,sec.size()-1);
        if(sizeof(*first) == 1 && isReg && stoi(sec) > 255) {
            cout << "ERROR32" << endl;
            isError = true;
            return;
        }
        else {
            if(sizeof(*first) == 1) {
                auto temp = (unsigned char) stoi(sec);
                auto *ptmp = &temp;
                or_reg(first, ptmp);
            }
            else {
                auto temp = (unsigned short) stoi(sec);
                auto *ptmp = &temp;
                or_reg(first, ptmp);
            }
        }
    }
    else {
        cout << "ERROR32" << endl;
        isError = true;
        return;
    }
}

template  <typename regtype1, typename regtype2>
void cmp_reg(regtype1 *first, regtype2 *sec) {
    int result;
    if(sizeof(*first) == 1 && sizeof(*sec) == 2) {
        cout << "ERROR23" << endl;
        isError = true;
        return;
    }
    else if(sizeof(*first) == 2 && sizeof(*sec) == 1) {
        auto extended = (unsigned short) *sec;
        result = *first - extended;
    }
    else result = *first - *sec;
    if(result > 0) {
        zf = 0;
        cf = 0;
    }
    else if(result < 0) {
        zf = 0;
        cf = 1;
    }
    else {
        zf = 1;
        cf = 0;
    }
}

template <class regtype>
void cmpFunc(regtype *first, string sec, bool isReg){
    if (vars.count((sec + "1")) || vars.count((sec + "2"))) {
        auto it = vars.find(sec + "1");
        if (it == vars.end()) {
            it = vars.find(sec + "2");
            if(sizeof(*first) == 1) {
                cout << "ERROR42" << endl;
                isError = true;
                return;
            } else {
                auto tmp = (unsigned short) (memory[it->second+1]*256 + memory[it->second]);
                unsigned short *ptmp = &tmp;
                cmp_reg(first, ptmp);
            }
        }
        else {
            if(sizeof(*first) == 2) {
                auto tmp = (unsigned short) memory[it->second];
                unsigned short *ptmp = &tmp;
                cmp_reg(first, ptmp);
            }
            else {
                unsigned char *ptmp = &memory[it->second];
                cmp_reg(first, ptmp);
            }
        }
    } else if (sec == "ax") {
        cmp_reg(first, pax);
    } else if (sec == "bx") {
        cmp_reg(first, pbx);
    } else if (sec == "cx") {
        cmp_reg(first, pcx);
    } else if (sec == "dx") {
        cmp_reg(first, pdx);
    } else if (sec == "di") {
        cmp_reg(first, pdi);
    } else if (sec == "sp") {
        cmp_reg(first, psp);
    } else if (sec == "si") {
        cmp_reg(first, psi);
    } else if (sec == "bp") {
        cmp_reg(first, pbp);
    } else if (sec == "ah") {
        cmp_reg(first, pah);
    } else if (sec == "al") {
        cmp_reg(first, pal);
    } else if (sec == "bh") {
        cmp_reg(first, pbh);
    } else if (sec == "bl") {
        cmp_reg(first, pbl);
    } else if (sec == "ch") {
        cmp_reg(first, pch);
    } else if (sec == "cl") {
        cmp_reg(first, pcl);
    } else if (sec == "dh") {
        cmp_reg(first, pdh);
    } else if (sec == "dl") {
        cmp_reg(first, pdl);
    } else if (sec.at(sec.size() - 1) == 'h' || sec.at(0) == '0') {
        if(sec.at(sec.size() - 1) == 'h' || sec.at(sec.size() - 1) == 'd') sec = sec.substr(0, sec.size() - 1);
        if(sizeof(*first) == 1 && isReg && sec.size() > 2) {
            cout << "ERROR42" << endl;
            isError = true;
            return;
        }else {
            if(sizeof(*first) == 1) {
                unsigned char tmp = hex2dec(sec);
                unsigned char *ptmp = &tmp;
                cmp_reg(first, ptmp);
            }
            else {
                auto tmp = (unsigned short) hex2dec(sec);
                unsigned short *ptmp = &tmp;
                cmp_reg(first, ptmp);
            }
        }
    }
    else if (sec.at(sec.size() - 1) == '\'') {
        unsigned char temp = sec.at(sec.size()-2);
        if(sizeof(*first) == 2) {
            auto tmp = (unsigned short) temp;
            unsigned short *ptmp = &tmp;
            cmp_reg(first, ptmp);
        }
        else {
            unsigned char *ptmp = &temp;
            cmp_reg(first, ptmp);
        }
    }
    else if(sec.at(sec.size()-1) == 'd'|| areDigit(sec)) {
        if(sec.at(sec.size()-1) == 'd') sec = sec.substr(0,sec.size()-1);
        if(sizeof(*first) == 1 && isReg && stoi(sec) > 255) {
            cout << "ERROR42" << endl;
            isError = true;
            return;
        }
        else {
            if(sizeof(*first) == 1) {
                auto tmp = (unsigned char) stoi(sec);
                unsigned char *ptmp = &tmp;
                cmp_reg(first, ptmp);
            }
            else {
                auto tmp = (unsigned short) stoi(sec);
                unsigned short *ptmp = &tmp;
                cmp_reg(first, ptmp);
            }
        }
    }
    else {
        cout << "ERROR42" << endl;
        isError = true;
        return;
    }
}

void editStr(string &s) {
    replace(s.begin(), s.end(), ',', ' ');
    replace(s.begin(), s.end(), '"', '\'');
    string::iterator new_end = unique(s.begin(), s.end(), [=](char lhs, char rhs){return (lhs == rhs) && (lhs == ' '); });
    s.erase(new_end, s.end());
    if(s.at(0) == ' ') s = s.substr(1, s.size()-1);
    if(s.at(s.size()-1) == ' ') s = s.substr(0, s.size()-1);
    while(s.find(" [") != string::npos) {
        int index = s.find(" [");
        if(s.at(index-2) == ' ') s.erase(s.begin() + index);
    }
    if(s.find("[ ") != string::npos) s.replace(s.find("[ "),2,"[");
    if(s.find(" ]") != string::npos) s.replace(s.find(" ]"),2,"]");
    if(s.find("' ") != string::npos) s.replace(s.find("' "),2,"'");
    s = s + ' ';
    if(s.find(" ' ") != string::npos) s.replace(s.find(" ' "),3,"'");
    if(s.at(s.size()-1) == ' ') s = s.substr(0, s.size()-1);
}

template <class datatype>
void print_bits(datatype x)
{
    int i;

    for(i=8*sizeof(x)-1; i>=0; i--) {
        (x & (1 << i)) ? putchar('1') : putchar('0');
    }
    cout << "" << endl;
}

template <class datatype>
void hexToArray(unsigned char arr[], datatype x, int index){
    int tmp = x;
    if (sizeof(x) == 1){
        auto *rep = (unsigned char *)&tmp;
        arr[index] = rep[0];
    }
    else{
        auto *rep = (unsigned char *)&tmp;
        arr[index] = rep[1];
        arr[index+1] = rep[0];
    }
}

template <class datatype>
void mul_reg(datatype y){
    if (sizeof(y) == 1){
        int tmp = (*pal)*(y);
        *pal = tmp % 256;
        *pah = tmp / 256;
        if(*pah == 0) {
            of = 0;
            cf = 0;
        }
        else {
            of = 1;
            cf = 1;
        }
    }
    else{
        int tmp = ax*y;
        *pax = tmp % 65536;
        *pdx = tmp / 65536;
        if(*pdx == 0) {
            of = 0;
            cf = 0;
        }
        else {
            of = 1;
            cf = 1;
        }
    }
}

template <class datatype>
void div_reg(datatype y){
    if(sizeof(y) == 1) {
        unsigned short tmp = ax;
        *pal = tmp / y;
        *pah = tmp % y;
    }
    else {
        int tmp = (*pdx)*65536 + (*pax);
        if(tmp / y > 65535) {
            cout << "ERROR27" << endl;
            isError = true;
            return;
        }
        else {
            *pax = tmp / y;
            *pdx = tmp % y;
        }
    }
}

template <class datatype>
void print_hex(datatype x)
{
    if (sizeof(x) == 1)
        printf("%02x\n",x);
    else
        printf("%04x\n",x);
}
int hex2dec(string hex) {
    int result = 0;
    for (int i=0; i<hex.length(); i++) {
        if (hex[i]>=48 && hex[i]<=57){
            result += (hex[i]-48)*pow(16,hex.length()-i-1);
        } else if (hex[i]>=65 && hex[i]<=70) {
            result += (hex[i]-55)*pow(16,hex.length()-i-1);
        } else if (hex[i]>=97 && hex[i]<=102) {
            result += (hex[i]-87)*pow(16,hex.length()-i-1);
        }
    }
    return result;
}
bool areDigit(string s) {
    for (int i=0; i< s.length(); i++) {
        if (s[i] >= 48 && s[i] <= 57) continue;
        else return false;
    }
    return true;
}