#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <sstream>
#include "cmath"
#include <algorithm>

using namespace std;

// prototypes
template <class datatype> void print_bits(datatype x) ;
template <class datatype> void print_hex(datatype x) ;
template <class regtype>  void add_reg(regtype *preg, unsigned char c)  ;
template <class regtype>  void sub_reg(regtype *preg, unsigned char c)  ;
template <class regtype>  void not_reg(regtype *preg)  ;
template <class regtype>  void or_reg_reg(regtype *preg1, regtype *preg2)  ;
template <class regtype>  void xor_reg_reg(regtype *preg1, regtype *preg2)  ;
template  <class regtype> void and_reg_reg(regtype *preg1, regtype *preg2)  ;
template <class regtype>  void mul_reg(regtype *preg)  ;
template <class regtype>  void div_reg(regtype *preg)  ;
template <class datatype> unsigned char hex2dec(datatype hex);
template <class regtype>  void inc_reg(regtype *preg);
template <class regtype>  void dec_reg(regtype *preg);
void print_16bitregs() ;
void movFunc(string first, string &sec) ;
string erase(string s);
template <class datatype> void hexToArray(unsigned char arr[], datatype x, int index);
bool check_number(string str);
template <class regtype> void firstParaYes_secParaNo(regtype *first, string sec, char desType);
template <class regtype> void firstParaNo_secParaNo_add(regtype *first, string sec);
template <class regtype> void firstParaNo_secParaNo_sub(regtype *first, string sec);
template <class regtype> void firstParaNo_secParaNo_mov(regtype *first, string sec);
template <class regtype> void firstParaNo_secParaYes(regtype *first, string &sec);
void addFunc(const string& first, const string& sec);
void subFunc(const string& first, const string& sec);
void editStr(string &s);
template <class datatype> void multiplication(datatype y);
template <class datatype> void division(datatype y);

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


