#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Funciones.h"
#include "disasembler.h"


void InicializarRegistros(tmv *mv);
void Inicia_memoria(char *dire,tmv *mv);
void Lectura(tmv *mv);
void Cant_op(char instruccion,char *top1,char *top2,short int *cant);
void Lectura_argumentos(int argc,char *argv[],short int *diss,tmv *mv);

int main(int argc,char *argv[]){
    tmv mv;
    unsigned short int diss=0;
    Lectura_argumentos(argc,argv,&diss,&mv);
    InicializarRegistros(&mv);
    Inicia_memoria(argv[1],&mv);
    //if(diss==1)
        Dissasembler_mostrar(mv);
    //printf("qq");
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
        (*mv).TablaDeSegmentos[1].base=valor;
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


/*
void Lectura(tmv *mv){
    char top1,top2;  //top1=B , top2=A
    unsigned char operacion;
    short int cant;
    long int op1,op2;
    int corte;
    while ((*mv).TablaRegistros[5]!=(*mv).TablaDeSegmentos[1].base)
    {
        op1=0;
        op2=0;
        operacion=(*mv).TablaMemoria[(*mv).TablaRegistros[5]]&0xF;      //lee la operacion
        Muestra_mem(0);
        Muestra_byte((*mv).TablaMemoria[0]);
        Cant_op((*mv).TablaMemoria[(*mv).TablaRegistros[5]],&top1,&top2,&cant); //lee la cantidad de operandos
        corte=top1^0x3;//le asigno el opuesto del tipo1 el cual representa el tamanio
        if (cant==2){
            for (int i=1;i<=corte;i++){     //si es de memoria corte van a ser 3 bytes los cuales estaran uno abajo del otro,inmediato 2 y registro 1
                (*mv).TablaRegistros[5]++;  //lee el siguiente byte
                op1+=((*mv).TablaMemoria[(*mv).TablaRegistros[5]])&0x000000FF;  //lee el byte
                if (i!=corte)
                    op1=op1<<8;    //corre un byte 
            }
            corte=top2^0x3;//le asigno el opuesto del tipo2 el cual representa la longitud
            for (int i=1;i<=corte;i++){
                (*mv).TablaRegistros[5]++;
                op2+=((*mv).TablaMemoria[(*mv).TablaRegistros[5]])&0x000000FF;
                if (i!=corte)
                    op2=op2<<8;
            }
        }else{
            if (cant==0){
                op1=operacion;
            }else
                for (int i=1;i<=corte;i++){
                    (*mv).TablaRegistros[5]++;
                    op1+=((*mv).TablaMemoria[(*mv).TablaRegistros[5]])&0x000000FF;
                    if (i!=corte)
                        op1=op1<<8;
                }
        }
        (*mv).TablaRegistros[5]++;// le sumo uno mas al ip antes de que se ejecute la operacion.
        //chequeo_errores(top1,top2,cant,operacion,op1,op2,*mv);
        if (cant==0 && op1==0)
            (*mv).TablaRegistros[5]=(*mv).TablaDeSegmentos[1].base;   //esto es un STOP y pone el IP en el DS asi termina el programa
        else{
            Pre_Funcion(cant,op1,op2,top1,top2,operacion,mv);//con esto arracamos la parte de hacer la funcion
        }
    }
}
*/
void Lectura(tmv *mv){
    char top1,top2,operacion;
    short int cant;
    long int op1,op2;
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
            Pre_Funcion(cant,op1,op2,top1,top2,operacion,mv);//con esto arracamos la parte de hacer la funcion
        else
            (*mv).TablaRegistros[5]=(*mv).TablaDeSegmentos[1].base;
       // mue(*mv);
    }
}


void Cant_op(char instruccion,char *top1,char *top2,short int *cant){

    *top1=5;
    *top2=5;
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

void Lectura_argumentos(int argc,char *argv[],short int *diss,tmv *mv){
    for (int i=2;i<argc;i++){
        if (strcmp(argv[i],"-d")==0)   // mostrar el código Assembler correspondiente al código máquina cargado en la memoria principal.
            (*diss)=1;
    }
}
