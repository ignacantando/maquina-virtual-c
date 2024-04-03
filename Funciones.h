
#define MAX_M 16384



typedef struct{
    short int base,size;
}tds; //tabla de descriptores de segmento

typedef struct{
    char TablaMemoria[MAX_M];
    long int TablaRegistros[16];
    tds TablaDeSegmentos[2];
}tmv; //tipo de maquina virtual


unsigned int memory=16384;





typedef void Funciones2op(long int op1,char top1,long int valor,tmv *tmv);
void MOV(long int op1,char top1,long int valor,tmv *tmv);
void ADD(long int op1,char top1,long int valor,tmv *tmv);
void SUB(long int op1,char top1,long int valor,tmv *tmv);
void SWAP(long int op1,long int op2,char top1,char top2,tmv *tmv);
void MUL(long int op1,char top1,long int valor,tmv *tmv);
void DIV(long int op1,char top1,long int valor,tmv *tmv);
void CMP(long int op1,char top1,long int valor,tmv *tmv);
void SHL(long int op1,char top1,long int valor2,tmv *tmv);
void SHR(long int op1,char top1,long int valor2,tmv *tmv);
void AND(long int op1,char top1,long int valor2,tmv *tmv);
void OR(long int op1,char top1,long int valor2,tmv *tmv);
void XOR(long int op1,char top1,long int valor2,tmv *tmv);
void RND(long int op1,char top1,long int valor2,tmv *tmv); //cambio
Funciones2op *funciones[]={MOV,ADD,SUB,0,MUL,DIV,CMP,SHL,SHR,AND,OR,XOR,RND};

typedef void Func_1op_tipo2(long int valor,tmv *tmv);
void JMP(long int valor,tmv *tmv);
void JZ(long int valor,tmv *tmv);
void JP(long int valor,tmv *tmv);
void JN(long int valor,tmv *tmv);
void JNZ(long int valor,tmv *tmv);
void JNP(long int valor,tmv *tmv);
void JNN(long int valor,tmv *tmv);
void LDH(long int valor,tmv *tmv);
void LDL(long int valor,tmv *tmv);
Func_1op_tipo2 *tipo2[]={0,JMP,JZ,JP,JN,JNZ,JNP,JNN,LDL,LDH};

typedef void Func_1op_tipo1(long int op,char top,tmv *tmv);
void NOT(long int op,char top,tmv *tmv);
void SYS(long int op,char top,tmv *tmv);
//void POP(long int op,char top,tmv *tmv);
Func_1op_tipo1 *tipo1[]={SYS,0,0,0,0,0,0,0,0,0,0,NOT,0};









void Pre_Funcion(short int cant,long int op1,long int op2,char top1,char top2,char operacion,tmv *tmv){
    long int valor;
    if (cant==2){
        if (operacion!=3){
            valor=Valor_op(op2,top2,*tmv);
            funciones[operacion](op1,top1,valor,tmv);
        }else
            SWAP(op1,op2,top1,top2,tmv);
    }else{
        if (cant==0)
            RET(tmv);
        else
            if (operacion<=10 && operacion>=1 || operacion==12 || operacion==14){
                valor=Valor_op(op1,top1,*tmv);
                tipo2[operacion](valor,tmv);
            }else
                tipo1[operacion](op1,top1,tmv);
    }
}









long int Valor_op(long int op,char top,tmv mv){
    long int tiporeg=op>>4;              //aca lo hace por que si esta en un registro,lo corres 4 bites y los 2 primeros que te quedan son la cantidad que agarras de esos 4 bytes, EAX(4 bytes) AL(1 byte)
    long int resultado=0;
    if (top==2){
            if (tiporeg==3)
                resultado=(short int)(mv.TablaRegistros[(op&0x0F)]&0x0000FFFF);
            if (tiporeg==2)
                resultado=(char)((mv.TablaRegistros[(op&0x0F)]&0x0000FF00)>>8);
            if (tiporeg==1)
                resultado=(char)(mv.TablaRegistros[(op&0x0F)]&0x000000FF);
            if (tiporeg==0)
                resultado=mv.TablaRegistros[(op&0x0F)];
    }
    if (top==0){
       resultado=Valor_mem(op,mv);
    }
    if (top==1){
        resultado=(short int)op;
    }
    return (resultado);
}
long int Valor_mem(long int op,tmv mv){
    long int resultado=0;
    long int pos_memo=puntero_memo(op,mv);
    unsigned short int cant=celdas_memo(op);
    for (int i=0;i<cant;i++){
        resultado+=(mv.TablaMemoria[pos_memo+i])&0x000000FF;
        if (i!=cant-1)
            resultado=resultado<<8;
    }
    long int aux=resultado;
    if (cant==1)
        resultado=(char)aux;
    if (cant==2)
        resultado=(short int)aux;
    return resultado;

}