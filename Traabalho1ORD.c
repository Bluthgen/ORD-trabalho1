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
int tamSec= 18, tamInvert= 0;
indices listaIndices[500];
int tamIndices= 0;
racas listaRacas[18];
racas listaNomeRacas[100];
int tamNomeRacas= 0;
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
 void leNomesRacas(char* nome){
    FILE* arq;
    if(!strlen(nome))
        nome="lista de racas.txt";
    if ((arq = fopen(nome, "r")) == NULL) {
        printf("Erro na criacão do arquivo Racas--- programa abortado\n");
        exit(1);
    }
    char buff[40];
    while(1){
        readnome(arq, buff);
        if(!strlen(buff))
            break;
        listaNomeRacas[tamNomeRacas].id= atoi(strtok(buff, " "));
        strcpy(listaNomeRacas[tamNomeRacas].raca, strtok(NULL, "\n"));
        tamNomeRacas++;
    }
    fclose(arq);
}
 char* getNomeRaca(int id){
    if(id<1 || id>tamNomeRacas)
        return "";
    return listaNomeRacas[id-1].raca;
}
 int getIdRaca(char* nome){
    if(strlen(nome)){
        int len= strlen(nome);
        for(int j=0; j<tamNomeRacas; j++){
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
        id = listaIndices[i].id;
        offset= listaIndices[i].offset+4;
        fprintf(ind,"%d",id);
        fprintf(ind,"|");
        fprintf(ind,"%d", offset);
        fprintf(ind,"|");
    }
}
 void gravaInvertida(){
    FILE* ind;
    if ((ind = fopen("invertida.txt", "w+")) == NULL) {
        printf("Erro na criação do arquivo Invertida--- programa abortado\n");
        exit(1);
    }
    int id, raca, num= tamInvert;
    long offset, prox;
    fwrite(&num, sizeof(int), 1, ind);
    for(int i= 0; i<= num; i++){
        id= listaInvertida[i].id;
        raca= listaInvertida[i].raca;
        offset= listaInvertida[i].offset;
        prox= listaInvertida[i].prox;
        fprintf(ind,"%d",id);
        fprintf(ind,"|");
        fprintf(ind,"%d",raca);
        fprintf(ind,"|");
        fprintf(ind,"%d", offset);
        fprintf(ind,"|");
        fprintf(ind,"%d",prox);
        fprintf(ind,"|");
    }
}
 void gravaSecundaria(){
    FILE* ind;
    if ((ind = fopen("secundaria.txt", "w+")) == NULL) {
        printf("Erro na criação do arquivo Secundaria--- programa abortado\n");
        exit(1);
    }
    int id, num= tamNomeRacas;
    long indice;
    for(int i= 0; i<= num; i++){
        id= listaSecundaria[i].id;
        indice= listaSecundaria[i].indice;
        printf("%d, %d\n", listaSecundaria[i].id, listaSecundaria[i].indice);
        fprintf(ind,"%d",id);
        fprintf(ind,"|");
        fprintf(ind,"%d",indice);
        fprintf(ind,"|");
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
 void carregaInvertida(){
    FILE* invert;
    if ((invert = fopen("invertida.txt", "r")) == NULL) {
        printf("Erro na criação do arquivo Invertida--- programa abortado\n");
        exit(1);
    }
    int id,raca,num;
    fread(&num, sizeof(int), 1, invert);
    long offset, prox;
    for(int i= 0; i<num; i++){
        fread(&id, sizeof(int), 1, invert);
        fread(&raca, sizeof(int), 1, invert);
        fread(&offset, sizeof(long), 1, invert);
        fread(&prox, sizeof(long), 1, invert);
        listaInvertida[tamInvert].id= id;
        listaInvertida[tamInvert].raca= raca;
        listaInvertida[tamInvert].offset= offset;
        listaInvertida[tamInvert].prox= prox;
        tamIndices++;
    }
}
 void carregaSecundaria(){
    FILE* secundaria;
    if ((secundaria = fopen("secundaria.txt", "r")) == NULL) {
        printf("Erro na criação do arquivo Secundaria--- programa abortado\n");
        exit(1);
    }
    int id, num;
    fread(&num, sizeof(int), 1, secundaria);
    long indice;
    for(int i= 0; i<num; i++){
        fread(&id, sizeof(int), 1, secundaria);
        fread(&indice, sizeof(long), 1, secundaria);
        listaSecundaria[tamIndices].id= id;
        listaSecundaria[tamIndices].indice= indice;
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
    if(id >= tamInvert+1){
        printf("\nIndividuo não encontrado!--- programa abortado\n");
        exit(1);
    }
    else{
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
            printf("%d, %d \n", listaSecundaria[atoi(individuo.id_r)].id, listaSecundaria[atoi(individuo.id_r)].indice);
        }
        p++;
    }
    tamInvert = p;
    int l = cont;
    while(l >= 0 ){
        if(listaInvertida[l].id == vetor_aux[listaInvertida[l].raca]){
            listaInvertida[l].prox = -1;
        }
        else if(listaInvertida[l].id < vetor_aux[listaInvertida[l].raca] ){
            listaInvertida[l].prox = vetor_aux[listaInvertida[l].raca];
            vetor_aux[listaInvertida[l].raca] = listaInvertida[l].id;
        }
        l--;
    }
     fclose(base);
 }
 void printRacas(){
	for(int i=0; i< tamNomeRacas; i++){
		printf("%s -> Digite %d\n", listaNomeRacas[i].raca, listaNomeRacas[i].id);
	}
}
 void grava(){
	gravaIndices();
	gravaInvertida();
	gravaSecundaria();
}

 void preparaParaLerOutra(){
    for(int i= 0; i<tamSec; i++){
        listaSecundaria[i].id= listaSecundaria[i].indice= 0;
    }
    for(int i= 0; i<tamInvert; i++){
        listaInvertida[i].id= listaInvertida[i].offset= listaInvertida[i].raca= 0;
        listaInvertida[i].id= -1;
    }
    for(int i= 0; i<tamIndices; i++){
        listaIndices[i].id= listaIndices[i].offset= 0;
    }
    tamSec= 0;
    tamInvert= 0;
    tamIndices= 0;
}
void dialogo(){
    printf("Bem vindo(a) a aplicacao!\n \
            Ainda nao foi cadastrado nenhum cao, por favor digite o nome de um arquivo:\n");
    char input[40];
    getline2(input, 40);
    povoaArquivo(input);
    gravaIndices();
    carregaIndices();
    leNomesRacas("");
    monta_lista();
    int flag=1;
    int opcao;
    while(flag){
        printf("O que vc deseja fazer?\n \
                0: Sair\n \
                1: Buscar um cão pelo seu id\n \
                2: Buscar um cão pelo sua raça\n \
                3: Ler um novo arquivo\n");
        input[0]= '\0';

        scanf("%d", &opcao);
        switch(opcao){
            case 0: flag=0;
              grava();
              exit(1);
            case 1:
                printf("Qual e o id do cao a ser buscado?\n");
                int id;
                scanf("%d", &id);
                buscaPorId(id);
                break;
            case 2:
                printf("Qual é a raça a ser buscada?\n");
                printRacas();
                int id_r;
                scanf("%d", &id_r);
                buscaPorRaca(id_r);
                break;
            case 3: ;
                printf("Qual o nome do arquivo a ser lido?\n");
                getline2(input, 40);
                preparaParaLerOutra();
                povoaArquivo(input);
                printf("\nLido segundo arquivo\n");
                gravaIndices();
                carregaIndices();
                monta_lista();
                printf("\nMontado\n");
                break;
        }
    }
}
 int main(){
    dialogo();

}
