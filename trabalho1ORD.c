#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_REC_SIZE 512
#define DELIM_STR "|"
#define DELIM '\n'

int vetor_aux[100];


typedef struct racas{
    int id;
    char raca[30];
}racas;

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

secundaria listaSecundaria[100];
invertida listaInvertida[500];
int tamSec= 0, tamInvert= 0;
indices listaIndices[500];
int tamIndices= 0;
racas listaRacas[18];
racas listaNomeRacas[18];

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

int getline2 (char *str, int tam){
	int i = 0;
	fgets(str, tam, stdin);
	//Tira o enter do fim da string, se houver
	for (i = 0; str[i] != '\n' && str[i] != '\0'; i++);
	str[i] = '\0';
	//avanca o ponteiro do stdin para o final
	fseek(stdin, 0L, SEEK_END);
	return i;
}

int readnome(FILE* fd, char* str){
    //char str[25];
    char c;
    int i = 0;
    str[0]= '\0';
    c = fgetc(fd);
    while (c!= EOF && c != DELIM){
        str[i++] = c;
        c = fgetc(fd);
    }
    str[i] = '\0';
    return i;
}



void merge(indices arr[], int l, int m, int r){
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
    while (i < n1 && j < n2){
        if (L[i].id <= R[j].id){
            arr[k] = L[i];
            i++;
        }else{
            arr[k] = R[j];
            j++;
        }
        k++;
    }
    while (i < n1){
        arr[k] = L[i];
        i++;
        k++;
    }
    while (j < n2){
        arr[k] = R[j];
        j++;
        k++;
    }
}

void mergeSort(indices arr[], int l, int r){
    if (l < r){
        int m = l+(r-l)/2;
        mergeSort(arr, l, m);
        mergeSort(arr, m+1, r);
        merge(arr, l, m, r);
    }
}

void leNomesRacas(){
    FILE* arq;
    if ((arq = fopen("nome-racas.txt", "r")) == NULL) {
        printf("Erro na criacão do arquivo Racas--- programa abortado\n");
        exit(1);
    }
    char buff[40];
    for(int i= 0; i<18; i++){
        readnome(arq, buff);
        listaNomeRacas[i].id= atoi(strtok(buff, " "));
        strcpy(listaNomeRacas[i].raca, strtok(NULL, "\n"));
    }
    fclose(arq);
}

char* getNomeRaca(int id){
    if(id<1 || id>18)
        return "";
    return listaNomeRacas[id-1].raca;
}

int getIdRaca(char* nome){ 
    if(strlen(nome)){
        int len= strlen(nome);
        for(int j=0; j<18; j++){            
            if(!strcmp(listaNomeRacas[j].raca, nome)){
                return j+1;
            }        
        }
    }
    return -1;
}