int main(int argc, char* argv[])
{

    vector<string> codelines;
    int memoryIdx = 0;

    string s = "66";
    cout << hex2dec(s) << endl;


    ifstream infile(argv[1]);
    string line = "";
    while(getline(infile, line)){
        lines.push_back(line);
        /*if(label){
            labels.insert(line, lines.size()-1);
        }
         else if(variable){
            var.insert(line, lines.size()-1);
        */

        //burada biri label için diğeri variable için olmak üzere 2 tane map yap
        //key olarak string alsın value olarak da lines arrayinin indexini alsın
    }

    bool cont = false;
    //BURASI PROGRAMIN LABEL VE VARİABLE OKUDUĞU YER

    /*
    for(int i = 0; i < lines.size(); i++){
        for(int j = 0; j < lines[i].size(); j++){
            if(lines[i].at(j) == ','){
                lines[i][j] = ' ';
            }
        }
    }
     */


    for(int i = 0; i < lines.size(); i++){
        if(lines[i] == "code segment"){
            cont = true;
            continue;
        }
        else if(lines[i] == "code ends"){
            cont = false;
        }
        if(cont){
            codelines.push_back(lines[i]);
        }
        cout << lines[i] << endl;
    }

    int i = 0;
    editStr(codelines[0]);
    while(codelines[i] != "int20h") {
        if(codelines[i].substr(0,3) == "mov"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,3) == "add"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,3) == "sub"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,3) == "dec"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,3) == "inc"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,3) == "mul"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,3) == "div"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,3) == "xor"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,2) == "or"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,3) == "and"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,3) == "not"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,3) == "rcl"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,3) == "rcr"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,3) == "shl"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,3) == "shr"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,4) == "push"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,3) == "pop"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,3) == "nop"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,3) == "cmp"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,2) == "jz"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,3) == "jnz"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,2) == "jn"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,3) == "jne"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,2) == "ja"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,3) == "jae"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,2) == "jb"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,3) == "jbe"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,4) == "jnae"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,3) == "jnb"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,4) == "jnbe"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,3) == "jnc"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,2) == "jc"){
            memoryIdx += 6;
        }
        else if(codelines[i] == "int21h"){
            memoryIdx += 6;
        }
        else{
            if(codelines[i].at(codelines[i].size()-1) == ':'){
                string tmp = codelines[i].substr(0, codelines[i].size()-1);
                labels.insert(pair<string, int> (tmp, i));
            }else{
                cout << "error: irrelevant code" << endl;
                return 0;
            }
        }
        i++;
        editStr(codelines[i]);
    }
    memoryIdx += 6;

    //VARİABLE OKUMA
    for(int j = i+1; j < codelines.size(); j++){
        string tmp = codelines[j];
        string var;
        string type;
        string info;
        stringstream check1(tmp);
        getline(check1, var, ' ');
        getline(check1, type, ' ');
        getline(check1, info, ' ');
        if(type == "db"){
            info = info.substr(0, info.size()-1);
            unsigned char data = hex2dec(info);
            vars.insert(pair<string, int> (var + "1", memoryIdx));
            memory[memoryIdx] = data;
            memoryIdx++;
        }else if(type == "dw"){
            info = info.substr(0, info.size()-1);
            unsigned char data1 = hex2dec(info.substr(0,2));
            unsigned char data2 = hex2dec(info.substr(2,2));
            vars.insert(pair<string, int> (var + "2", memoryIdx));
            memory[memoryIdx] = data1;
            memory[memoryIdx+1] = data2;
            memoryIdx+=2;
        }else{
            cout << "ERROR" << endl;
            isError = true;
            break;
        }
    }

    //ASIL KOD BURADAN BAŞLIYOR
    for(int i = 0; i < codelines.size(); i++){
        if(isError) break;
        string tmp = codelines[i];
        string first;
        string sec;
        if(tmp.substr(0, 3) == "mov") {
            tmp = tmp.substr(3, tmp.size()-3);
            stringstream check1(tmp);
            getline(check1, first, ' ');
            getline(check1, sec, ' ');
            bool isOffset = false;
            if(sec.substr(0,6) == "offset") {
                sec = sec.substr(6, sec.size()-6);
                isOffset = true;
            }
            if(isOffset){
                if (vars.count((sec + "1")) || vars.count((sec + "2"))) {
                    auto it = vars.find(sec + "1");
                    if (it == vars.end())
                        it = vars.find(sec + "2");
                    if(first.find('[') != string::npos){
                        char desType = first.at(0);
                        first = first.substr(2, first.size()-3);
                        if(first == "ax"){
                            memory[ax] = it->second;
                        }
                        else if(first == "bx"){
                            memory[bx] = it->second;
                        }
                        else if(first == "cx"){
                            memory[cx] = it->second;
                        }
                        else if(first == "dx"){
                            memory[dx] = it->second;
                        }
                        else if(first == "di"){
                            memory[di] = it->second;
                        }
                        else if(first == "sp"){
                            memory[sp] = it->second;
                        }
                        else if(first == "si"){
                            memory[si] = it->second;
                        }
                        else if(first == "bp"){
                            memory[bp] = it->second;
                        }
                        else if(first == "ah"){
                            memory[*pah] = it->second;
                        }
                        else if(first == "al"){
                            memory[*pal] = it->second;
                        }
                        else if(first == "bh"){
                            memory[*pbh] = it->second;
                        }
                        else if(first == "bl"){
                            memory[*pbl] = it->second;
                        }
                        else if(first == "ch"){
                            memory[*pch] = it->second;
                        }
                        else if(first == "cl"){
                            memory[*pcl] = it->second;
                        }
                        else if(first == "dh"){
                            memory[*pdh]= it->second;
                        }
                        else if(first == "dl"){
                            memory[*pdl] = it->second;
                        }
                    }else{
                        if(first == "ax"){
                            //TODO acaba burada ax yerine ah ve al'yi mi değiştirmeliyiz? çünkü ax bu ikisinden oluşuyor
                            ax = it->second;
                        }
                        else if(first == "bx"){
                            bx = it->second;
                        }
                        else if(first == "cx"){
                            cx = it->second;
                        }
                        else if(first == "dx"){
                            dx = it->second;
                        }
                        else if(first == "di"){
                            di = it->second;
                        }
                        else if(first == "sp"){
                            sp = it->second;
                        }
                        else if(first == "si"){
                            si = it->second;
                        }
                        else if(first == "bp"){
                            bp = it->second;
                        }
                        else if(first == "ah"){
                            *pah = it->second;
                        }
                        else if(first == "al"){
                            *pal = it->second;
                        }
                        else if(first == "bh"){
                            *pbh = it->second;
                        }
                        else if(first == "bl"){
                            *pbl = it->second;
                        }
                        else if(first == "ch"){
                            *pch = it->second;
                        }
                        else if(first == "cl"){
                            *pcl = it->second;
                        }
                        else if(first == "dh"){
                            *pdh = it->second;
                        }
                        else if(first == "dl"){
                            *pdl = it->second;
                        }
                    }
                }else{
                    cout << "Key-value pair not present in map" << endl;
                }
            }else {
                movFunc(first, sec);
            }
            //VAR IN MEMORYDEKİ ADRESİNİ DÖNMESİ GEREK
        }
        else if(tmp.substr(0, 3) == "add"){
            tmp = tmp.substr(3, tmp.size()-3);
            stringstream check1(tmp);
            getline(check1, first, ' ');
            getline(check1, sec, ' ');
            addFunc(first, sec);
        }
        else if(tmp.substr(0, 3) == "sub"){
            tmp = tmp.substr(3, tmp.size()-3);
            stringstream check1(tmp);
            getline(check1, first, ' ');
            getline(check1, sec, ' ');
            subFunc(first, sec);
        }
        else if(tmp.substr(0, 3) == "inc"){
            tmp = tmp.substr(3, tmp.size()-3);
            stringstream check1(tmp);
            getline(check1, first, ' ');
            if (first == "ax") {
                *pax++;
            } else if (first == "bx") {
                *pbx++;
            } else if (first == "cx") {
                *pcx++;
            } else if (first == "dx") {
                *pdx++;
            } else if (first == "di") {
                *pdi++;
            } else if (first == "sp") {
                *psp++;
            } else if (first == "si") {
                *psi++;
            } else if (first == "bp") {
                *pbp++;
            } else if (first == "ah") {
                *pah++;
            } else if (first == "al") {
                *pal++;
            } else if (first == "bh") {
                *pbh++;
            } else if (first == "bl") {
                *pbl++;
            } else if (first == "ch") {
                *pch++;
            } else if (first == "cl") {
                *pcl++;
            } else if (first == "dh") {
                *pdh++;
            } else if (first == "dl") {
                *pdh++;
            }
        }
        else if(tmp.substr(0, 3) == "dec") {
            tmp = tmp.substr(3, tmp.size()-3);
            stringstream check1(tmp);
            getline(check1, first, ' ');
            if (first == "ax") {
                *pax--;
            } else if (first == "bx") {
                *pbx--;
            } else if (first == "cx") {
                *pcx--;
            } else if (first == "dx") {
                *pdx--;
            } else if (first == "di") {
                *pdi--;
            } else if (first == "sp") {
                *psp--;
            } else if (first == "si") {
                *psi--;
            } else if (first == "bp") {
                *pbp--;
            } else if (first == "ah") {
                *pah--;
            } else if (first == "al") {
                *pal--;
            } else if (first == "bh") {
                *pbh--;
            } else if (first == "bl") {
                *pbl--;
            } else if (first == "ch") {
                *pch--;
            } else if (first == "cl") {
                *pcl--;
            } else if (first == "dh") {
                *pdh--;
            } else if (first == "dl") {
                *pdh--;
            }
        }
        else if(tmp.substr(0, 3) == "mul") {
            tmp = tmp.substr(3, tmp.size()-3);
            stringstream check1(tmp);
            getline(check1, first, ' ');
            if(first == "ax"){
                multiplication(*pax);
                //bölümü dx e , kalanı ax e at
            } else if(first == "bx"){
                multiplication(*pbx);
            } else if(first == "cx"){
                multiplication(*pcx);
            } else if(first == "dx"){
                multiplication(*pdx);
            } else if(first == "di"){
                multiplication(*pdi);
            } else if(first == "sp"){
                multiplication(*psp);
            } else if(first == "si"){
                multiplication(*psi);
            } else if(first == "bp"){
                multiplication(*pbp);
            } else if(first == "ah"){
                multiplication(*pah);
            } else if(first == "al"){
                multiplication(*pal);
            } else if(first == "bh"){
                multiplication(*pbh);
            } else if(first == "bl"){
                multiplication(*pbl);
            } else if(first == "ch"){
                multiplication(*pch);
            } else if(first == "cl"){
                multiplication(*pcl);
            } else if(first == "dh"){
                multiplication(*pdh);
            } else if(first == "dl"){
                multiplication(*pdl);
            } else {
                if(first.at(0) == '[') first = first.substr(1, first.size()-2);
                else {
                    first = first.substr(2, first.size()-3);
                }
                if(first.at(first.size()-1) == 'h') {
                    first = first.substr(0, first.size()-1);
                    if((int) hex2dec(first) > 65535) {
                        cout << "ERROR" << endl;
                        isError = true;
                        return 0;
                    }
                    unsigned char *pnum = &memory[hex2dec(first)];
                    multiplication(*pnum);
                }
                else {
                    if(first.at(first.size()-1) == 'd') {
                        first = first.substr(0, first.size()-1);
                    }
                    if(stoi(first) > 65535) {
                        cout << "ERROR" << endl;
                        isError = true;
                        return 0;
                    }
                    unsigned char *pnum = &memory[stoi(first)];
                    multiplication(*pnum);
                }
            }
        }
        else if(tmp.substr(0, 3) == "div") {
            tmp = tmp.substr(3, tmp.size()-3);
            stringstream check1(tmp);
            getline(check1, first, ' ');
            if(first == "ax"){
                division(*pax);
            } else if(first == "bx"){
                division(*pbx);
            } else if(first == "cx"){
                division(*pcx);
            } else if(first == "dx"){
                division(*pdx);
            } else if(first == "di"){
                division(*pdi);
            } else if(first == "sp"){
                division(*psp);
            } else if(first == "si"){
                division(*psi);
            } else if(first == "bp"){
                division(*pbp);
            } else if(first == "ah"){
                division(*pah);
            } else if(first == "al"){
                division(*pal);
            } else if(first == "bh"){
                division(*pbh);
            } else if(first == "bl"){
                division(*pbl);
            } else if(first == "ch"){
                division(*pch);
            } else if(first == "cl"){
                division(*pcl);
            } else if(first == "dh"){
                division(*pdh);
            } else if(first == "dl"){
                division(*pdl);
            } else {
                if(first.at(0) == '[') first = first.substr(1, first.size()-2);
                else {
                    first = first.substr(2, first.size()-3);
                }
                if(first.at(first.size()-1) == 'h') {
                    first = first.substr(0, first.size()-1);
                    if((int) hex2dec(first) > 65535) {
                        cout << "ERROR" << endl;
                        isError = true;
                        return 0;
                    }
                    unsigned char *pnum = &memory[hex2dec(first)];
                    division(*pnum);
                }
                else {
                    if(first.at(first.size()-1) == 'd') {
                        first = first.substr(0, first.size()-1);
                    }
                    if(stoi(first) > 65535) {
                        cout << "ERROR" << endl;
                        isError = true;
                        return 0;
                    }
                    unsigned char *pnum = &memory[stoi(first)];
                    division(*pnum);
                }
            }
        }
        else if(tmp.substr(0, 3) == "xor") {
            tmp = tmp.substr(3, tmp.size()-3);
            stringstream check1(tmp);
            getline(check1, first, ' ');
            getline(check1, sec, ' ');
        }
        else if(tmp.substr(0, 2) == "or") {
            tmp = tmp.substr(2, tmp.size()-2);
            stringstream check1(tmp);
            getline(check1, first, ' ');
            getline(check1, sec, ' ');
        }
        else if(tmp.substr(0, 3) == "and") {
            tmp = tmp.substr(3, tmp.size()-3);
            stringstream check1(tmp);
            getline(check1, first, ' ');
            getline(check1, sec, ' ');
        }
        else if(tmp.substr(0, 3) == "not") {
            tmp = tmp.substr(3, tmp.size()-3);
            stringstream check1(tmp);
            getline(check1, first, ' ');
            getline(check1, sec, ' ');
        }
        else if(tmp.substr(0, 3) == "rcl") {
            tmp = tmp.substr(3, tmp.size()-3);
            stringstream check1(tmp);
            getline(check1, first, ' ');
            getline(check1, sec, ' ');
        }
        else if(tmp.substr(0, 3) == "rcr") {
            tmp = tmp.substr(3, tmp.size()-3);
            stringstream check1(tmp);
            getline(check1, first, ' ');
            getline(check1, sec, ' ');
        }
        else if(tmp.substr(0, 3) == "shl") {
            tmp = tmp.substr(3, tmp.size()-3);
            stringstream check1(tmp);
            getline(check1, first, ' ');
            getline(check1, sec, ' ');
        }
        else if(tmp.substr(0, 3) == "shr") {
            tmp = tmp.substr(3, tmp.size()-3);
            stringstream check1(tmp);
            getline(check1, first, ' ');
            getline(check1, sec, ' ');
        }
        else if(tmp.substr(0, 4) == "push") {
            tmp = tmp.substr(4, tmp.size()-4);
            stringstream check1(tmp);
            getline(check1, first, ' ');
            getline(check1, sec, ' ');
        }
        else if(tmp.substr(0, 3) == "pop") {
            tmp = tmp.substr(3, tmp.size()-3);
            stringstream check1(tmp);
            getline(check1, first, ' ');
            getline(check1, sec, ' ');
        }
        else if(tmp.substr(0, 3) == "nop");
        else if(tmp.substr(0, 3) == "cmp") {
            tmp = tmp.substr(3, tmp.size()-3);
            stringstream check1(tmp);
            getline(check1, first, ' ');
            getline(check1, sec, ' ');
        }
        else if(tmp.substr(0, 4) == "jnae") {
            tmp = tmp.substr(4, tmp.size()-4);
            stringstream check1(tmp);
            getline(check1, first, ' ');
            getline(check1, sec, ' ');
        }
        else if(tmp.substr(0, 4) == "jnbe") {
            tmp = tmp.substr(4, tmp.size()-4);
            stringstream check1(tmp);
            getline(check1, first, ' ');
            getline(check1, sec, ' ');
        }
        else if(tmp.substr(0, 3) == "jae") {
            tmp = tmp.substr(3, tmp.size()-3);
            stringstream check1(tmp);
            getline(check1, first, ' ');
            getline(check1, sec, ' ');
        }
        else if(tmp.substr(0, 3) == "jbe") {
            tmp = tmp.substr(3, tmp.size()-3);
            stringstream check1(tmp);
            getline(check1, first, ' ');
            getline(check1, sec, ' ');
        }
        else if(tmp.substr(0, 3) == "jnb") {
            tmp = tmp.substr(3, tmp.size()-3);
            stringstream check1(tmp);
            getline(check1, first, ' ');
            getline(check1, sec, ' ');
        }
        else if(tmp.substr(0, 3) == "jnc") {
            tmp = tmp.substr(3, tmp.size()-3);
            stringstream check1(tmp);
            getline(check1, first, ' ');
            getline(check1, sec, ' ');
        }
        else if(tmp.substr(0, 3) == "jne") {
            tmp = tmp.substr(3, tmp.size()-3);
            stringstream check1(tmp);
            getline(check1, first, ' ');
            getline(check1, sec, ' ');
        }
        else if(tmp.substr(0, 3) == "jnz") {
            tmp = tmp.substr(3, tmp.size()-3);
            stringstream check1(tmp);
            getline(check1, first, ' ');
            getline(check1, sec, ' ');
        }
        else if(tmp.substr(0, 2) == "ja") {
            tmp = tmp.substr(2, tmp.size()-2);
            stringstream check1(tmp);
            getline(check1, first, ' ');
            getline(check1, sec, ' ');
        }
        else if(tmp.substr(0, 2) == "jb") {
            tmp = tmp.substr(2, tmp.size()-2);
            stringstream check1(tmp);
            getline(check1, first, ' ');
            getline(check1, sec, ' ');
        }
        else if(tmp.substr(0, 2) == "jc") {
            tmp = tmp.substr(2, tmp.size()-2);
            stringstream check1(tmp);
            getline(check1, first, ' ');
            getline(check1, sec, ' ');
        }
        else if(tmp.substr(0, 2) == "jn") {
            tmp = tmp.substr(2, tmp.size()-2);
            stringstream check1(tmp);
            getline(check1, first, ' ');
            getline(check1, sec, ' ');
        }
        else if(tmp.substr(0, 2) == "jz") {
            tmp = tmp.substr(2, tmp.size()-2);
            stringstream check1(tmp);
            getline(check1, first, ' ');
            getline(check1, sec, ' ');
        }
        else if(tmp.substr(0, 6) == "int21h") {

        }
    }
    ax = 3 ;
    bx = 4 ;

    // 1. Read instruction lines
    // 2. Place  dw and db data  and compute addresses
    // 3.  PC = line 0
    // while (PC !=  int20line) {
    //     getline(PC)
    //     parse instruction in the line  to extract opcode and operands
    //     if (strcmp(opcode,"ADD) {
    //        .... determine operands and call appropriate add function
    //     }
    //     else if (strcmp(opcode,"MOV") ) {
    //         .. determine operands call  appropriate mov function
    //print_hex(*pah) ;
    //print_hex(*pal) ;
    //add_reg(pax, 8);
    //sub_reg(pbx, 2);
    //mul_reg(pbx);
    //div_reg(pbx);
    //mov_reg_reg(pax,pbx) ;
    //*pax = 50001 ;
    //     }
    //
    // }

    //print_16bitregs() ;
    print_hex(*pah) ;
    print_hex(*pal) ;
    add_reg(pah, 5);
    //not_reg(pah);
    xor_reg_reg(pal, pah);
    //mov_reg_reg(pbx, pax);
    print_bits(*pal);
    print_bits(*pah);
    //print_bits(*pax);
}

