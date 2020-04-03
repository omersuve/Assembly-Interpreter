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
template <class regtype>  void mov_reg_reg(regtype *preg1,regtype *preg2)  ;
template <class regtype>  void mov_reg_offset(regtype *preg1, int a)  ;
template <class regtype>  void add_reg(regtype *preg, unsigned char c)  ;
template <class regtype>  void sub_reg(regtype *preg, unsigned char c)  ;
template <class regtype>  void not_reg(regtype *preg)  ;
template <class regtype>  void or_reg_reg(regtype *preg1, regtype *preg2)  ;
template <class regtype>  void xor_reg_reg(regtype *preg1, regtype *preg2)  ;
template  <class regtype> void and_reg_reg(regtype *preg1, regtype *preg2)  ;
template <class regtype>  void mul_reg(regtype *preg)  ;
template <class regtype>  void div_reg(regtype *preg)  ;
unsigned char hex2dec(string hex);
template <class regtype>  void inc_reg(regtype *preg);
template <class regtype>  void dec_reg(regtype *preg);
void print_16bitregs() ;
void movFunc(string f, string s) ;
string erase(string s);

// global variables ( memory, registers and flags )
unsigned char memory[2<<15];    // 64K memory
vector<string> lines;
unsigned short ax = 0 ;
unsigned short bx = 0 ;
unsigned short cx = 0 ;
unsigned short dx = 0 ;
unsigned short di = 0 ;
unsigned short bp = 0 ;
unsigned short si = 0 ;
unsigned short sp = (2<<15)-2 ;
unsigned char ah = 0 ;
unsigned char al = 0 ;
unsigned char bh = 0 ;
unsigned char bl = 0 ;
unsigned char ch = 0 ;
unsigned char cl = 0 ;
unsigned char dh = 0 ;
unsigned char dl = 0 ;

bool     zf       ;              // zero flag
bool     sf       ;              // sign flag
bool     af       ;              // auxillary flag
bool     of       ;              // overflow flag
bool     cf       ;              // carry flag

