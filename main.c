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
        fread(&version,sizeof(char),1,arch);//lee la version
        if(version==1){  //despues lee los dos bytes de el tamanio del codigo
            fread(&val,sizeof(char),1,arch);   //primer byte
            valor+=val;
            valor=(valor<<8);            //lo corre 8 bits(1 byte)
            fread(&val,sizeof(char),1,arch);  //lee el segundo byte
            valor+=val;      
            (*mv).TablaDeSegmentos[0].base=0;  //el CS empieza en 0
            (*mv).TablaDeSegmentos[0].size=valor;    //el tamanio es de el valor que leimos
            (*mv).TablaDeSegmentos[1].base=valor;    //El DS enmpieza en donde termina el CS
            (*mv).TablaDeSegmentos[1].size=sizeof((*mv).TablaMemoria)-valor; //el tamanio es todo menos lo que ocupa el codigo
        }
        while (!feof(arch)){  
            fread(&lec,sizeof(char),1,arch);
            (*mv).TablaMemoria[i]=lec;//cargo el CS con todo el codigo 
            i++;
        }
        fclose(arch);
    }else{
        printf("ERROR(0): el archivo no existe");
        exit(-1);
    }
}



void Lectura(tmv *mv){
    char top1,top2;  //top1=B , top2=A
    unsigned char operacion;
    short int cant;
    long int op1,op2;
    int corte;
    char dbg;
    while ((*mv).TablaRegistros[5]!=(*mv).TablaDeSegmentos[1].base)
    {
        op1=0;
        op2=0;
        operacion=(*mv).TablaMemoria[(*mv).TablaRegistros[5]]&0xF;      //lee la operacion
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



void Cant_op(char instruccion,char *top1,char *top2,short int *cant){
    
    *top1=(instruccion>>6)&0x03;   //saco el tipo de operando 1
    *top2=(instruccion>>4)&0x03;   //saco el tipo de operando 2

    if (((instruccion>>4)&0x01)==0)                 //miro el 5 bite si es 0 son 2 operandos
        *cant=2;
    else
        if((instruccion>>5)&0x0F==0x0F)         //miro el 5 bite si hay 4 unos entonces no hay operandos
            *cant=0;
        else
            *cant=1;
}