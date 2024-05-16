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
    InicializarRegistros(&mv);
    InicializarMemoria(argv[1],&mv);
    if(argv[2]!=NULL && strcmp(argv[2],"-d")==0)
        MostrarDissasembler(mv);
    Lectura(&mv);
}



void InicializarRegistros(tmv *mv){
        (*mv).TablaRegistros[0]=0;//cs
        (*mv).TablaRegistros[1]=0x00010000;//ds
        (*mv).TablaRegistros[5]=0;//ip
        (*mv).TablaRegistros[8]=0;//cc
        (*mv).TablaRegistros[9]=0;//ac
}


void InicializarMemoria(char *direccion,tmv *mv,unsigned int memory){
    char codigo,version,nombre[5];
    FILE *arch=fopen(direccion,"rb");
    int i=0;
    unsigned short int valor=0;
    unsigned char val=0;
    if (arch!=NULL){
        fread(nombre,sizeof(nombre),1,arch);//lee el titulo
        fread(&version,sizeof(char),1,arch);//lee la version
        if (version==2){
            unsigned int vec[6];
            for (int j=0;j<5;j++){
                valor=0;
                fread(&val,sizeof(char),1,arch);
                valor+=val;
                fread(&val,sizeof(char),1,arch);
                valor=valor<<8;
                valor+=val;
                vec[j]=valor;  //guarda el tamanio del segmento
            }
            SeteoV2(mv,vec,memory);
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
        }
        while (!feof(arch)){
                fread(&codigo, sizeof(char), 1, arch);
                (*mv).TablaMemoria[i]=codigo;//cargo el CS
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
    while ((*mv).TablaRegistros[5]!=(*mv).TablaDeSegmentos[1].base)
    {
        op1=0;
        op2=0;
        operacion=(*mv).TablaMemoria[(*mv).TablaRegistros[5]]&0x0F;
        CantidadOperandos((*mv).TablaMemoria[(*mv).TablaRegistros[5]],&top1,&top2,&cant);
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
            VerificarOperacion(op1,*mv);
            if (top2==0)
                VerificarOperacion(op2,*mv);
        }
        if (operacion<0 || operacion>12){
            if(operacion!=15)
                printf("ERROR(1): el codigo de operacion no existe");
            exit(-1);
        }
        if(cant!=0)
            Prefuncion(cant,op1,op2,top1,top2,operacion,mv);
        else
            (*mv).TablaRegistros[5]=(*mv).TablaDeSegmentos[1].base;
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


void SeteoV2(tmv *mv,unsigned int vec[],unsigned int memory){
    unsigned int aux=0;
    int c=0;
    int i=0;
    for (i;i<=4;i++){
        if (vec[i]>0){
            if (aux>memory){
                printf("ERROR(4): los segmentos ocupan mas de la memoria disponible");
                exit(-1);
            }
            if (i==1)    //DATA SEGMENT
                (*mv).TablaRegistros[1]=c<<16;
            if (i==2)    //EXTRA SEGMENT
                (*mv).TablaRegistros[2]=c<<16;
            if (i==3){//STACK SEGMENT
                (*mv).TablaRegistros[3]=c<<16;
                (*mv).TablaRegistros[6]=c<<16;
                (*mv).TablaRegistros[6]+=vec[i]; //seteo el offset al SP igual al size
            } 
            if (i==4){   //CONSTANT SEGMENT
                 (*mv).TablaRegistros[4]=c<<16;
            }
            if(i==5){   //ENTRY POINT

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