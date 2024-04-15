#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Funciones.h"
#include "disasembler.h"


void InicializarRegistros(tmv *mv);
void Inicia_memoria(char *dire,tmv *mv);
void Lectura(tmv *mv);
void Cant_op(char instruccion,char *top1,char *top2,short int *cant);

int main(int argc,char *argv[]){
    tmv mv;
    InicializarRegistros(&mv);
    Inicia_memoria(argv[1],&mv);
    //if(argv[2]!=NULL && strcmp(argv[2],"-d")==0)
        Dissasembler_mostrar(mv);
    Lectura(&mv);
}



void InicializarRegistros(tmv *mv){
        (*mv).TablaRegistros[0]=0;//cs
        (*mv).TablaRegistros[1]=0x00010000;//ds
        (*mv).TablaRegistros[5]=0;//ip
        (*mv).TablaRegistros[8]=0;//cc
        (*mv).TablaRegistros[9]=0;//ac
}



void Inicia_memoria(char *dire,tmv *mv){
    char version,lec,nombre[5];
    FILE *arch=fopen(dire,"rb");
    int i=0;
    unsigned short int valor=0;
    unsigned char val=0;
    if (arch!=NULL){
        fread(nombre,sizeof(nombre),1,arch);//lee el titulo
        fread(&version,sizeof(char),1,arch);//lee la version
        fread(&val,sizeof(char),1,arch);
        valor+=val;
        valor=(valor<<8);
        fread(&val,sizeof(char),1,arch);
        valor+=val;
        (*mv).TablaDeSegmentos[0].base=0;
        (*mv).TablaDeSegmentos[0].size=valor;
        (*mv).TablaDeSegmentos[1].base=valor+1;
        (*mv).TablaDeSegmentos[1].size=sizeof((*mv).TablaMemoria)-valor;
        if (version==1)
            while (!feof(arch)){
                fread(&lec, sizeof(char), 1, arch);
                (*mv).TablaMemoria[i]=lec;//cargo el CS
                i++;
            }
        fclose(arch);
    }else{
        printf("ERROR(0): el archivo no existe");
        exit(-1);
    }
}

void Lectura(tmv *mv){
    char top1,top2,operacion;
    short int cant;
    long long int op1,op2;
    int corte;
    while ((*mv).TablaRegistros[5]!=(*mv).TablaDeSegmentos[1].base)
    {
        op1=0;
        op2=0;
        operacion=(*mv).TablaMemoria[(*mv).TablaRegistros[5]]&0x0F;
        Cant_op((*mv).TablaMemoria[(*mv).TablaRegistros[5]],&top1,&top2,&cant);
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
        }else{
            for (int i=1;i<=corte;i++){
                (*mv).TablaRegistros[5]++;
                op1+=((*mv).TablaMemoria[(*mv).TablaRegistros[5]])&0x000000FF;
                if (i!=corte)
                    op1=op1<<8;
            }
        }
        (*mv).TablaRegistros[5]++;// le sumo uno mas al ip antes de que se ejecute la operacion.
        if (top1==0){
            Integridad_op(op1,*mv);
            if (top2==0)
                Integridad_op(op2,*mv);
        }
        if (operacion<0 || operacion>12){
            if(operacion!=15)
                printf("ERROR(1): el codigo de operacion no existe");
            exit(-1);
        }
        if(cant!=0)
            Pre_Funcion(cant,op1,op2,top1,top2,operacion,mv);
        else
            (*mv).TablaRegistros[5]=(*mv).TablaDeSegmentos[1].base;
    }
}

void mue(tmv mv){
//int pos=(mv).TablaDeSegmentos[1].base;
//printf("[0] %d %d %d %d \n",(mv).TablaMemoria[pos],(mv).TablaMemoria[pos+1],(mv).TablaMemoria[pos+2],(mv).TablaMemoria[pos+3]);

for(int i=0;i<=250;i++)
    printf("i=%d  =%2.2X \n",i,mv.TablaMemoria[i]);
}
void Cant_op(char instruccion,char *top1,char *top2,short int *cant){
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