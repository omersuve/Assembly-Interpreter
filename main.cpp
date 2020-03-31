#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <sstream>

using namespace std;

// prototypes
template <class datatype> void print_bits(datatype x) ;
template <class datatype> void print_hex(datatype x) ;
template <class regtype>  void mov_reg_reg(regtype *preg1,regtype *preg2)  ;
template <class regtype>  void add_reg(regtype *preg, unsigned char c)  ;
template <class regtype>  void sub_reg(regtype *preg, unsigned char c)  ;
template <class regtype>  void not_reg(regtype *preg)  ;
template <class regtype>  void or_reg_reg(regtype *preg1, regtype *preg2)  ;
template <class regtype>  void xor_reg_reg(regtype *preg1, regtype *preg2)  ;
template  <class regtype> void and_reg_reg(regtype *preg1, regtype *preg2)  ;
template <class regtype>  void mul_reg(regtype *preg)  ;
template <class regtype>  void div_reg(regtype *preg)  ;
void print_16bitregs() ;

// global variables ( memory, registers and flags )
unsigned char memory[2<<15];    // 64K memory
vector<string> lines;
unsigned short ax = 0 ;
unsigned short bx = 0 ;
unsigned short cx = 0 ;
unsigned short dx = 0 ;
unsigned short PC = 0 ;
unsigned short di = 0 ;
unsigned short bp = 0 ;
unsigned short si = 0 ;
unsigned short sp = (2<<15)-2 ;

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

    ifstream infile(argv[1]);
    // BURASI EN BAŞTA VARIABLE LARI INSTRUCTIONLARI VE LABEL LARI OKUMAK İÇİN
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
    }
    for(int i = 0; i < codelines.size(); i++){
        if(codelines[i].substr(0,3) == "mov"){
            memoryIdx += 6;
        }
        else if(codelines[i] == "int20h"){
            memoryIdx += 6;
            for(int j = i+1; j < codelines.size(); j++){
                string tmp = codelines[j];
                string var;
                string type;
                string info;
                unsigned char data;
                stringstream check1(tmp);
                getline(check1, var, ' ');
                getline(check1, type, ' ');
                getline(check1, info, ' ');
                vars.insert({var, memoryIdx});
                if(type == "db"){
                    data = info;
                    memory[memoryIdx] = data;
                    memoryIdx++;
                }else if(type == "dw"){
                    data = info;
                    memory[memoryIdx] = data;
                    memoryIdx+=2;
                }else{

                }
                //memorye ASCII kodu atılacak
                //Scanner olarak al
            }
        }
        cout << codelines[i] << '\n';
    }

    //ASIL KOD BURADAN BAŞLIYOR
    for(int i = 0; i < codelines.size(); i++){
        if(codelines[i].substr(0,3) == "mov") {
            string tmp1 = codelines[i].substr(4, 2);
            string tmp2 = codelines[i].substr(8, 2);
            mov_reg_reg(&tmp1, &tmp2);
        }
        else if(codelines[i].substr(0,3) == "sub"){
            
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
void mov_reg_reg(regtype *preg1,regtype *preg2)
{
    *preg1 = *preg2 ;
}

template  <class regtype>
void add_reg(regtype *preg, unsigned char c)
{
    *preg = *preg + c;
}

template  <class regtype>
void sub_reg(regtype *preg, unsigned char c)
{
    *preg = *preg - c;
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