void addFunc(const string& first, const string& sec) {
    if (first == "ax") {
        firstParaNo_secParaNo_add(pax, sec);
    } else if (first == "bx") {
        firstParaNo_secParaNo_add(pbx, sec);
    } else if (first == "cx") {
        firstParaNo_secParaNo_add(pcx, sec);
    } else if (first == "dx") {
        firstParaNo_secParaNo_add(pdx, sec);
    } else if (first == "di") {
        firstParaNo_secParaNo_add(pdi, sec);
    } else if (first == "sp") {
        firstParaNo_secParaNo_add(psp, sec);
    } else if (first == "si") {
        firstParaNo_secParaNo_add(psi, sec);
    } else if (first == "bp") {
        firstParaNo_secParaNo_add(pbp, sec);
    } else if (first == "ah") {
        firstParaNo_secParaNo_add(pah, sec);
    } else if (first == "al") {
        firstParaNo_secParaNo_add(pal, sec);
    } else if (first == "bh") {
        firstParaNo_secParaNo_add(pbh, sec);
    } else if (first == "bl") {
        firstParaNo_secParaNo_add(pbl, sec);
    } else if (first == "ch") {
        firstParaNo_secParaNo_add(pch, sec);
    } else if (first == "cl") {
        firstParaNo_secParaNo_add(pcl, sec);
    } else if (first == "dh") {
        firstParaNo_secParaNo_add(pdh, sec);
    } else if (first == "dl") {
        firstParaNo_secParaNo_add(pdl, sec);
    }
}

