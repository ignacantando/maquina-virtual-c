#define MAX_M 16384
#include <time.h>

typedef struct{
    short int base,size;
}tds; //tabla de descriptores de segmento

typedef struct{
    char TablaMemoria[MAX_M];
    int TablaRegistros[16];
    tds TablaDeSegmentos[8];
}tmv; //tipo de maquina virtual

//variables globales del programa final
unsigned short int Toggle_debbug=0;
char direVMI[50]="";
unsigned int memory=16384;
unsigned short int entry;

typedef void Funciones2op(int op1,char top1,int valor,tmv *mv);
void MOV(int op1,char top1,int valor,tmv *mv);
void ADD(int op1,char top1,int valor,tmv *mv);
void SUB(int op1,char top1,int valor,tmv *mv);
void SWAP(int op1,int op2,char top1,char top2,tmv *mv);
void MUL(int op1,char top1,int valor,tmv *mv);
void DIV(int op1,char top1,int valor,tmv *mv);
void CMP(int op1,char top1,int valor,tmv *mv);
void SHL(int op1,char top1,int valor2,tmv *mv);
void SHR(int op1,char top1,int valor2,tmv *mv);
void AND(int op1,char top1,int valor2,tmv *mv);
void OR(int op1,char top1,int valor2,tmv *mv);
void XOR(int op1,char top1,int valor2,tmv *mv);
void RND(int op1,char top1,int valor2,tmv *mv);
Funciones2op *funciones[]={MOV,ADD,SUB,0,MUL,DIV,CMP,SHL,SHR,AND,OR,XOR,RND};

typedef void Funciones1opTipo2(int valor,tmv *mv);
void JMP(int valor,tmv *mv);
void JZ(int valor,tmv *mv);
void JP(int valor,tmv *mv);
void JN(int valor,tmv *mv);
void JNZ(int valor,tmv *mv);
void JNP(int valor,tmv *mv);
void JNN(int valor,tmv *mv);
void LDH(int valor,tmv *mv);
void LDL(int valor,tmv *mv);
void PUSH(int valor,tmv *mv);
void CALL(int valor,tmv *mv);
Funciones1opTipo2 *tipo2[]={0,JMP,JZ,JP,JN,JNZ,JNP,JNN,LDL,LDH,0,PUSH,0,CALL};//puede estar mal

typedef void Funciones1opTipo1(int op,char top,tmv *mv);
void NOT(int op,char top,tmv *mv);
void SYS(int op,char top,tmv *mv);
void POP(int op,char top,tmv *mv);
Funciones1opTipo1 *tipo1[]={SYS,0,0,0,0,0,0,0,0,0,NOT,0,POP,0,0};//puede estar mal

void RET(tmv *mv);

int ValorOperando(int op,char top,tmv mv);
int ValorMemoria(int op,tmv mv);
unsigned short int celdas_memo(int op);
int ValorRegistro(char tipo,int op,tmv mv);
int DireccionMemoria(int op,tmv mv);
int SeteaAsignacion(char tipo,int op,tmv mv);
int MascaraRegistro(int valor, char tipo);
void Leer(char tipo,int *tot);
void VerificarOperacion(int op,tmv mv);
void Escribe(int valor,unsigned char tipo,int tamano);
void Prefuncion(short int cant,int op1,int op2,char top1,char top2,char operacion,tmv *mv);

void Prefuncion(short int cant,int op1,int op2,char top1,char top2,char operacion,tmv *mv){
    int valor;
    if (cant==2){
        if (operacion!=3){
            valor=ValorOperando(op1,top1,*mv);
            funciones[operacion](op2,top2,valor,mv);
        }else
            SWAP(op1,op2,top1,top2,mv);
    }else{
        if (cant==0)
            RET(mv);
        else{
            if ((operacion<10 && operacion>=1) || operacion==11 || operacion==13){
                valor=ValorOperando(op1,top1,*mv);
                tipo2[operacion](valor,mv);
            }
            else{
            tipo1[operacion](op1,top1,mv);
            }
        }
    }
}

