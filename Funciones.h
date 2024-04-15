
#define MAX_M 16384
#include <time.h>


typedef struct{
    short int base,size;
}tds; //tabla de descriptores de segmento

typedef struct{
    char TablaMemoria[MAX_M];
    int TablaRegistros[16];
    tds TablaDeSegmentos[2];
}tmv; //tipo de maquina virtual


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

typedef void Func_1op_tipo2(int valor,tmv *mv);
void JMP(int valor,tmv *mv);
void JZ(int valor,tmv *mv);
void JP(int valor,tmv *mv);
void JN(int valor,tmv *mv);
void JNZ(int valor,tmv *mv);
void JNP(int valor,tmv *mv);
void JNN(int valor,tmv *mv);
void LDH(int valor,tmv *mv);
void LDL(int valor,tmv *mv);
Func_1op_tipo2 *tipo2[]={0,JMP,JZ,JP,JN,JNZ,JNP,JNN,LDL,LDH};

typedef void Func_1op_tipo1(int op,char top,tmv *mv);
void NOT(int op,char top,tmv *mv);
void SYS(int op,char top,tmv *mv);
Func_1op_tipo1 *tipo1[]={SYS,0,0,0,0,0,0,0,0,0,NOT,0,0};

int Valor_op(int op,char top,tmv mv);
int Valor_mem(int op,tmv mv);
int puntero_memo(int op,tmv mv);
void Leer(char tipo,int *tot);
void Integridad_op(int op,tmv mv);
void Escribe(int valor,unsigned char tipo,int tamano);


void imprimirBinario(unsigned int num){
      if (num>1) {
            imprimirBinario(num >> 1);
     }
     printf("%d ", num &1);

}
void Pre_Funcion(short int cant,int op1,int op2,char top1,char top2,char operacion,tmv *mv){
    int valor;
    if (cant==2){
        if (operacion!=3){
            valor=Valor_op(op1,top1,*mv);
            funciones[operacion](op2,top2,valor,mv);
        }else
            SWAP(op1,op2,top1,top2,mv);
    }else{
        if (operacion<10 && operacion>=1){
            valor=Valor_op(op1,top1,*mv);
            tipo2[operacion](valor,mv);
        }
        else{
           tipo1[operacion](op1,top1,mv);
        }
    }
}



