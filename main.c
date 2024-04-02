#include <stdio.h>
#include <stdlib.h>
#include "Funciones.h"



void Inicia_registro(tmv *mv);
void Inicia_memoria(char *dire,tmv *mv,unsigned int memory);
void Lectura(tmv *mv);

int main(int argc,char *argv[]){
    tmv mv;
    unsigned short int diss=0;
    Lectura_argumentos(argc,argv,&diss,&memory,&mv);
    Inicia_registro(&mv);
    Inicia_memoria(argv[1],&mv,memory);
    if(diss==1)
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



void Inicia_memoria(char *dire,tmv *mv,unsigned int memory){
    char version,lec,nombre[5];
    FILE *arch=fopen(dire,"rb");
    int i=0;
    unsigned short int valor=0;
    unsigned char val=0;
    if (arch!=NULL){
        fread(nombre,sizeof(nombre),1,arch);//lee el titulo
            (*mv).cant_seg++;
            fread(&val,sizeof(char),1,arch);
            valor+=val;
            valor=(valor<<8);
            fread(&val,sizeof(char),1,arch);
            valor+=val;
            (*mv).TablaDeDatos[0].pos=0;
            (*mv).TablaDeDatos[0].tamano=valor;
            (*mv).TablaDeDatos[1].pos=valor;
            (*mv).TablaDeDatos[1].tamano=sizeof((*mv).TablaMemoria)-valor;
        
        while (!feof(arch)){
            fread(&lec,sizeof(char),1,arch);
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



    while ((*mv).TablaRegistros[5]!=(*mv).TablaDeSegmentos[1].base){

    }
}



void Cant_op(char instruccion,char *top1,char *top2,short int *cant){
    *top1=(instruccion>>6)&0x03;//haciendo el corrimiento me quedaria 00XX donde XX es el valor del operando
    *top2=(instruccion>>4)&0x03;//haciendo la operacion tambien quedaria 00XX.
    if (*top1==3)
        *cant=0;
    else
        if (*top2==3)
            *cant=1;
        else
            *cant=2;
}