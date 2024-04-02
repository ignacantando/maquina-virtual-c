
#define MAX_M 16384



typedef struct{
    short int base,size;
}tds; //tabla de descriptores de segmento

typedef struct{
    char TablaMemoria[MAX_M];
    short int TablaRegistros[16];
    tds TablaDeSegmentos[2];
}tmv; //tipo de maquina virtual


unsigned int memory=16384;