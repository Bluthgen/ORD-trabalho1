#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_REC_SIZE 512
#define DELIM_STR "|"


int getline (char *str, int size){
	int i = 0;
	fgets(str, size, stdin);
	for (i = 0; str[i] != '\n' && str[i] != '\0'; i++);
	str[i] = '\0';
	fseek(stdin, 0L, SEEK_END);
	return i;
}

short get_rec(char* recbuff, FILE* fd){

      int rec_lgth;

      if (fscanf(fd, "%d", &rec_lgth) == EOF) // get record length
         return(0);

      rec_lgth = fread(recbuff, sizeof(char), rec_lgth, fd); // read record

      recbuff[rec_lgth] = '\0';

      return(rec_lgth);
}

main(){

    FILE* fd;
    int rec_count, fld_count;
    int rec_lgth;
    char filename[15];
    char recbuff[MAX_REC_SIZE + 1];
    char * field;

    printf("Entre com o nome do arquivo a ser aberto: ");
    getline(filename, 15);

    if ((fd=fopen(filename,"rb")) == NULL) {
       printf("Erro na abertura do arquivo --- programa abortado\n");
       exit(1);
    }

    rec_count = 0;

	while ((rec_lgth = get_rec(recbuff, fd)) > 0) {

		printf("Registro %d\n", ++rec_count);
		fld_count = 0;

		field = strtok(recbuff,"|");
		while (field != NULL) {
			printf("\tCampo %d: %s\n",++fld_count, field);
			field = strtok(NULL, "|");
		}
		getchar();
    }

    fclose(fd);
}