void subFunc(const string& first, const string& sec) {
    if (first == "ax") {
        firstParaNo_secParaNo_sub(pax, sec);
    } else if (first == "bx") {
        firstParaNo_secParaNo_sub(pbx, sec);
    } else if (first == "cx") {
        firstParaNo_secParaNo_sub(pcx, sec);
    } else if (first == "dx") {
        firstParaNo_secParaNo_sub(pdx, sec);
    } else if (first == "di") {
        firstParaNo_secParaNo_sub(pdi, sec);
    } else if (first == "sp") {
        firstParaNo_secParaNo_sub(psp, sec);
    } else if (first == "si") {
        firstParaNo_secParaNo_sub(psi, sec);
    } else if (first == "bp") {
        firstParaNo_secParaNo_sub(pbp, sec);
    } else if (first == "ah") {
        firstParaNo_secParaNo_sub(pah, sec);
    } else if (first == "al") {
        firstParaNo_secParaNo_sub(pal, sec);
    } else if (first == "bh") {
        firstParaNo_secParaNo_sub(pbh, sec);
    } else if (first == "bl") {
        firstParaNo_secParaNo_sub(pbl, sec);
    } else if (first == "ch") {
        firstParaNo_secParaNo_sub(pch, sec);
    } else if (first == "cl") {
        firstParaNo_secParaNo_sub(pcl, sec);
    } else if (first == "dh") {
        firstParaNo_secParaNo_sub(pdh, sec);
    } else if (first == "dl") {
        firstParaNo_secParaNo_sub(pdl, sec);
    }
}

