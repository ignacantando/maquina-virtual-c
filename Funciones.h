
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
            valor=Valor_op(op2,top2,*tmv);          //Como sabe que solo necesita el valor de el operando 2 va a buscar el valor, el op1 lo necesitamos por que muchas veces guardamos ahi
            funciones[operacion](op1,top1,valor,tmv);
        }else
            SWAP(op1,op2,top1,top2,tmv); //aca necesitamos los dos
    }else{
        if (cant==0)
            RET(tmv);  //stop
        else
            if (operacion<=10 && operacion>=1 || operacion==12 || operacion==14){ //en las de tipo dos no necesitamos la direccion del op1
                valor=Valor_op(op1,top1,*tmv);
                tipo2[operacion](valor,tmv);
            }else
                tipo1[operacion](op1,top1,tmv);  //aca si la necesitamos
    }
}



long int Valor_op(long int op,char top,tmv mv){
    long int tiporeg=op>>4;        //aca lo hace por que si esta en un registro,lo corres 4 bites y los 2 primeros que te quedan son la cantidad que agarras de esos 4 bytes, EAX(4 bytes) AL(1 byte)
    long int resultado=0;
    if (top==2){
            if (tiporeg==3)
                resultado=(short int)(mv.TablaRegistros[(op&0x0F)]&0x0000FFFF);    //AX  primeros 16 bits
            if (tiporeg==2)
                resultado=(char)((mv.TablaRegistros[(op&0x0F)]&0x0000FF00)>>8);    //AH  
            if (tiporeg==1)
                resultado=(char)(mv.TablaRegistros[(op&0x0F)]&0x000000FF);          //AL
            if (tiporeg==0) 
                resultado=mv.TablaRegistros[(op&0x0F)];     //EAX
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
    resultado=(mv.TablaMemoria[pos_memo])&0x000000FF;
    return resultado;
}

long int puntero_memo(long int op,tmv mv){
    short int offset=(op&0x0000FFFF);     //primeros 2 bytes para el offset
    int treg=(op&0x000F0000)>>16;         //4 bits que te da que segmento queres apuntar
    long int reg=mv.TablaRegistros[treg]; //si es 0 elige donde apunta el CS y el 1 seria el DS
    long int reg_pointer=reg>>16;         //puntero al registro, ya sea CS o DS  aca lo que sacamos es la base(direccion fisica)
    short int reg_offset=(reg&0x0000FFFF);//offset del registro (tamanio)
    return mv.TablaDeSegmentos[reg_pointer].base+reg_offset+offset;//
}


long int Valor_reg(char tipo,long int op,tmv mv){
    long int resultado=0;
    if (tipo==0)
        resultado=mv.TablaRegistros[(op&0x0F)];
    if (tipo==1)
        resultado=mv.TablaRegistros[(op&0x0F)]&0x000000FF;
    if (tipo==2)
        resultado=(mv.TablaRegistros[(op&0x0F)]&0x0000FF00)>>8;
    if (tipo==3)
        resultado=mv.TablaRegistros[(op&0x0F)]&0x0000FFFF;
    return resultado;
}
long int Suma_reg(char tipo,long int op,tmv mv){
    long int resultado=0;
    if (tipo==1)
        resultado=mv.TablaRegistros[(op&0x0F)]&0xFFFFFF00;
    if (tipo==2)
        resultado=mv.TablaRegistros[(op&0x0F)]&0xFFFF00FF;
    if (tipo==3)
        resultado=mv.TablaRegistros[(op&0x0F)]&0xFFFF0000;
    return resultado;
}
long int Mascara_registro(long int valor, char tipo){
    long int resultado=0;
    if (tipo==0)                      //EAX
        resultado=valor;
    if (tipo==1)
        resultado=valor&0x000000FF;    //AL
    if (tipo==2)
        resultado=(valor&0x0000FF)<<8;      //AH
    if (tipo==3)
        resultado=valor&0x000FFFF;          //AX
    return resultado;
}






void MOV(long int op1,char top1,long int valor,tmv *mv){
    char tiporeg=(op1>>4)&0x3;
    if (top1==0){
        long int pos_memo=puntero_memo(op1,*mv);
        (*mv).TablaMemoria[pos_memo]=(valor);
    }else{
        valor=Mascara_registro(valor,tiporeg);
        (*mv).TablaRegistros[(op1&0x0F)]=Suma_reg(tiporeg,op1,*mv);
        (*mv).TablaRegistros[(op1&0x0F)]+=valor;
    }
}
void ADD(long int op1,char top1,long int valor,tmv *mv){
    char tiporeg=(op1>>4)&0x3;
    long int aux=0,nz=0;
    if (top1==0){
        long int valorm=Valor_mem(op1,*mv);
        nz=valorm+valor;
        valorm=nz;
        MOV(op1,top1,valorm,mv);
    }else{
        aux=Valor_reg(tiporeg,op1,*mv);
        aux+=valor;
        nz=aux;
        aux=Mascara_registro(aux,tiporeg);
        if (tiporeg!=0)
            aux+=Suma_reg(tiporeg,op1,*mv);
        (*mv).TablaRegistros[(op1&0x0F)]=aux;
    }
    Ultima_operacion(mv,nz);
}