int ValorOperando(int op,char top,tmv mv){
    int tiporeg=op>>4;
    int resultado=0;
    if (top==2){
            if (tiporeg==3)
                resultado=(short int)(mv.TablaRegistros[(op&0x0F)]&0x0000FFFF);
            if (tiporeg==2)
                resultado=(char)((mv.TablaRegistros[(op&0x0F)]&0x0000FF00)>>8);
            if (tiporeg==1){
                resultado=(char)(mv.TablaRegistros[(op&0x0F)]&0x000000FF);
            }
            if (tiporeg==0)
                resultado=mv.TablaRegistros[(op&0x0F)];
    }
    if (top==0){
       resultado=ValorMemoria(op,mv);
    }
    if (top==1){
        resultado=(short int)op;
    }
    return (resultado);
}

 int ValorMemoria(int op,tmv mv){
    int resultado=0;
    int pos_memo=DireccionMemoria(op,mv);
    unsigned short int cant=celdas_memo(op);
    for (int i=0;i<cant;i++){
        resultado+=(mv.TablaMemoria[pos_memo+i])&0x000000FF;
        if (i!=cant-1)
            resultado=resultado<<8;
    }
    int aux=resultado;
    if (cant==1)
        resultado=(char)aux;
    if (cant==2)
        resultado=(short int)aux;
    return resultado;

}

unsigned short int celdas_memo(int op){
    unsigned int aux=op&(0x00C00000);     //C=12   1100   
    unsigned int aux2=aux>>22;
    aux2=aux2^0x3;
    aux2++;
    return aux2;
}

int DireccionMemoria(int op,tmv mv){
    short int offset=(op&0x0000FFFF);
    int treg=(op&0x0F0000)>>16;
    int reg=mv.TablaRegistros[treg];
    int reg_pointer=reg>>16;
    short int reg_offset=(reg&0x0000FFFF);
    return mv.TablaDeSegmentos[reg_pointer].base+reg_offset+offset;
}

