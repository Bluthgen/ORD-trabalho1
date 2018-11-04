#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_REC_SIZE 512
#define DELIM_STR "|"
#define DELIM '\n'

int vetor_aux[100];

typedef struct invertida{
    int id;
    int raca;
    long offset;
    long prox;
}invertida;

typedef struct secundaria{
    int id;
    long indice;
}secundaria;

typedef struct caes{
    char id_i[5];
    char id_r[5];
    char nome[20];
    char sexo[1];
}caes;

typedef struct indices{
    int id;
    long offset;
}indices;

secundaria listaSecundaria[100];
invertida listaInvertida[500];
int tamSec= 0, tamInvert= 0;
indices listaIndices[500];
int tamIndices= 0;

int readfield(FILE*, caes*);

int readfield(FILE* fd, caes* ind){
    char str[20];
    char c;
    int i = 0;
    for(int j = 0; j < 4; j++){
        i = 0;
        str[0]= '\0';
        c = fgetc(fd);
        while (c!= EOF && c != DELIM){
            str[i++] = c;
            c = fgetc(fd);
        }
        str[i] = '\0';
        switch(j){
            case 0: strcpy(ind->id_i, str);
                    break;
            case 1: strcpy(ind->id_r, str);
                    break;
            case 2: strcpy(ind->nome, str);
                    break;
            case 3: strcpy(ind->sexo, str);
                    break;
        }
    }
    return i;
}

void merge(indices arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;
    indices L[n1], R[n2];
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1+ j];
    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2)
    {
        if (L[i].id <= R[j].id)
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }
    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }
}


void mergeSort(indices arr[], int l, int r)
{
    if (l < r)
    {
        int m = l+(r-l)/2;
        mergeSort(arr, l, m);
        mergeSort(arr, m+1, r);
        merge(arr, l, m, r);
    }
}



void criaIndices(FILE* base){
    /*
        Tamanho de cada registro: sizeof(int) + sizeof(long)
    */
    int tam, id= 0, id_max, continua, i= -1;
    char buff[40], *temp;
    long offset;
    fseek(base, 0, SEEK_SET);
    while(1){
        i++;
        offset= ftell(base);
        continua = fread(&tam, sizeof(int), 1, base);
        if(!continua)
            break;
        fread(buff, sizeof(char), tam, base);
        buff[tam] = '\0';
        temp= strtok(buff, "|");
        id= atoi(temp);
        listaIndices[i].id= id;
        listaIndices[i].offset= offset;
        tamIndices= i;
    }
    mergeSort(listaIndices, 0, i-1);
}



void povoaArquivo(){
    caes individuo;
    FILE* input, *base;
    char s[200] = "";
    int tam;
    if ((input = fopen("individuos.txt", "r")) == NULL) {
        printf("Erro na criação do arquivo Individuo--- programa abortado\n");
        exit(1);
    }
    if ((base = fopen("base.txt", "w+")) == NULL) {
        printf("Erro na criação do arquivo Base--- programa abortado\n");
        exit(1);
    }
    fseek(base, 0, SEEK_SET);
    int fld_count = 0;
    while (readfield(input, &individuo) > 0){
          strcat(s, individuo.id_i);
          strcat(s, "|");
          strcat(s, individuo.id_r);
          strcat(s, "|");
          strcat(s, individuo.nome);
          strcat(s, "|");
          strcat(s, individuo.sexo);
          strcat(s, "|");
          tam = strlen(s);
          //printf("A\n");
          fwrite(&tam, sizeof(tam), 1, base);
          fwrite(s, sizeof(char), tam, base);
          strcpy(s, "");
    }
    fclose(input);
    criaIndices(base);
    fclose(base);

}

void gravaIndices(){
    FILE* ind;
    if ((ind = fopen("indices.txt", "w+")) == NULL) {
        printf("Erro na criação do arquivo Indices--- programa abortado\n");
        exit(1);
    }
    int id;
    long offset;
    for(int i= 0; i<= tamIndices; i++){
        id= listaIndices[i].id;
        offset= listaIndices[i].offset;
        fwrite(&id, sizeof(int), 1, ind);
        fwrite(&offset, sizeof(long), 1, ind);
    }
}