template  <class regtype>
void add_reg(regtype *preg, unsigned char c) {
    *preg += c;
}

template  <class regtype>
void sub_reg(regtype *preg, unsigned char c) {
    *preg -= c;
}

template <class regtype>
void inc_reg(regtype *preg) {
    *preg ++;
}

template <class regtype>
void dec_reg(regtype *preg) {
    *preg --;
}

template  <class regtype>
void mul_reg(regtype *preg) {
    ax = ax * (*preg);
}

template  <class regtype>
void div_reg(regtype *preg) {
    *pal = ax / (*preg);
    *pah = ax % (*preg);
}

template  <class regtype>
void or_reg_reg(regtype *preg1, regtype *preg2) {
    *preg1 = (*preg1 | *preg2);
}

template  <class regtype>
void and_reg_reg(regtype *preg1, regtype *preg2) {
    *preg1 = (*preg1 & *preg2);
}

template  <class regtype>
void xor_reg_reg(regtype *preg1, regtype *preg2) {
    *preg1 = ((~(*preg1) & (*preg2)) | (~(*preg2) & (*preg1)));
}

template  <class regtype>
void not_reg(regtype *preg) {
    *preg = ~(*preg);
}

/*
string erase(string s){
    bool a = false;
    for(int i = 0; i < s.size()-1; i++){
        if(s.at(i) == ' ' && !a){
            s.erase(s.begin() + i);
        }else
            break;
        if(s.at(i+1) != ' '){
            s.erase(s.begin() + i - 1);
            a = true;
            break;
        }
    }
    for(int i = s.size()-1; i > 0 ; i--){
        if(s.at(i) == ' '){
            s.erase(s.begin() + s.size()-i);
        } else
            break;
        if(s.at(i-1) != ' '){
            s.erase(s.begin() +  s.size() - i + 1);
            b = true;
            break;
        }
    }
    return s;
}
*/