int ValorRegistro(char tipo,int op,tmv mv){
    int resultado=0;
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

int SeteaAsignacion(char tipo,int op,tmv mv){
    int resultado=0;
    if (tipo==1)
        resultado=mv.TablaRegistros[(op&0x0F)]&0xFFFFFF00;
    if (tipo==2)
        resultado=mv.TablaRegistros[(op&0x0F)]&0xFFFF00FF;
    if (tipo==3)
        resultado=mv.TablaRegistros[(op&0x0F)]&0xFFFF0000;
    return resultado;
}

int MascaraRegistro(int valor, char tipo){
    int resultado=0;
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

void ModificaCC(tmv *mv,int nz){
    (*mv).TablaRegistros[8]=0; //queda asi si es positivo
    if (nz==0)
        (*mv).TablaRegistros[8]=0x40000000;     //=0
    if (nz<0)
        (*mv).TablaRegistros[8]=0x80000000;     //negativo
}

void MOV(int op1,char top1,int valor,tmv *mv){
    char tiporeg=(op1>>4)&0x3;
    if (top1==0){
        int pos_memo=DireccionMemoria(op1,*mv);
        unsigned short int cant=celdas_memo(op1)-1;
        for(int i=cant;i>=0;i--){
            (*mv).TablaMemoria[pos_memo+i]=(((valor))>>(cant-i)*8);
        }
    }else{
        valor=MascaraRegistro(valor,tiporeg);
        (*mv).TablaRegistros[(op1&0x0F)]=SeteaAsignacion(tiporeg,op1,*mv);
        (*mv).TablaRegistros[(op1&0x0F)]+=valor;
    }
}

void ADD(int op1,char top1,int valor,tmv *mv){
    char tiporeg=(op1>>4)&0x3;
    int nz=0,valorm;
    if (top1==0){
        valorm=ValorMemoria(op1,*mv);
    }else{
        valorm=ValorRegistro(tiporeg,op1,*mv);
    }
    nz=valorm+valor;
    MOV(op1,top1,nz,mv);
    ModificaCC(mv,nz);
}

void SUB(int op1,char top1,int valor,tmv*mv){
    char tiporeg=(op1>>4)&0x3;
    int nz=0,valorm;
    if (top1==0)
        valorm=ValorMemoria(op1,*mv);
    else
        valorm=ValorRegistro(tiporeg,op1,*mv);
    nz=valorm-valor;
    MOV(op1,top1,nz,mv);
    ModificaCC(mv,nz);

}

void SWAP(int op1,int op2,char top1,char top2,tmv *mv){
    int aux,aux2;
    if (top1==top2){
        if (top1==0){
           aux=ValorMemoria(op1,*mv);      //busco valor op1 en memoria
           aux2=ValorMemoria(op2,*mv);     //busco valor op2 en memoria
        }else{
            aux=ValorOperando(op1,top1,*mv);     //busco valor op1 en registro o si es inmediato
            aux2=ValorOperando(op2,top2,*mv);     //busco valor op2 en registro o si es inmediato
        }
        MOV(op1,top1,aux2,mv);      //seteo el valor del op2 en la posicion o registro del 1
        MOV(op2,top2,aux,mv);
    }
}

void MUL(int op1,char top1,int valor,tmv *mv){
    char tiporeg=(op1>>4)&0x3;
    int nz=0,valorm;
    if (top1==0)
        valorm=ValorMemoria(op1,*mv);
    else
        valorm=ValorRegistro(tiporeg,op1,*mv);
    nz=valorm*valor;
    MOV(op1,top1,nz,mv);
    ModificaCC(mv,nz);
}

void DIV(int op1,char top1,int valor,tmv *mv){
    char tiporeg=(op1>>4)&0x3;
    int nz=0,valorm;
    if(valor!=0){
        if (top1==0)
            valorm=ValorMemoria(op1,*mv);
        else
            valorm=ValorRegistro(tiporeg,op1,*mv);
        nz=valorm/valor;
        (*mv).TablaRegistros[9]=valorm%valor;
        MOV(op1,top1,nz,mv);
        ModificaCC(mv,nz);
    }else{
        printf("ERROR(2): no se puede dividir por 0");
        exit(-1);
    }
}

void CMP(int op1,char top1,int valor2,tmv *mv){
    int valor=ValorOperando(op1,top1,*mv);
    ModificaCC(mv,valor-valor2);
}

void SHL(int op1,char top1,int valor,tmv *mv){
    char tiporeg=(op1>>4)&0x3;
    int nz=0,valorm;
    if (top1==0)
        valorm=ValorMemoria(op1,*mv);
    else
        valorm=ValorRegistro(tiporeg,op1,*mv);
    nz=valorm<<valor;
    MOV(op1,top1,nz,mv);
    ModificaCC(mv,nz);
}

void SHR(int op1,char top1,int valor,tmv *mv){
    char tiporeg=(op1>>4)&0x3;
    int nz=0,valorm;
    if (top1==0)
        valorm=ValorMemoria(op1,*mv);
    else
        valorm=ValorRegistro(tiporeg,op1,*mv);
    nz=valorm>>valor;
    MOV(op1,top1,nz,mv);
    ModificaCC(mv,nz);
}

void AND(int op1,char top1,int valor,tmv *mv){
    char tiporeg=(op1>>4)&0x3;
    int nz=0,valorm;
    if (top1==0)
        valorm=ValorMemoria(op1,*mv);
    else
        valorm=ValorRegistro(tiporeg,op1,*mv);
    nz=valorm&&valor;
    MOV(op1,top1,nz,mv);
    ModificaCC(mv,nz);
}

void OR(int op1,char top1,int valor,tmv *mv){
    char tiporeg=(op1>>4)&0x3;
    int nz=0,valorm;
    if (top1==0)
        valorm=ValorMemoria(op1,*mv);
    else
        valorm=ValorRegistro(tiporeg,op1,*mv);
    nz=valorm||valor;
    MOV(op1,top1,nz,mv);
    ModificaCC(mv,nz);
}

void XOR(int op1,char top1,int valor,tmv *mv){
    char tiporeg=(op1>>4)&0x3;
    int nz=0,valorm;
    if (top1==0)
        valorm=ValorMemoria(op1,*mv);
    else
        valorm=ValorRegistro(tiporeg,op1,*mv);
    nz=valorm^valor;
    MOV(op1,top1,nz,mv);
    ModificaCC(mv,nz);
}

void RND(int op1,char top1,int valor2,tmv *mv){
    // Semilla para la generación de números aleatorios
    srand(time(NULL));

    // Generar número aleatorio entre 0 y valor2
    int valor = rand() % (valor2 + 1);

    MOV(op1,top1,valor,mv);
}

void JMP(int valor,tmv *mv){
    (*mv).TablaRegistros[5]=valor;  //pone el IP en el valor que le damos
}

void JZ(int valor,tmv *mv){
    if((*mv).TablaRegistros[8]==0x40000000) //Si es 0 salta al valor
        JMP(valor,mv);
}

void JP(int valor,tmv *mv){
    if((*mv).TablaRegistros[8]==0x00000000) //si es positivo salta
        JMP(valor,mv);
}

void JN(int valor,tmv *mv){
    if((*mv).TablaRegistros[8]==0x80000000) //si es negativo salta
        JMP(valor,mv);
}

void JNZ(int valor,tmv *mv){
    if((*mv).TablaRegistros[8]==0x80000000 || (*mv).TablaRegistros[8]==0x00000000)  //si es negativo o positivo salta
        JMP(valor,mv);
}

void JNP(int valor,tmv*mv){
    if((*mv).TablaRegistros[8]==0x80000000 || (*mv).TablaRegistros[8]==0x40000000)  //si es negativo o 0 salta
        JMP(valor,mv);
}

void JNN(int valor,tmv *mv){
    if((*mv).TablaRegistros[8]==0x40000000 || (*mv).TablaRegistros[8]==0x00000000)  //si es 0 o positivo salta
        JMP(valor,mv);
}

void LDH(int valor,tmv *mv){
    (*mv).TablaRegistros[9]=((*mv).TablaRegistros[9]&0x0000FFFF)+((valor&0x0000FFFF)<<16);
}

void LDL(int valor,tmv *mv){
    (*mv).TablaRegistros[9]=((*mv).TablaRegistros[9]&0xFFFF0000)+(valor&0x0000FFFF);
}

void NOT(int op,char top,tmv *mv){
    char tiporeg=(op>>4)&0x3;
    int nz=0,valorm;
    if (top==0)
        valorm=ValorMemoria(op,*mv);
    else
        valorm=ValorRegistro(tiporeg,op,*mv);
    nz=~valorm;    //NOT
    MOV(op,top,nz,mv);
    ModificaCC(mv,nz);
}

void SYS(int op,char top,tmv *mv){
    int valor,pos,tot;
    int rep,byt;
    char tipo;
    unsigned int tamanostr;
    valor=ValorOperando(op,top,*mv);
    int regpointer=((*mv).TablaRegistros[13]&0xFFFF0000)>>16;
    int memo=(*mv).TablaDeSegmentos[regpointer].base;
    int offset=((*mv).TablaRegistros[13]&0x0000FFFF);
    pos=memo+offset;
    rep=(*mv).TablaRegistros[12]&0x000000FF;//CL
    byt=((*mv).TablaRegistros[12]&0x0000FF00)>>8;//CH
    tipo=(*mv).TablaRegistros[10]&0x000000FF;//AL
    tamanostr=(*mv).TablaRegistros[12]&0x0000FFFF;//CX
    //escribe por pantalla
    if (valor==2){
        for(int i=1;i<=rep;i++){    //cantidad de celdas
            tot=0;
            for (int j=1;j<=byt;j++){   //tamanio
                tot+=((*mv).TablaMemoria[pos+j-1])&0xFF;
                if (j!=byt)
                    tot=tot<<8;
            }
            Muestra_mem(pos);
            Escribe(tot,tipo,byt);
            printf("\n");
            pos+=byt;
        }
    }
    //lee por pantalla
    if (valor==1){
        for (int i=1;i<=rep;i++){
            Muestra_mem(pos);
            Leer(tipo,&tot);
            for (int j=1;j<=byt;j++){
                (*mv).TablaMemoria[pos+j-1]=(tot>>(byt-j)*8)&0x000000FF;
            }
            pos+=byt;
        }
    }
    if (valor==3){
        char palabra[1600];
        gets(palabra);
        fflush(stdin);
        EscribeString(palabra,tamanostr,pos,mv);
    }
    if (valor==4){
        LeeString(pos,mv);
    }
    if (valor==7)
        system("cls");
    if (valor==0xF && strcmp(direVMI,"")!=0)
        Toggle_debbug=1;
}

void EscribeString(char *palabra,unsigned int tamanostr,long int pos,tmv *mv){
    int i=0;
    while (i<strlen(palabra) && i!=tamanostr){
        (*mv).TablaMemoria[pos+i]=palabra[i];
        i++;
    }
    if (i==tamanostr)
        (*mv).TablaMemoria[pos+i]=0x00;
    else
        (*mv).TablaMemoria[pos+i+1]=0x00;
}

void LeeString(long int pos,tmv *mv){
    int i=0;
    while ((*mv).TablaMemoria[pos+i]!=0x00){
        printf("%c",(*mv).TablaMemoria[pos+i]);
        i++;
    }
}

void Leer(char tipo,int *tot){
    if (tipo==1)
        scanf("%d",tot);
    if (tipo==2)
        scanf("%c",tot);
    if (tipo==4)
        scanf("%o",tot);
    if (tipo==8)
        scanf("%x",tot);
}

void Escribe(int valor,unsigned char tipo,int tamano){
    unsigned char u;
    int aux;
        u=tipo&0x1;
        if (u!=0)
            printf("#%d ",valor);
        u=tipo&0x2;
        if (u!=0){
            printf("'");
            //mostrar byte a byte
            for(int i=1;i<=tamano;i++){
                aux=(valor>>(tamano-i)*8)&0x000000FF;
                if (aux>=32 && aux<=126)
                    printf("%c",aux);
                else
                    printf(".");
            }
            printf(" ");
        }
        u=tipo&0x4;
        if (u!=0)
            printf("@%o ",valor);
        u=tipo&0x8;
        if (u!=0)
            printf("%c%X ",'%',valor);
}

void VerificarOperacion(int op,tmv mv){
    char pos0=mv.TablaDeSegmentos[1].base;
    short int valor=0;
    int offset=(op&0x0000FFFF);
    int treg=(op&0x00FF0000)>>16;
    int reg=mv.TablaRegistros[treg];
    int punteroRegistro=reg>>16;
    int offsetRegistro=reg&0x0000FFFF;
    if (reg==0){
        valor=offset;
    }else{
        valor=offset+mv.TablaDeSegmentos[punteroRegistro].base+offsetRegistro;
    }
    if (valor<pos0){
        printf("ERROR (3): estas entrando al CS");
        exit(-1);
    }
}

void Integridad_op(long int op,tmv mv){
    int valor=0;
    short int offset=op&0x0000FFFF;      //(0000)segmento (0003)offset
    int treg=(op&0x00FF0000)>>16;
    long int reg=mv.TablaRegistros[treg];
    long int reg_pointer=(reg>>16)&0x1;
    short int reg_offset=reg&0x0000FFFF;
    int pos0=mv.TablaDeSegmentos[reg_pointer].size;
    int pos1=mv.TablaDeSegmentos[reg_pointer].base;
        valor=offset+reg_offset;
        if (valor>pos0 || pos0+valor<0 || pos1+valor<pos1){
            printf("ERROR (3): Segmentation fault");
            exit(-1);
        }
}

void PUSH(int valor,tmv *mv){
    int ss_pointer=(((*mv).TablaRegistros[6]&0xFFFF0000)>>16);  //puntero al dato, por ejemplo da 6 y asi lo busca en la tabla de datos
    int sp_offset=(*mv).TablaRegistros[6]&0x0000FFFF;       //corrimiento del SP en el segmento
    int pos_ss=(*mv).TablaDeSegmentos[ss_pointer].base;          //posicion en memoria del ss
    if (sp_offset-4<0){     //Como el sp apunta al maximo tamanio si este se pasa es por que la pila esta llena
        printf("ERROR(5): Stack overflow error");
        exit(-1);
    }
    for (int i=0;i<=3;i++){
        sp_offset-=1;   //decremento en 4 el sp
        (*mv).TablaMemoria[pos_ss+sp_offset]=(valor>>(i*8));    //guardo el valor en memoria(pila)
    }
    (*mv).TablaRegistros[6]=(*mv).TablaRegistros[6]&0xFFFF0000; //me parece medio al pedo esto por que no va a cambiar el segmento
    (*mv).TablaRegistros[6]+=sp_offset;     //le cambia el offset
}

void POP(int op,char top,tmv *mv){
    printf("POP");
    int aux=0;
    int ss_pointer=(((*mv).TablaRegistros[6]&0xFFFF0000)>>16);
    int sp_offset=(*mv).TablaRegistros[6]&0x0000FFFF;
    int pos_ss=(*mv).TablaDeSegmentos[ss_pointer].base;
    if (sp_offset+4>(*mv).TablaDeSegmentos[ss_pointer].size){
        printf("ERROR(6): Stack underflow error");
        exit(-1);
    }
    for (int i=0;i<=3;i++){
        aux+=((*mv).TablaMemoria[pos_ss+sp_offset])&0xFF;
        if (i!=3)
            aux=aux<<8;
        sp_offset+=1;
    }
    (*mv).TablaRegistros[6]=(*mv).TablaRegistros[6]&0xFFFF0000;
    (*mv).TablaRegistros[6]+=sp_offset;
    MOV(op,top,aux,mv);
}

void CALL(int valor,tmv *mv){
    PUSH((*mv).TablaRegistros[5],mv);
    JMP(valor,mv);
}

void RET(tmv *mv){
    POP(0x05,2,mv);  //(*mv).TablaRegistros[5]
    printf("tabla=%d",(*mv).TablaRegistros[5]);
}