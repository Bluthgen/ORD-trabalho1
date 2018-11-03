#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_REC_SIZE 512
#define DELIM_STR "|"
#define DELIM '\n'

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

indices listaIndices[500];
int tamIndices= 0;

int readfield(FILE*, caes*);
int getline (char*, int);



void read() {

    FILE* fd;
    char s[30];
    char filename[15];
    int fld_count;

    printf("Entre com o nome do arquivo a ser lido: ");
    getline(filename, 15);
    if ((fd = fopen(filename, "r")) == NULL) {
       printf("Erro na abertura do arquivo --- programa abortado\n");
       exit(1);
    }
    /* loop do programa principal -- chama a função 'readfield'
	   enqto a função tiver sucesso (retorno > 0) */
    fld_count = 0;
   // while (readfield(fd,s) > 0)
          printf("\tcampo #%d: %s\n", ++fld_count, s);

    fclose(fd);
}

/* função que lê um campo na string s
   e retorna o tamanho da string lida
*/
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

/* função que lê uma linha do stdin para a string str
*/
int getline (char *str, int tam){
	int i = 0;
	fgets(str, tam, stdin);
	//Tira o enter do fim da string, se houver
	for (i = 0; str[i] != '\n' && str[i] != '\0'; i++);
	str[i] = '\0';
	//avanca o ponteiro do stdin para o final
	fseek(stdin, 0L, SEEK_END);
	return i;
}

int getline2 (char*, int);

int write() {
    char first[30], last[30], address[30], city[20], state[15], zip[9];
    char filename[15];
    FILE* fd;

    printf("Entre com o nome do arquivo a ser criado: ");
    getline2(filename, 15);

    if ((fd = fopen(filename,"w")) == NULL) {
        printf("Erro na criação do arquivo --- programa abortado\n");
        exit(1);
    }

    printf("\n\nDigite o Sobrenome, ou <ENTER> para sair\n>>>");
    getline2(last, 30);

    while(strlen(last) > 0) {
        printf("\nPrimeiro nome\n>>>");
        getline(first, 30);
        printf("\nEndereco\n>>>");
        getline(address, 30);
        printf("\nCidade\n>>>");
        getline(city, 20);
        printf("\nEstado\n>>>");
        getline(state, 15);
        printf("\nCEP\n>>>");
        getline2(zip, 9);

        /* envia as strings para o arquivo */
        fputs(last, fd);
        fputs(DELIM_STR, fd);
        fputs(first, fd);
        fputs(DELIM_STR, fd);
        fputs(address, fd);
        fputs(DELIM_STR, fd);
        fputs(city, fd);
        fputs(DELIM_STR, fd);
        fputs(state, fd);
        fputs(DELIM_STR, fd);
        fputs(zip, fd);
        fputs(DELIM_STR, fd);

        /* começa a próxima entrada */
        printf("\n\nDigite o Sobrenome, ou <ENTER> para sair\n>>>");
        getline2(last, 30);
    }

    /* fechar o arquivo */
    fclose(fd);
}

/* função que lê uma linha do stdin para a string str
*/
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

int main(){
    /*
        Use povoaArquivo() para importar uma nova lista de caes ou carregaIndices() para usar a mesma lista da sessâo anterior
    */
    //povoaArquivo();
    carregaIndices();
    buscaPorId(1);

}