long buscaOffsetDoIndiceNoArquivo(int id){
    FILE* indices;
    if ((indices = fopen("indices.txt", "r")) == NULL) {
        printf("Erro na criação do arquivo Indices--- programa abortado\n");
        exit(1);
    }
    fseek(indices, (id - 1)*(sizeof(int)+sizeof(long)), SEEK_SET);
    int id_lido;
    long offset;
    fread(&id_lido, sizeof(int), 1, indices);
    if(id != id_lido){
        printf("ERRO - O indivíduo com id#%d não foi encontrado", id);
        fclose(indices);
        exit(1);
    }
    fread(&offset, sizeof(long), 1, indices);
    fclose(indices);
    return offset;
}

void carregaIndices(){
    FILE* indices;
    if ((indices = fopen("indices.txt", "r")) == NULL) {
        printf("Erro na criação do arquivo Indices--- programa abortado\n");
        exit(1);
    }
    int id, i= 0;
    long offset;
    while(fread(&id, sizeof(int), 1, indices)){
        fread(&offset, sizeof(long), 1, indices);
        listaIndices[tamIndices].id= id;
        listaIndices[tamIndices].offset= offset;
        tamIndices++;
    }
}

long buscaOffsetDoIndice(int id){
    int pos= tamIndices/2, min= 0, max= tamIndices;
    while(listaIndices[pos].id != id){
        if(max - min < 2){
            printf("Erro - não encontrado");
            exit(1);
        }
        if(listaIndices[pos].id < id)
            min= pos;
        else
            max= pos;
        pos= (min+max)/2;
    }
    return listaIndices[pos].offset;
}

struct caes buscaPorId(int id){
    long offset= buscaOffsetDoIndice(id);
    FILE* base;
    if ((base = fopen("base.txt", "r+")) == NULL) {
        printf("Erro na criação do arquivo Base--- programa abortado\n");
        exit(1);
    }
    char buff[40];
    int tam;
    fseek(base, offset, SEEK_SET);
    fread(&tam, sizeof(int), 1, base);
    fread(buff, sizeof(char), tam, base);
    buff[tam] = '\0';
    struct caes individuo;
    strcpy(individuo.id_i, strtok(buff, "|"));
    strcpy(individuo.id_r, strtok(NULL, "|"));
    strcpy(individuo.nome, strtok(NULL, "|"));
    strcpy(individuo.sexo, strtok(NULL, "|"));
    //Adicionar a raça depois que a busca por id_r estiver pronta
    printf("Encontrado individuo:\n\tId: %s\n\tNome: %s\n\tSexo: %s", individuo.id_i, individuo.nome, individuo.sexo);
    fclose(base);
    return individuo;
}


void monta_lista(){
    long offset=0;
    FILE* base;
    if ((base = fopen("base.txt", "r+")) == NULL) {
        printf("Erro na criação do arquivo Base--- programa abortado\n");
        exit(1);
    }
    struct caes individuo;
    char buff[40];
    int tam, cont=0;
    fseek(base, 0, SEEK_SET);
   int i=0, k=0;
    while(fread(&tam, sizeof(int), 1, base)){
        
        fseek(base, offset,SEEK_CUR);        
        fread(buff, sizeof(char), tam, base);
        buff[tam] = '\0';        
        strcpy(individuo.id_i, strtok(buff, "|"));
        strcpy(individuo.id_r, strtok(NULL, "|"));
        strcpy(individuo.nome, strtok(NULL, "|"));
        strcpy(individuo.sexo, strtok(NULL, "|"));
    
        listaInvertida[atoi(individuo.id_i)].id = atoi(individuo.id_i);
        listaInvertida[atoi(individuo.id_i)].raca = atoi(individuo.id_r);
        listaInvertida[atoi(individuo.id_i)].offset = listaIndices[atoi(individuo.id_i)].offset;
        vetor_aux[atoi(individuo.id_r)]=listaInvertida[atoi(individuo.id_i)].id;


        offset = buscaOffsetDoIndice(i+1);
    }
    int l = 55;
    while(l != 0 ){
        if(listaInvertida[l].id == vetor_aux[listaInvertida[l].raca]){
            listaInvertida[l].prox = -1;
        }
        else if(listaInvertida[l].id < vetor_aux[listaInvertida[l].raca] ){
            listaInvertida[l].prox = vetor_aux[listaInvertida[l].raca];
            vetor_aux[listaInvertida[l].raca] = listaInvertida[l].id;
        }
        printf("\nid: %d prox:%i",listaInvertida[l].id, listaInvertida[l].prox);
        l--;
    }


    fclose(base);

}

int main(){
    /*
        Use povoaArquivo() para importar uma nova lista de caes ou carregaIndices() para usar a mesma lista da sessâo anterior
    */
    //povoaArquivo();
    carregaIndices();
    //buscaPorId(1);
    monta_lista();

}