// initialize pointers
unsigned short *pax = &ax ;
unsigned short *pbx = &bx ;
unsigned short *pcx = &cx ;
unsigned short *pdx = &dx ;
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
    map <string, int> labels;
    map <string, int> vars;
    vector<string> codelines;
    int memoryIdx = 0;

    string s = "66";
    cout << hex2dec(s) << endl;


    ifstream infile(argv[1]);
    string line = "";
    while(getline(infile, line)){
        lines.push_back(erase(line));
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

    for(int i = 0; i < lines.size(); i++){
        for(int j = 0; j < lines[i].size(); j++){
            if(lines[i].at(j) == ','){
                lines[i][j] = ' ';
            }
        }
    }


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
    while(codelines[i] != "int 20h") {
        if(codelines[i].substr(0,3) == "mov"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,3) == "add"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,3) == "dec"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,3) == "inc"){
            memoryIdx += 6;
        }
        else if(codelines[i].substr(0,3) == "sub"){
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
        else if(codelines[i] == "int 21h"){
            memoryIdx += 6;
        }
        else{
            i++;
            continue;
        }
        i++;
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
            cout << "error" << endl;
        }
    }

    //ASIL KOD BURADAN BAŞLIYOR
    for(int i = 0; i < codelines.size(); i++){
        string tmp = codelines[i];
        string type;
        string first;
        string sec;
        string var = "";
        stringstream check1(tmp);
        getline(check1, type, ' ');
        if(type == "mov") {
            getline(check1, first, ' ');
            getline(check1, sec, ' ');
            getline(check1, var, ' ');
            if(var != ""){
                //OFFSET İSE
                //TODO OFFSET İSE FİRST'DE KÖŞELİ PARANTEZ OLABİLİR
                map<string, int>::iterator it ;
                it = vars.find(var);
                if(it == vars.end())
                    cout << "Key-value pair not present in map" << endl;
                else {
                    if(first == "ax"){
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
                        ah = it->second;
                    }
                    else if(first == "al"){
                        al = it->second;
                    }
                    else if(first == "bh"){
                        bh = it->second;
                    }
                    else if(first == "bl"){
                        bl = it->second;
                    }
                    else if(first == "ch"){
                        ch = it->second;
                    }
                    else if(first == "cl"){
                        cl = it->second;
                    }
                    else if(first == "dh"){
                        dh = it->second;
                    }
                    else if(first == "dl"){
                        dl = it->second;
                    }

                }
            }else {
                if (first.find('[') != string::npos) {
                    char desType = first.at(0);
                    //TODO SECOND'DA KÖŞELİ VAR MI DİYE BAK
                    first = first.substr(2, 4);
                    if (vars.count((sec + "1")) || vars.count((sec + "2"))) {
                        map<string, int>::iterator it;
                        it = vars.find(sec);
                        if (it != vars.end())
                            ax = memory[it->second];
                    }
                    else if (sec.at(sec.size() - 1) == 'h') {
                        var = sec.substr(0, var.size() - 1);
                        if (desType == 'b' && var.size() > 2) {
                            cout << "ERROR" << endl;
                            break;
                        }
                        if (desType == 'w' && var.size() < 4) {
                            cout << "ERROR" << endl;
                            break;
                        }
                        if (first == "ax") {
                            memory[ax] = hex2dec(var.substr(0, 2));
                            memory[ax + 1] = hex2dec(var.substr(2, 4));
                        } else if (first == "bx") {
                            memory[bx] = hex2dec(var.substr(0, 2));
                            memory[bx + 1] = hex2dec(var.substr(2, 4));
                        } else if (first == "cx") {
                            memory[cx] = hex2dec(var.substr(0, 2));
                            memory[cx + 1] = hex2dec(var.substr(2, 4));
                        } else if (first == "dx") {
                            memory[dx] = hex2dec(var.substr(0, 2));
                            memory[dx + 1] = hex2dec(var.substr(2, 4));
                        } else if (first == "di") {
                            memory[di] = hex2dec(var.substr(0, 2));
                            memory[di + 1] = hex2dec(var.substr(2, 4));
                        } else if (first == "sp") {
                            memory[sp] = hex2dec(var.substr(0, 2));
                            memory[sp + 1] = hex2dec(var.substr(2, 4));
                        } else if (first == "si") {
                            memory[si] = hex2dec(var.substr(0, 2));
                            memory[si + 1] = hex2dec(var.substr(2, 4));
                        } else if (first == "bp") {
                            memory[bp] = hex2dec(var.substr(0, 2));
                            memory[bp + 1] = hex2dec(var.substr(2, 4));
                        } else if (first == "ah") {
                            memory[ah] = hex2dec(var);
                        } else if (first == "al") {
                            memory[al] = hex2dec(var);
                        } else if (first == "bh") {
                            memory[bh] = hex2dec(var);
                        } else if (first == "bl") {
                            memory[bl] = hex2dec(var);
                        } else if (first == "ch") {
                            memory[ch] = hex2dec(var);
                        } else if (first == "cl") {
                            memory[cl] = hex2dec(var);
                        } else if (first == "dh") {
                            memory[dh] = hex2dec(var);
                        } else if (first == "dl") {
                            memory[dl] = hex2dec(var);
                        }
                    }
                } else {
                    //TODO SECOND'DA KÖŞELİ VAR MI DİYE BAK
                    //TODO ALTTAKİ İF İ FONKSİYON YAP
                    if (first == "ax") {
                        if (vars.count((sec + "1")) || vars.count((sec + "2"))) {
                            map<string, int>::iterator it;
                            it = vars.find(sec);
                            if (it != vars.end())
                                ax = memory[it->second];
                        } else if (sec.at(sec.size() - 1) == 'h') {
                            string s = sec.substr(0, sec.size() - 1);
                            ax = hex2dec(s);
                        } else {
                            if (sec == "bx") {
                                mov_reg_reg(pax, pbx);
                            } else if (sec == "cx") {
                                mov_reg_reg(pax, pcx);
                            }
                                //SADECE SAYI İSE
                            else {
                               ax = stoi(sec);
                            }
                        }
                    } else if (first == "bx") {

                    }
                    //TODO BURADAKİ ELSE-İF GEREKSİZ ŞUANLIK
                    else if (first == "w[bx]") {
                        if (sec.at(sec.size() - 1) == 'h') {
                            string s1 = sec.substr(0, 2);
                            string s2 = sec.substr(2, 4);
                            int tmp = bx;
                            memory[tmp] = hex2dec(s1);
                            memory[tmp + 1] = hex2dec(s2);
                        }
                    } else {
                    }
                }
            }
            //VAR IN MEMORYDEKİ ADRESİNİ DÖNMESİ GEREK
        }
        else if(type == "add"){
            getline(check1, first, ' ');
            getline(check1, sec, ' ');
            unsigned char c = hex2dec(sec.substr(0,sec.size()-1));
            add_reg(first.c_str(),c);
        }
        else if(type == "inc"){
            getline(check1, first, ' ');
            inc_reg(first.c_str());
        }
        else if(type == "dec"){
            getline(check1, first, ' ');
            dec_reg(first.c_str());
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

template <class regtype>
void mov_reg_reg(regtype *preg1, regtype *preg2)
{
    *preg1 = *preg2 ;
}

template <class regtype>
void mov_reg_offset(regtype *preg1, int a)
{
    *preg1 = a ;
}

template  <class regtype>
void add_reg(regtype *preg, unsigned char c)
{
    preg = preg + c;
}

template  <class regtype>
void sub_reg(regtype *preg, unsigned char c)
{
    *preg = *preg - c;
}

template <class regtype>  void inc_reg(regtype *preg){
    *preg ++;
}

template <class regtype>  void dec_reg(regtype *preg){
    *preg --;
}

template  <class regtype>
void mul_reg(regtype *preg)
{
    ax = ax * (*preg);
}

template  <class regtype>
void div_reg(regtype *preg)
{
    *pal = ax / (*preg);
    *pah = ax % (*preg);
}
template  <class regtype>
void or_reg_reg(regtype *preg1, regtype *preg2)
{
    *preg1 = (*preg1 | *preg2);
}

template  <class regtype>
void and_reg_reg(regtype *preg1, regtype *preg2)
{
    *preg1 = (*preg1 & *preg2);
}

template  <class regtype>
void xor_reg_reg(regtype *preg1, regtype *preg2)
{
    *preg1 = ((~(*preg1) & (*preg2)) | (~(*preg2) & (*preg1)));
}

template  <class regtype>
void not_reg(regtype *preg)
{
    *preg = ~(*preg);
}

string erase(string s){
    bool a = false;
    for(int i = 0; i < s.size()-1; i++){
        if(s.at(i) == ' ' && a == false){
            s.erase(s.begin() + i);
        }else
            break;
        if(s.at(i+1) != ' '){
            s.erase(s.begin() + i - 1);
            a = true;
            break;
        }
    }
    bool b = false;
    for(int i = s.size()-1; i > 0 ; i--){
        if(s.at(i) == ' ' && b == false){
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

void movFunc(string f, string s){
    if(f.find('[') == string::npos && s.find('[') == string::npos){

    }else if(f.find('[') != string::npos && s.find('[') == string::npos){

    }else if(f.find('[') == string::npos && s.find('[') != string::npos){

    }else{

    }
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
void print_hex(datatype x)
{
    if (sizeof(x) == 1)
        printf("%02x\n",x);
    else
        printf("%04x\n",x);
}

unsigned char hex2dec(string hex)
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

