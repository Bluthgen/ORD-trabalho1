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

void criaIndices(FILE* base){
    /*
        Tamanho de cada registro: sizeof(int) + sizeof(long)
        Assume que os id_i estão em ordem
        Se algum id_i for pulado, adiciona uma entrada para o individuo faltando com valores -1 para ficar mais facil de buscar
    */

    FILE* indices;
    if ((indices = fopen("indices.txt", "w+")) == NULL) {
        printf("Erro na criação do arquivo Indices--- programa abortado\n");
        exit(1);
    }
    int tam, id= 0, id_ant, continua;
    char buff[40], *temp;
    long offset;
    fseek(base, 0, SEEK_SET);
    while(1){
        offset= ftell(base);
        continua = fread(&tam, sizeof(int), 1, base);
        if(!continua)
            break;
        fread(buff, sizeof(char), tam, base);
        buff[tam] = '\0';
        temp= strtok(buff, "|");
        id_ant= id;
        id= atoi(temp);
        if(id != id_ant + 1){
            fseek(base, (-1)*(sizeof(char)*tam + sizeof(int)), SEEK_CUR);
            id= -1;
            offset= -1;
            fwrite(&id, sizeof(int), 1, indices);
            fwrite(&offset, sizeof(long), 1, indices);
            id= id_ant+1;
            continue;
        }
        fwrite(&id, sizeof(int), 1, indices);
        fwrite(&offset, sizeof(long), 1, indices);
    }
    fclose(indices);
}

long buscaOffsetDoIndice(int id){
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
    povoaArquivo();
    buscaPorId(3);
}