void movFunc(string first, string &sec) {
    char desType;
    if(first.find('[') != string::npos && sec.find('[') == string::npos){
        //mov w[ax], bx
        desType = first.at(0);
        if(desType == '[') first = first.substr(1, first.size()-2);
        else first = first.substr(2, first.size()-3);
        if(first == "ax"){
            firstParaYes_secParaNo(pax, sec, desType);
        } else if(first == "bx"){
            firstParaYes_secParaNo(pbx, sec, desType);
        } else if(first == "cx"){
            firstParaYes_secParaNo(pcx, sec, desType);
        } else if(first == "dx"){
            firstParaYes_secParaNo(pdx, sec, desType);
        } else if(first == "di"){
            firstParaYes_secParaNo(pdi, sec, desType);
        } else if(first == "sp"){
            firstParaYes_secParaNo(psp, sec, desType);
        } else if(first == "si"){
            firstParaYes_secParaNo(psi, sec, desType);
        } else if(first == "bp"){
            firstParaYes_secParaNo(pbp, sec, desType);
        } else if(first == "ah"){
            firstParaYes_secParaNo(pah, sec, desType);
        } else if(first == "al"){
            firstParaYes_secParaNo(pal, sec, desType);
        } else if(first == "bh"){
            firstParaYes_secParaNo(pbh, sec, desType);
        } else if(first == "bl"){
            firstParaYes_secParaNo(pbl, sec, desType);
        } else if(first == "ch"){
            firstParaYes_secParaNo(pch, sec, desType);
        } else if(first == "cl"){
            firstParaYes_secParaNo(pcl, sec, desType);
        } else if(first == "dh"){
            firstParaYes_secParaNo(pdh, sec, desType);
        } else if(first == "dl"){
            firstParaYes_secParaNo(pdl, sec, desType);
        } else{
            if(first.at(first.size()-1) == 'h') {
                first = first.substr(0, first.size()-1);
                if((int) hex2dec(first) > 65535) {
                    cout << "ERROR" << endl;
                    isError = true;
                    return;
                }
                unsigned char *pnum = &memory[hex2dec(first)];
                firstParaYes_secParaNo(pnum, sec, desType);
            }
            else {
                if(first.at(first.size()-1) == 'd') {
                    first = first.substr(0, first.size()-1);
                }
                if(stoi(first) > 65535) {
                    cout << "ERROR" << endl;
                    isError = true;
                    return;
                }
                unsigned char *pnum = &memory[stoi(first)];
                firstParaYes_secParaNo(pnum, sec, desType);
            }
        }
        if(isError) return;
    }else if(first.find('[') == string::npos && sec.find('[') == string::npos){
        if (vars.count((first + "1")) || vars.count((first + "2"))) {
            auto it = vars.find(first + "1");
            if (it == vars.end()) it = vars.find(first + "2");
            unsigned char *pvar = &memory[it->second];
            firstParaNo_secParaNo_mov(pvar, sec);
        } else if (first == "ax") {
            firstParaNo_secParaNo_mov(pax, sec);
        } else if (first == "bx") {
            firstParaNo_secParaNo_mov(pbx, sec);
        } else if (first == "cx") {
            firstParaNo_secParaNo_mov(pcx, sec);
        } else if (first == "dx") {
            firstParaNo_secParaNo_mov(pdx, sec);
        } else if (first == "di") {
            firstParaNo_secParaNo_mov(pdi, sec);
        } else if (first == "sp") {
            firstParaNo_secParaNo_mov(psp, sec);
        } else if (first == "si") {
            firstParaNo_secParaNo_mov(psi, sec);
        } else if (first == "bp") {
            firstParaNo_secParaNo_mov(pbp, sec);
        } else if (first == "ah") {
            firstParaNo_secParaNo_mov(pah, sec);
        } else if (first == "al") {
            firstParaNo_secParaNo_mov(pal, sec);
        } else if (first == "bh") {
            firstParaNo_secParaNo_mov(pbh, sec);
        } else if (first == "bl") {
            firstParaNo_secParaNo_mov(pbl, sec);
        } else if (first == "ch") {
            firstParaNo_secParaNo_mov(pch, sec);
        } else if (first == "cl") {
            firstParaNo_secParaNo_mov(pcl, sec);
        } else if (first == "dh") {
            firstParaNo_secParaNo_mov(pdh, sec);
        } else if (first == "dl") {
            firstParaNo_secParaNo_mov(pdl, sec);
        }
    }else{
        //mov dl,[bx]
        if(sec.at(0) != '[') {
            desType = sec.at(0);
            sec = sec.substr(1, sec.size()-1);
        }
        sec = sec.substr(1, sec.size()-2);
        if (first == "ax") {
            firstParaNo_secParaYes(pax, sec);
        } else if (first == "bx") {
            firstParaNo_secParaYes(pbx, sec);
        } else if (first == "cx") {
            firstParaNo_secParaYes(pcx, sec);
        } else if (first == "dx") {
            firstParaNo_secParaYes(pdx, sec);
        } else if (first == "di") {
            firstParaNo_secParaYes(pdi, sec);
        } else if (first == "sp") {
            firstParaNo_secParaYes(psp, sec);
        } else if (first == "si") {
            firstParaNo_secParaYes(psi, sec);
        } else if (first == "bp") {
            firstParaNo_secParaYes(pbp, sec);
        } else if (first == "ah") {
            firstParaNo_secParaYes(pah, sec);
        } else if (first == "al") {
            firstParaNo_secParaYes(pal, sec);
        } else if (first == "bh") {
            firstParaNo_secParaYes(pbh, sec);
        } else if (first == "bl") {
            firstParaNo_secParaYes(pbl, sec);
        } else if (first == "ch") {
            firstParaNo_secParaYes(pch, sec);
        } else if (first == "cl") {
            firstParaNo_secParaYes(pcl, sec);
        } else if (first == "dh") {
            firstParaNo_secParaYes(pdh, sec);
        } else if (first == "dl") {
            firstParaNo_secParaYes(pdl, sec);
        }
    }
}
/*
bool check_number(string str) {
    for (int i = 0; i < str.length(); i++){
        if (isdigit(str[i]) == false)
            return false;
    }
    return true;
}
*/

template <class regtype>
void firstParaNo_secParaNo_add(regtype *first, string sec){
    if (vars.count((sec + "1")) || vars.count((sec + "2"))) {
        auto it = vars.find(sec + "1");
        if (it == vars.end())
            it = vars.find(sec + "2");
        *first += memory[it->second];
    }
    else if (sec.at(sec.size() - 1) == 'h') {
        string s = sec.substr(0, sec.size() - 1);
        *first += hex2dec(s);
    }
    else if (sec.at(sec.size() - 1) == '\'') {
        unsigned char temp = sec.at(sec.size()-2);
        *first += temp;
    }
    else {
        if (sec == "ax") {
            *first += ax;
        } else if (sec == "bx") {
            *first += bx;
        } else if (sec == "cx") {
            *first += cx;
        } else if (sec == "dx") {
            *first += dx;
        } else if (sec == "di") {
            *first += di;
        } else if (sec == "sp") {
            *first += sp;
        } else if (sec == "si") {
            *first += si;
        } else if (sec == "bp") {
            *first += bp;
        } else if (sec == "ah") {
            *first += *pah;
        } else if (sec == "al") {
            *first += *pal;
        } else if (sec == "bh") {
            *first += *pbh;
        } else if (sec == "bl") {
            *first += *pbl;
        } else if (sec == "ch") {
            *first += *pch;
        } else if (sec == "cl") {
            *first += *pcl;
        } else if (sec == "dh") {
            *first += *pdh;
        } else if (sec == "dl") {
            *first += *pdl;
        }
            //SADECE SAYI İSE
        else {
            if(sec.at(sec.size()-1) == 'd')
                *first = stoi(sec.substr(0,sec.size()-1));
            else
                *first = stoi(sec);
        }
    }
}

