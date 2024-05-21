#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Funciones.h"
#include "disasembler.h"


void InicializarRegistros(tmv *mv);
void InicializarMemoria(char *direccion,tmv *mv,unsigned int memory);
void Lectura(tmv *mv);
void CantidadOperandos(char instruccion,char *top1,char *top2,short int *cant);

int main(int argc,char *argv[]){
    tmv mv;
    unsigned short int diss=0;
    LecturaArgumentos(argc,argv,&diss,&memory,direVMI);
    InicializarRegistros(&mv);
    if(argv[1][strlen(argv[1])-1]=='x'){  //ACA TENEMOS QUE VER COMO HACER SI EL ARCHIVO VMX NO ESTA, EN ESE CASO SOLO LEERIAMOS EL VMI(el arg[1] va a ser uno de los dos)
        InicializarMemoria(argv[1],&mv,memory);
    }
    else
        cargarIMG(&mv);
    if(diss==1)
        MostrarDissasembler(mv);
    mue(mv);
    Lectura(&mv);
}

void InicializarRegistros(tmv *mv){
        (*mv).TablaRegistros[5]=0;//ip
        (*mv).TablaRegistros[8]=0;//cc
        (*mv).TablaRegistros[9]=0;//ac
}

void InicializarMemoria(char *direccion,tmv *mv,unsigned int memory){
    char codigo,version,nombre[5];
    FILE *arch=fopen(direccion,"rb");
    int i,flag;
    unsigned int valor=0;
    unsigned char val=0;
    if (arch!=NULL){
        fread(nombre,sizeof(nombre),1,arch);//lee el titulo
        fread(&version,sizeof(char),1,arch);//lee la version
        if (version==2){
            ver=1;
            unsigned int vec[6];
            for (int j=0;j<6;j++){
                valor=0;
                fread(&val,sizeof(char),1,arch);
                valor+=val;
                fread(&val,sizeof(char),1,arch);
                valor=valor<<8;
                valor+=val;
                vec[j]=valor;  //guarda el tamanio del segmento
            }
            SetearRegistros(mv,vec,memory);
            i=vec[4]+1;
            flag=i;
            entry+=i;
        }
        else{
            fread(&val,sizeof(char),1,arch);
            valor+=val;
            valor=(valor<<8);
            fread(&val,sizeof(char),1,arch);
            valor+=val;
            (*mv).TablaDeSegmentos[0].base=0;
            (*mv).TablaDeSegmentos[0].size=valor;
            (*mv).TablaDeSegmentos[1].base=valor;
            (*mv).TablaDeSegmentos[1].size=sizeof((*mv).TablaMemoria)-valor; 
            (*mv).TablaRegistros[1]=0x00010000;
            (*mv).TablaRegistros[0]=0x00000000;
            entry=0;
            ver=0;
        }
        while (!feof(arch) && (i-flag)< (*mv).TablaDeSegmentos[1].size){
            fread(&codigo, sizeof(char), 1, arch);
            (*mv).TablaMemoria[i]=codigo;//cargo el CS
            i++;
        }
        i=0;
        while (!feof(arch)){
            fread(&codigo, sizeof(char), 1, arch);    
            (*mv).TablaMemoria[i]=codigo;//cargo el KS
            i++;
        }
        fclose(arch);
    }else{
        printf("ERROR(0): el archivo no existe");
        exit(-1);
    }
}