int Valor_op(int op,char top,tmv mv){
    int tiporeg=op>>4;        //aca lo hace por que si esta en un registro,lo corres 4 bites y los 2 primeros que te quedan son la cantidad que agarras de esos 4 bytes, EAX(4 bytes) AL(1 byte)
    int resultado=0;
    if (top==2){
            if (tiporeg==3)
                resultado=(short int)(mv.TablaRegistros[(op&0x0F)]&0x0000FFFF);    //AX  primeros 16 bits
            if (tiporeg==2)
                resultado=(char)((mv.TablaRegistros[(op&0x0F)]&0x0000FF00)>>8);    //AH  
            if (tiporeg==1){   
                resultado=(char)(mv.TablaRegistros[(op&0x0F)]&0x000000FF);   
            }       //AL
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


int Valor_mem(int op,tmv mv){
    int resultado=0;
    int pos_memo=puntero_memo(op,mv);
    for (int i=0;i<=3;i++){
        resultado+=(mv.TablaMemoria[pos_memo+i])&0x000000FF;
        if (i!=3)
            resultado=resultado<<8;
    }
    return resultado;
}

int puntero_memo(int op,tmv mv){
    short int offset=(op&0x0000FFFF);     //ultimos 2 bytes para el offset
    int treg=(op&0x0F0000)>>16;         //4 bits que te da que segmento queres apuntar
    int reg=mv.TablaRegistros[treg]; //si es 0 elige donde apunta el CS y el 1 seria el DS
    int reg_pointer=reg>>16;         //puntero al registro, ya sea CS o DS  
    short int reg_offset=(reg&0x0000FFFF);//offset del registro 
    return mv.TablaDeSegmentos[reg_pointer].base+reg_offset+offset;
}


int Valor_reg(char tipo,int op,tmv mv){
    int resultado=0;
    if (tipo==0)
        resultado=mv.TablaRegistros[(op&0x0F)];   //EAX
    if (tipo==1)
        resultado=mv.TablaRegistros[(op&0x0F)]&0x000000FF;  //AL
    if (tipo==2)
        resultado=(mv.TablaRegistros[(op&0x0F)]&0x0000FF00)>>8;  //AH
    if (tipo==3)
        resultado=mv.TablaRegistros[(op&0x0F)]&0x0000FFFF;  //AX
    return resultado;
}

int Setea_Asignacion(char tipo,int op,tmv mv){    //esto setea el registro es 0 para que se puede asignar el nuevo numero
    int resultado=0;
    if (tipo==1)
        resultado=mv.TablaRegistros[(op&0x0F)]&0xFFFFFF00; 
    if (tipo==2)
        resultado=mv.TablaRegistros[(op&0x0F)]&0xFFFF00FF;  
    if (tipo==3)
        resultado=mv.TablaRegistros[(op&0x0F)]&0xFFFF0000;
    return resultado;
}
int Mascara_registro(int valor, char tipo){
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
void Ultima_operacion(tmv *mv,int nz){
    (*mv).TablaRegistros[8]=0; //queda asi si es positivo
    if (nz==0)
        (*mv).TablaRegistros[8]=0x40000000;     //=0
    if (nz<0)
        (*mv).TablaRegistros[8]=0x80000000;     //negativo
}






void MOV(int op1,char top1,int valor,tmv *mv){
    char tiporeg=(op1>>4)&0x3;  //esto es para ver que registro es
    if (top1==0){
        int pos_memo=puntero_memo(op1,*mv);  //buscamos el puntero a memoria
        for(int i=0;i<=3;i++){
            (*mv).TablaMemoria[pos_memo+i]=(((valor))>>(3-i)*8);  //guardamos los 4 bytes        aca mi duda es si no guarda tambien los valores mas altos en los siguientes
        }
    }else{   //significa que es un registro
        valor=Mascara_registro(valor,tiporeg);      //le aplicamos la mascara a el valor segun el tipo de registro
        (*mv).TablaRegistros[(op1&0x0F)]=Setea_Asignacion(tiporeg,op1,*mv); //pone en 0 el registro
        (*mv).TablaRegistros[(op1&0x0F)]+=valor;        //seteo el valor de el op2
    }
}
void ADD(int op1,char top1,int valor,tmv *mv){    //hice cambios en este
    printf("ADD-");
    char tiporeg=(op1>>4)&0x3;
    int nz=0,valorm;
    if (top1==0){       //en memoria
        valorm=Valor_mem(op1,*mv); //valor del operando 1
    }else{      //es un registro
        valorm=Valor_reg(tiporeg,op1,*mv); //busca el valor del registro de op1
    }
    nz=valorm+valor;    //suma los dos operandos
    MOV(op1,top1,nz,mv); 
    Ultima_operacion(mv,nz);
}

void SUB(int op1,char top1,int valor,tmv*mv){  //cambios
    char tiporeg=(op1>>4)&0x3;
    int nz=0,valorm;
    if (top1==0)
        valorm=Valor_mem(op1,*mv);  
    else
        valorm=Valor_reg(tiporeg,op1,*mv);
    nz=valorm-valor;    //resta
    MOV(op1,top1,nz,mv);
    Ultima_operacion(mv,nz);
    
}

void SWAP(int op1,int op2,char top1,char top2,tmv *mv){
    int aux,aux2;
    if (top1==top2){
        if (top1==0){
           aux=Valor_mem(op1,*mv);      //busco valor op1 en memoria
           aux2=Valor_mem(op2,*mv);     //busco valor op2 en memoria
        }else{
            aux=Valor_op(op1,top1,*mv);     //busco valor op1 en registro o si es inmediato
            aux2=Valor_op(op2,top2,*mv);     //busco valor op2 en registro o si es inmediato
        }
        MOV(op1,top1,aux2,mv);      //seteo el valor del op2 en la posicion o registro del 1
        MOV(op2,top2,aux,mv);       //vise
    }
}
void MUL(int op1,char top1,int valor,tmv *mv){    //cambio
    printf("MUL-\n");
    char tiporeg=(op1>>4)&0x3;
    int nz=0,valorm;
    if (top1==0)
        valorm=Valor_mem(op1,*mv);  
    else
        valorm=Valor_reg(tiporeg,op1,*mv);
    nz=valorm*valor;    //mult
    MOV(op1,top1,nz,mv);
    Ultima_operacion(mv,nz);
}

void DIV(int op1,char top1,int valor,tmv *mv){
    char tiporeg=(op1>>4)&0x3;
    int nz=0,valorm;
    if(valor!=0){
        if (top1==0)
            valorm=Valor_mem(op1,*mv);  
        else
            valorm=Valor_reg(tiporeg,op1,*mv);
        nz=valorm/valor;    //div
        (*mv).TablaRegistros[9]=valorm%valor;
        MOV(op1,top1,nz,mv);
        Ultima_operacion(mv,nz);
    }else{
        printf("ERROR(2): no se puede dividir por 0");
        exit(-1);
    }
}

void CMP(int op1,char top1,int valor2,tmv *mv){
    int valor=Valor_op(op1,top1,*mv); //Calculamos el valor del op1
    Ultima_operacion(mv,valor-valor2);  //mandamos a ultima operacion la resta, si op2 es mayor entonces va a dar negativo y si son iguales 0.
}

void SHL(int op1,char top1,int valor,tmv *mv){
    char tiporeg=(op1>>4)&0x3;
    int nz=0,valorm;
    if (top1==0)
        valorm=Valor_mem(op1,*mv);  
    else
        valorm=Valor_reg(tiporeg,op1,*mv);
    nz=valorm<<valor;    //shift left
    MOV(op1,top1,nz,mv);
    Ultima_operacion(mv,nz);
}

void SHR(int op1,char top1,int valor,tmv *mv){
    char tiporeg=(op1>>4)&0x3;
    int nz=0,valorm;
    if (top1==0)
        valorm=Valor_mem(op1,*mv);  
    else
        valorm=Valor_reg(tiporeg,op1,*mv);
    nz=valorm>>valor;    //shift right
    MOV(op1,top1,nz,mv);
    Ultima_operacion(mv,nz);
}
void AND(int op1,char top1,int valor,tmv *mv){
    char tiporeg=(op1>>4)&0x3;
    int nz=0,valorm;
    if (top1==0)
        valorm=Valor_mem(op1,*mv);  
    else
        valorm=Valor_reg(tiporeg,op1,*mv);
    nz=valorm&&valor;    //and
    MOV(op1,top1,nz,mv);
    Ultima_operacion(mv,nz);
}
void OR(int op1,char top1,int valor,tmv *mv){
    char tiporeg=(op1>>4)&0x3;
    int nz=0,valorm;
    if (top1==0)
        valorm=Valor_mem(op1,*mv);  
    else
        valorm=Valor_reg(tiporeg,op1,*mv);
    nz=valorm||valor;    //OR
    MOV(op1,top1,nz,mv);
    Ultima_operacion(mv,nz);
}
void XOR(int op1,char top1,int valor,tmv *mv){
    char tiporeg=(op1>>4)&0x3;
    int nz=0,valorm;
    if (top1==0)
        valorm=Valor_mem(op1,*mv);  
    else
        valorm=Valor_reg(tiporeg,op1,*mv);
    nz=valorm^valor;    //XOR   
    MOV(op1,top1,nz,mv);
    Ultima_operacion(mv,nz);
}

void RND(int op1,char top1,int valor2,tmv *mv){      //cambio de consigna
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
        valorm=Valor_mem(op,*mv);  
    else
        valorm=Valor_reg(tiporeg,op,*mv);
    nz=~valorm;    //NOT 
    MOV(op,top,nz,mv);
    Ultima_operacion(mv,nz);
}

void SYS(int op,char top,tmv *mv){
    mue( *mv);
    int valor,pos,tot;
    int rep,byt;
    char tipo;
    valor=Valor_op(op,top,*mv);
    //preparo pos en edx
    int regpointer=((*mv).TablaRegistros[13]&0xFFFF0000)>>16;
    int memo=(*mv).TablaDeSegmentos[regpointer].base;
    int offset=((*mv).TablaRegistros[13]&0x0000FFFF);
    pos=memo+offset;
    rep=(*mv).TablaRegistros[12]&0x000000FF;//CL
    byt=((*mv).TablaRegistros[12]&0x0000FF00)>>8;//CH
    tipo=(*mv).TablaRegistros[10]&0x000000FF;//AL
    //escribe por pantalla
    if (valor==2){
        for(int i=1;i<=rep;i++){    //cantidad de celdas
            tot=0;
            for (int j=1;j<=byt;j++){   //tamanio
                tot+=((*mv).TablaMemoria[pos+j-1])&0xFF;
                if (j!=byt)
                    tot=tot<<8;
            }
            Muestra_mem(pos-(*mv).TablaDeSegmentos[1].base);
            Escribe(tot,tipo,byt);
            printf("\n");
            pos+=byt;
        }
    }
    //lee por pantalla
    if (valor==1){
        for (int i=1;i<=rep;i++){
            Muestra_mem(pos-(*mv).TablaDeSegmentos[1].base);
            Leer(tipo,&tot);
            for (int j=1;j<=byt;j++){
                (*mv).TablaMemoria[pos+j-1]=(tot>>(byt-j)*8)&0x000000FF;
            }
            pos+=byt;
        }
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


void Integridad_op(int op,tmv mv){
    char pos0=mv.TablaDeSegmentos[1].base;
    short int valor=0;
    int offset=(op&0x0000FFFF);
    int treg=(op&0x00FF0000)>>16;
    int reg=mv.TablaRegistros[treg];
    int reg_pointer=reg>>16;
    int reg_offset=reg&0x0000FFFF;
    if (reg==0){
        valor=offset;
    }else{
        valor=offset+mv.TablaDeSegmentos[reg_pointer].base+reg_offset;
    }
    if (valor<pos0){
        printf("ERROR (3): estas entrando al CS");
        exit(-1);
    }
}