template <class regtype>
void firstParaNo_secParaNo_sub(regtype *first, string sec) {
    if (vars.count((sec + "1")) || vars.count((sec + "2"))) {
        auto it = vars.find(sec + "1");
        if (it == vars.end())
            it = vars.find(sec + "2");
        *first -= memory[it->second];
    }
    else if (sec.at(sec.size() - 1) == 'h') {
        string s = sec.substr(0, sec.size() - 1);
        *first -= hex2dec(s);
    }
    else if (sec.at(sec.size() - 1) == '\'') {
        unsigned char temp = sec.at(sec.size()-2);
        *first -= temp;
    }
    else {
        if (sec == "ax") {
            *first -= ax;
        } else if (sec == "bx") {
            *first -= bx;
        } else if (sec == "cx") {
            *first -= cx;
        } else if (sec == "dx") {
            *first -= dx;
        } else if (sec == "di") {
            *first -= di;
        } else if (sec == "sp") {
            *first -= sp;
        } else if (sec == "si") {
            *first -= si;
        } else if (sec == "bp") {
            *first -= bp;
        } else if (sec == "ah") {
            *first -= *pah;
        } else if (sec == "al") {
            *first -= *pal;
        } else if (sec == "bh") {
            *first -= *pbh;
        } else if (sec == "bl") {
            *first -= *pbl;
        } else if (sec == "ch") {
            *first -= *pch;
        } else if (sec == "cl") {
            *first -= *pcl;
        } else if (sec == "dh") {
            *first -= *pdh;
        } else if (sec == "dl") {
            *first -= *pdl;
        }
            //SADECE SAYI İSE
        else {
            if(sec.at(sec.size()-1) == 'd')
                *first -= stoi(sec.substr(0,sec.size()-1));
            else
                *first -= stoi(sec);
        }
    }
}

template <class regtype>
void xorF(regtype *first, string sec){
    if (vars.count((sec + "1")) || vars.count((sec + "2"))) {
        auto it = vars.find(sec + "1");
        if (it == vars.end())
            it = vars.find(sec + "2");
        *first += memory[it->second];
    }
    else if (sec.at(sec.size() - 1) == 'h') {
        string s = sec.substr(0, sec.size() - 1);
        *first += hex2dec(s);
    }
    else if (sec.at(sec.size() - 1) == '\'') {
        unsigned char temp = sec.at(sec.size()-2);
        *first += temp;
    }
    else {
        if (sec == "ax") {
            *first += ax;
        } else if (sec == "bx") {
            *first += bx;
        } else if (sec == "cx") {
            *first += cx;
        } else if (sec == "dx") {
            *first += dx;
        } else if (sec == "di") {
            *first += di;
        } else if (sec == "sp") {
            *first += sp;
        } else if (sec == "si") {
            *first += si;
        } else if (sec == "bp") {
            *first += bp;
        } else if (sec == "ah") {
            *first += *pah;
        } else if (sec == "al") {
            *first += *pal;
        } else if (sec == "bh") {
            *first += *pbh;
        } else if (sec == "bl") {
            *first += *pbl;
        } else if (sec == "ch") {
            *first += *pch;
        } else if (sec == "cl") {
            *first += *pcl;
        } else if (sec == "dh") {
            *first += *pdh;
        } else if (sec == "dl") {
            *first += *pdl;
        }
            //SADECE SAYI İSE
        else {
            if(sec.at(sec.size()-1) == 'd')
                *first += stoi(sec.substr(0,sec.size()-1));
            else
                *first += stoi(sec);
        }
    }
}

template <class regtype>
void firstParaNo_secParaNo_mov(regtype *first, string sec){
    //todo iki parametrenin sizelarını kıyasla
    if (vars.count((sec + "1")) || vars.count((sec + "2"))) {
        auto it = vars.find(sec + "1");
        if (it == vars.end())
            it = vars.find(sec + "2");
        /*if(sizeof(*first) != stoi(it->first.substr(it->first.size()-1))) {
            cout << "ERROR" << endl;
            isError = true;
            return;
        }*/
        *first = memory[it->second];
    } else if (sec.at(sec.size() - 1) == 'h') {
        string s = sec.substr(0, sec.size() - 1);
        /*if(sizeof(*first) != sizeof(hex2dec(s))) {
            cout << "ERROR" << endl;
            isError = true;
            return;
        }*/
        *first = hex2dec(s);
    } else {
        if (sec == "ax") {
            if(sizeof(*first) == 1) {
                cout << "ERROR" << endl;
                isError = true;
                return;
            }
            *first = ax;
        } else if (sec == "bx") {
            *first = bx;
        } else if (sec == "cx") {
            *first = cx;
        } else if (sec == "dx") {
            *first = dx;
        } else if (sec == "di") {
            *first = di;
        } else if (sec == "sp") {
            *first = sp;
        } else if (sec == "si") {
            *first = si;
        } else if (sec == "bp") {
            *first = bp;
        } else if (sec == "ah") {
            if(sizeof(*first) == 2) {
                cout << "ERROR" << endl;
                isError = true;
                return;
            }
            *first = *pah;
        } else if (sec == "al") {
            *first = *pal;
        } else if (sec == "bh") {
            *first = *pbh;
        } else if (sec == "bl") {
            *first = *pbl;
        } else if (sec == "ch") {
            *first = *pch;
        } else if (sec == "cl") {
            *first = *pcl;
        } else if (sec == "dh") {
            *first = *pdh;
        } else if (sec == "dl") {
            *first = *pdl;
        }
            //SADECE SAYI İSE
        else {
            if(sec.at(sec.size()-1) == 'd') sec = sec.substr(0,sec.size()-1);
            /*if(sizeof(*first) != sizeof(stoi(sec))) {
                cout << "ERROR" << endl;
                isError = true;
                return;
            }*/
            *first = stoi(sec);
        }
    }
}