void Lectura(tmv *mv){   //hay cambios
    char top1,top2,operacion;
    short int cant;
    int op1,op2;
    int corte;
    char dbg;
    (*mv).TablaRegistros[5]=entry;
    while ((*mv).TablaRegistros[5]!=(*mv).TablaDeSegmentos[2].base)
    {
        op1=0;
        op2=0;
        operacion=(*mv).TablaMemoria[(*mv).TablaRegistros[5]]&0x0F;
        //printf("operacion: %1X ",operacion);
        //if((*mv).TablaRegistros[5]==29)
            //mue(*mv);
       // printf("operacion: %2.2X ",operacion);
        CantidadOperandos((*mv).TablaMemoria[(*mv).TablaRegistros[5]],&top1,&top2,&cant);
        //printf("top1=%d,top2=%d,cant=%d,posicion=%d",top1,top2,cant,(*mv).TablaRegistros[5]);
        //printf("posicion=%d|| cantidad=%d",(*mv).TablaRegistros[5],cant);
        corte=top1^0x3;//le asigno el opuesto del tipo2 el cual representa la longitud
        if (cant==2){
            for (int i=1;i<=corte;i++){
                (*mv).TablaRegistros[5]++;
                op1+=((*mv).TablaMemoria[(*mv).TablaRegistros[5]])&0x000000FF;
                if (i!=corte)
                    op1=op1<<8;
            }
            corte=top2^0x3;//le asigno el opuesto del tipo2 el cual representa la longitud
            for (int i=1;i<=corte;i++){
                (*mv).TablaRegistros[5]++;
                op2+=((*mv).TablaMemoria[(*mv).TablaRegistros[5]])&0x000000FF;
                if (i!=corte)
                    op2=op2<<8;
            }
        }else if(cant==1){
            for (int i=1;i<=corte;i++){
                (*mv).TablaRegistros[5]++;
                op1+=((*mv).TablaMemoria[(*mv).TablaRegistros[5]])&0x000000FF;
                if (i!=corte)
                    op1=op1<<8;
            }
        }
        (*mv).TablaRegistros[5]++;// le sumo uno mas al ip antes de que se ejecute la operacion.
        if(cant!=0)
            Errores(top1,top2,cant,operacion,op1,op2,*mv);
        if(cant!=0 || operacion==0xFE){
             if (breakpointt==1){//BREAK POINT
                generarIMG(*mv);      
                scanf("%c",&dbg);       //ESPERA EL COMANDO QUE PONGAMOS
                fflush(stdin);          //VACIA PILA
                if (dbg=='g')           //SI ES Q SACA EL BREAKPOIT
                    breakpointt=0;
                else if(dbg=='q'){
                    exit(-1);    
                }
                Prefuncion(cant,op1,op2,top1,top2,operacion,mv);   //VA A LA FUNCION
            }else
                Prefuncion(cant,op1,op2,top1,top2,operacion,mv);//HACE EL CODIGO NORMAL SIN BREAKPOINT
        }
        else if(cant==0 && operacion==15){
            (*mv).TablaRegistros[5]=(*mv).TablaDeSegmentos[2].base;
        }
        else{
            Prefuncion(cant,op1,op2,top1,top2,operacion,mv);
        }
    }
}

void mue(tmv mv){
for(int i=0;i<=250;i++)
    printf("i=%d  =%1X \n",i,mv.TablaMemoria[i]);
}

void generarIMG(tmv mv){
    FILE *arch=fopen(direVMI,"wb");
    int memoryaux=memory/1024;
    if (arch==NULL){
        printf("ERROR no se pudo abrir el archivo binario");
    }else{
        int TD;
        char vec[16384+200];
        int j;
        vec[0]='V';
        vec[1]='M';
        vec[2]='I';
        vec[3]='2';
        vec[4]='4';
        vec[5]=1;
        j=6;
        for (int k=1;k>=0;k--){
            vec[j]=((memoryaux)>>(k)*8); 
            j++;
        }
        for (int i=0;i<=15;i++){
            for (int k=3;k>=0;k--){
                vec[j]=(mv.TablaRegistros[i])>>(k)*8;
                j++;
            }
        }
        for (int i=0;i<5;i++){
            TD=0;
            TD=mv.TablaDeSegmentos[i].base;
            TD=TD<<16;
            TD+=mv.TablaDeSegmentos[i].size;
            for (int k=3;k>=0;k--){
                vec[j]=(TD>>(k)*8);
                j++;
            }
        }
        int s=0;
        for(int i=j;i<16384;i++){
            vec[i]=mv.TablaMemoria[s];
            s++;
        }
        fwrite(vec,sizeof(vec),1,arch);
    }
    fclose(arch);
}

void cargarIMG(tmv *mv){
    FILE *arch=fopen(direVMI,"rb");
    int j=0,memoriaaux,auxreg=0,regflag=3;
    char leido;
    if (arch==NULL){
        printf("ERROR no se pudo abrir el archivo binario");
    }
    else{
        fseek(arch, 6, SEEK_SET);
        fread(&leido, sizeof(unsigned char), 1, arch);
        fread(&leido, sizeof(unsigned char), 1, arch);
        memory=leido;
        // Leer los registros
        for (int i = 0; i < 16; i++) {
            (*mv).TablaRegistros[i] = 0;
            for (int k = 3; k >= 0; k--) {
                unsigned char byte;
                fread(&byte, sizeof(unsigned char), 1, arch);
                (*mv).TablaRegistros[i] |= byte << (k * 8);
            }
        }

        // Leer la tabla de descriptores de segmentos
        for (int i = 0; i < 5; i++) {
            int TD = 0;
            for (int k = 3; k >= 0; k--) {
                unsigned char byte;
                fread(&byte, sizeof(unsigned char), 1, arch);
                TD |= byte << (k * 8);
            }
            (*mv).TablaDeSegmentos[i].base = (short int)(TD >> 16);
            (*mv).TablaDeSegmentos[i].size = (short int)(TD & 0xFFFF);
        }

        // Leer la memoria principal
        int s = 0;
        while (fread(&(*mv).TablaMemoria[s], sizeof(char), 1, arch) == 1) {
            s++;
            if (s >=16384 ) {
                break; // Evita leer más allá del tamaño de la memoria especificada
            }
        }
    }
    entry=(*mv).TablaRegistros[5];
    fclose(arch);    
}