void gravaIndices(){
    FILE* ind;
    if ((ind = fopen("indices.txt", "w+")) == NULL) {
        printf("Erro na criação do arquivo Indices--- programa abortado\n");
        exit(1);
    }
    int id, num= tamIndices;
    long offset;
    fwrite(&num, sizeof(int), 1, ind);
    for(int i= 0; i<= num; i++){
        id= listaIndices[i].id;
        offset= listaIndices[i].offset;
        fwrite(&id, sizeof(int), 1, ind);
        fwrite(&offset, sizeof(long), 1, ind);
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
    int id, num;
    fread(&num, sizeof(int), 1, indices);
    long offset;
    for(int i= 0; i<num; i++){
        fread(&id, sizeof(int), 1, indices);
        fread(&offset, sizeof(long), 1, indices);
        listaIndices[tamIndices].id= id;
        listaIndices[tamIndices].offset= offset;
        tamIndices++;
    }
}

int getIndice(int id) {
   int lowerBound = 0;
   int upperBound = tamIndices -1;
   int midPoint = -1;
   int comparisons = 0;
   int index = -1;

   while(lowerBound <= upperBound) {
      comparisons++;

      midPoint = lowerBound + (upperBound - lowerBound) / 2;
      if(listaIndices[midPoint].id == id) {
         index = midPoint;
         break;
      } else {
         if(listaIndices[midPoint].id < id) {
            lowerBound = midPoint + 1;
         }
         else {
            upperBound = midPoint -1;
         }
      }
   }
   return index;
}

long buscaOffsetDoIndice(int id){
    int index= getIndice(id);
    if(index>=0){
        return listaIndices[index].offset;
    }
    return -1;
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
    printf("Encontrado individuo:\n\tId: %s\n\tRaca: %s\n\tNome: %s\n\tSexo: %s\n", individuo.id_i, getNomeRaca(atoi(individuo.id_r)), individuo.nome, individuo.sexo);
    fclose(base);
    return individuo;
}


void buscaPorRaca(int id_r){
    int i = (int)listaSecundaria[id_r].indice;
    printf("%d", i);
    while(i > -1){
        int id = listaInvertida[i-1].id;
        buscaPorId(id);

        i = listaInvertida[i-1].prox;
    }
}

void buscaPorNomeDaRaca(char* nome){ 
    int id_r= getIdRaca(nome);
    if(id_r < 0)
        printf("Raça não encontrada");
    int i = (int)listaSecundaria[id_r].indice;
    while(i > -1){
        int id = listaInvertida[i-1].id;
        buscaPorId(id);
        i = listaInvertida[i-1].prox;
    }
}

void povoaArquivo(char* nome){
    caes individuo;
    FILE* input, *base;
    if(nome == ""){
        nome= "arquivo.txt";
    }
    char s[200] = "";
    int tam;
    if ((input = fopen(nome, "r")) == NULL) {
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
   int i=0, k=0, p=0;
    while(fread(&tam, sizeof(int), 1, base)){
        cont++;
        fseek(base, offset,SEEK_CUR);
        fread(buff, sizeof(char), tam, base);
        buff[tam] = '\0';
        strcpy(individuo.id_i, strtok(buff, "|"));
        strcpy(individuo.id_r, strtok(NULL, "|"));
        strcpy(individuo.nome, strtok(NULL, "|"));
        strcpy(individuo.sexo, strtok(NULL, "|"));

        listaInvertida[p].id = atoi(individuo.id_i);
        listaInvertida[p].raca = atoi(individuo.id_r);
        listaInvertida[p].offset = listaIndices[atoi(individuo.id_i)-1].offset+4;
        vetor_aux[atoi(individuo.id_r)]=listaInvertida[p].id;

        if(listaSecundaria[atoi(individuo.id_r)].id == 0){
            listaSecundaria[atoi(individuo.id_r)].id = atoi(individuo.id_r);
            listaSecundaria[atoi(individuo.id_r)].indice = p+1;
        }
        offset = buscaOffsetDoIndice(i+1);
        p++;
    }
    //for(int v=1;v<=18;v++){
    //  printf("raca: %ld, %d\n",listaSecundaria[v].id,listaSecundaria[v].indice);
    //}
    int l = cont;
    while(l >= 0 ){
        if(listaInvertida[l].id == vetor_aux[listaInvertida[l].raca]){
            listaInvertida[l].prox = -1;
        }
        else if(listaInvertida[l].id < vetor_aux[listaInvertida[l].raca] ){
            listaInvertida[l].prox = vetor_aux[listaInvertida[l].raca];
            vetor_aux[listaInvertida[l].raca] = listaInvertida[l].id;
        }
        //printf("ID:%ld, PROX:%d\n", listaInvertida[l].id, listaInvertida[l].prox);
        l--;
    }


    fclose(base);

}


void dialogo(){
    printf("Bem vindo(a) a aplicação!\n \
            Ainda não foi cadastrado nenhum cão, por favor digite o nome de um arquivo:\n");
    char input[40];
    getline2(input, 40);
    povoaArquivo(input);
    int flag= -1;
    while(1){
        printf("O que você deseja fazer?\n \
                0: Sair\n \
                1: Buscar um cão pelo seu id\n \
                2: Buscar um cão pelo sua raça\n");
        input[0]= '\0';

        int flag= -1;

        getline2(input,1);
        switch(input[0]){
            case '0': exit(1);
            case '1': flag= 1;
                      break;
            case '2': flag= 2;
                      break;
            default: flag= -1;
        }
        if(flag == 1){
            printf("Qual é o id do cão a ser buscado?\n");
            getline2(input, 2);

        }
    }

}

int main(){
    /*
        Use povoaArquivo() para importar uma nova lista de caes ou carregaIndices() para usar a mesma lista da sessâo anterior
    */
   // leNomesRacas();
    povoaArquivo("individuos.txt");
    gravaIndices();
    carregaIndices();

    monta_lista();
    leNomesRacas();
    //buscaPorId(3);
    //buscaPorId(7);
    //buscaPorId(9);
    //buscaPorId(5);
    //buscaPorId(6);
    //buscaPorRaca(3);
    //buscaPorRaca(12);
    buscaPorNomeDaRaca("BEAGLE");
    //buscaPorId(10);


}