template <class regtype>
void firstParaYes_secParaNo(regtype *first,string sec, char desType){
    if(*first > 65535) {
        cout << "ERROR" << endl;
        isError = true;
        return;
    }
    if (vars.count((sec + "1")) || vars.count((sec + "2"))) {
        auto it = vars.find(sec + "1");
        if (it == vars.end())
            it = vars.find(sec + "2");
        memory[*first] = it->second;
    }
    else if (sec.at(sec.size() - 1) == 'h') {
        if(sec.at(0) == '0' && sec.size() > 2) sec = sec.substr(1, sec.size()-1);
        string num = sec.substr(0, sec.size() - 1);
        if(num.size() == 1 || num.size() == 2) {
            memory[*first] = hex2dec(num);
            if(desType == 'w')
            memory[*first+1] = 0;
        }
        else if(num.size() == 3 || num.size() == 4) {
            if(desType == 'b') {
                cout << "ERROR" << endl;
                isError = true;
                return;
            }
            else {
                memory[*first] = hex2dec(num.substr(num.size() - 2, 2));
                memory[*first + 1] = hex2dec(num.substr(0, num.size() - 2));
            }
        }
        else {
            cout << "ERROR" << endl;
            isError = true;
            return;
        }
        // buraya şu şekilde atılacak -> 12c4h ise ch ı memory[first] , 12 yi memory[first+1]
    }
    else{
        if (sec == "ax") {
            hexToArray(memory, ax, *first);
        } else if (sec == "bx") {
            hexToArray(memory, bx, *first);
        } else if (sec == "cx") {
            hexToArray(memory, cx, *first);
        } else if (sec == "dx") {
            hexToArray(memory, dx, *first);
        } else if (sec == "di") {
            hexToArray(memory, di, *first);
        } else if (sec == "sp") {
            hexToArray(memory, sp, *first);
        } else if (sec == "si") {
            hexToArray(memory, si, *first);
        } else if (sec == "bp") {
            hexToArray(memory, bp, *first);
        } else if (sec == "ah") {
            hexToArray(memory, *pah, *first);
        } else if (sec == "al") {
            hexToArray(memory, *pal, *first);
        } else if (sec == "bh") {
            hexToArray(memory, *pbh, *first);
        } else if (sec == "bl") {
            hexToArray(memory, *pbl, *first);
        } else if (sec == "ch") {
            hexToArray(memory, *pch, *first);
        } else if (sec == "cl") {
            hexToArray(memory, *pcl, *first);
        } else if (sec == "dh") {
            hexToArray(memory, *pdh, *first);
        } else if (sec == "dl") {
            hexToArray(memory, *pdl, *first);
        }
        else {
            if(sec.at(sec.size()-1) == 'd')
                sec = sec.substr(0,sec.size() - 1);
            memory[*first] = stoi(sec);
        }
    }
    
}

template <class regtype>
void firstParaNo_secParaYes(regtype *first ,string &sec){
    if (sec.at(sec.size() - 1) == 'h') {
        sec = sec.substr(0, sec.size() - 1);
        *first = memory[hex2dec(sec)];
    } else {
        if (sec == "ax") {
            *first = memory[ax]*256 + memory[ax+1];
        } else if (sec == "bx") {
            *first = memory[bx]*256 + memory[bx+1];
        } else if (sec == "cx") {
            *first = memory[cx]*256 + memory[cx+1];
        } else if (sec == "dx") {
            *first = memory[dx]*256 + memory[dx+1];
        } else if (sec == "di") {
            *first = memory[di]*256 + memory[di+1];
        } else if (sec == "sp") {
            *first = memory[sp]*256 + memory[sp+1];
        } else if (sec == "si") {
            *first = memory[si]*256 + memory[si+1];
        } else if (sec == "bp") {
            *first = memory[bp]*256 + memory[bp+1];
        } else if (sec == "ah") {
            *first = memory[*pah];
        } else if (sec == "al") {
            *first = memory[*pal];
        } else if (sec == "bh") {
            *first = memory[*pbh];
        } else if (sec == "bl") {
            *first = memory[*pbl];
        } else if (sec == "ch") {
            *first = memory[*pch];
        } else if (sec == "cl") {
            *first = memory[*pcl];
        } else if (sec == "dh") {
            *first = memory[*pdh];
        } else if (sec == "dl") {
            *first = memory[*pdl];
        }
            //SADECE SAYI İSE
        else {
            if(sec.at(sec.size()-1) == 'd')
                sec = sec.substr(0,sec.size() - 1);
            if(stoi(sec) > 65535) {
                cout << "ERROR" << endl;
                isError = true;
                return;
            }
            *first = memory[stoi(sec)];
        }
    }
}

void editStr(string &s) {
    s.erase(remove(s.begin(),s.end(),' '),s.end());
    replace(s.begin(), s.end(), ',', ' ');
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
        unsigned char *rep = (unsigned char *)&tmp;
        arr[index] = rep[0];
    }
    else{
        unsigned char *rep = (unsigned char *)&tmp;
        arr[index] = rep[1];
        arr[index+1] = rep[0];
    }
}

template <class datatype>
void multiplication(datatype y){
    int a = pow(2, 16);
    if (sizeof(y) == 1){
        int tmp = (*pal)*(y);
        *pal = tmp % a;
        *pah = tmp / a;
    }
    else{
        int tmp = ax*y;
        ax = tmp % a;
        dx = tmp / a;
    }
}

template <class datatype>
void division(datatype y){
    *pal = ax / y;
    *pah = ax % y;
}

template <class datatype>
void print_hex(datatype x)
{
    if (sizeof(x) == 1)
        printf("%02x\n",x);
    else
        printf("%04x\n",x);
}

template <class datatype>
unsigned char hex2dec(datatype hex)
{
    unsigned char result = 0;
    for (int i=0; i<hex.length(); i++) {
        if (hex[i]>=48 && hex[i]<=57){
            result += (hex[i]-48)*pow(16,hex.length()-i-1);
        } else if (hex[i]>=65 && hex[i]<=70) {
            result += (hex[i]-55)*pow(16,hex.length( )-i-1);
        } else if (hex[i]>=97 && hex[i]<=102) {
            result += (hex[i]-87)*pow(16,hex.length()-i-1);
        }
    }
    return result;
}

void print_16bitregs()
{
    printf("AX:%04x\n",ax);
    printf("BX:%04x\n",bx);
    printf("CX:%04x\n",cx);
    printf("DX:%04x\n",dx);
    printf("BP:%04x\n",bp);
    printf("SP:%04x\n",sp);
    printf("SI:%04x\n",si);
    printf("DI:%04x\n",di);
}