void LecturaArgumentos(int argc,char *argv[],short int *diss,unsigned int *memory,char *direVMI){
    int i;
    if(argv[strlen(argv[1])-1]=="x") 
        i=2;
    else
        i=1;
    for (i;i<argc;i++){
        if (strcmp(argv[i],"-d")==0)
            (*diss)=1;
        else{
            if (argv[i][strlen(argv[i])-1]=='i'){
                strcpy(direVMI,argv[i]);
            }else if(argv[i][0]=='m'){
                (*memory)=0;
                for (int j=0;j<strlen(argv[i]);j++){
                    (*memory)=(*memory)*10;
                    (*memory)+=argv[i][j]-48;
                }
                (*memory)*=1024;
            }
        }
    }
}

void Errores(char top1,char top2,short int cant,char operacion,int op1,int op2,tmv mv){
    if (top1==0){
        VerificarOperacion(op1,mv);
    if (top2==0)
        VerificarOperacion(op2,mv);
    }
    if (cant==2 && (operacion<0 || operacion>12)){
        printf("ERROR(1): el codigo de operacion no existe");
        exit(-1);
    }
    if (cant==1 && (operacion<0 || operacion>13)){
        printf("ERROR(1): el codigo de operacion no existe");
        exit(-1);
    }
    if (cant==0 && (operacion!=15 || operacion!=14)){
        printf("ERROR(1): el codigo de operacion no existe");
        exit(-1);
    }
}

void CantidadOperandos(char instruccion,char *top1,char *top2,short int *cant){
    if (((instruccion>>4)&0x01)==0){          //miro el 5 bite si es 0 son 2 operandos
        *cant=2;
        *top1=(instruccion>>6)&0x03;   //saco el tipo de operando 2  B
        *top2=(instruccion>>4)&0x03;   //saco el tipo de operando 1  A
    }
    else
        if(((instruccion>>5)&0x07)==0x07)         //miro el 5 bite si hay 4 unos entonces no hay operandos
            *cant=0;
        else{
            *top1=(instruccion>>6)&0x03;   //saco el tipo de operando 2  A
            *cant=1;
        }
}

void SetearRegistros(tmv *mv,unsigned int vec[],unsigned int memory){
    unsigned int aux=0;
    int c=0,corrimiento;
    int i=0;

    if(vec[4]==0){
        corrimiento=0;
        (*mv).TablaRegistros[4]=-1;
        (*mv).TablaDeSegmentos[c].base=aux;
        (*mv).TablaDeSegmentos[c].size=vec[4];
        c++;
    }
    else{
        corrimiento=1;
        (*mv).TablaRegistros[4]=c<<16;
        (*mv).TablaDeSegmentos[c].base=aux;
        (*mv).TablaDeSegmentos[c].size=vec[4]+1;
        aux+=vec[4]+1;
        c++;
    }

    for (i;i<=6;i++){
        if (vec[i]>0){
            if (aux>memory){
                printf("ERROR(4): los segmentos ocupan mas de la memoria disponible");
                exit(-1);
            }
            if(i==0)
                (*mv).TablaRegistros[0]=(i+1)<<16;
            if (i==1)    //DATA SEGMENT
                (*mv).TablaRegistros[1]=(i+1)<<16;
            if (i==2)    //EXTRA SEGMENT
                (*mv).TablaRegistros[2]=(i+1)<<16;
            if (i==3){//STACK SEGMENT
                (*mv).TablaRegistros[3]=(i+1)<<16;
                (*mv).TablaRegistros[6]=(i+1)<<16;
                (*mv).TablaRegistros[6]+=vec[i]; //seteo el offset al SP igual al size
            } 
            if(i==5){   //ENTRY POINT
                entry=vec[i];
            }
            (*mv).TablaDeSegmentos[c].base=aux;
            (*mv).TablaDeSegmentos[c].size=vec[i];
            aux+=vec[i];
            c++;
        }else{
            if(i<5)
                (*mv).TablaRegistros[i]=-1;    
        }
    }
}