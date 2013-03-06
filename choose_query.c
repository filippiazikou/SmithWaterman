#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h> 

int main(int argc, char **argv) {
	FILE *source;
	FILE * dest;
	int length, offset, size, thorivos, x, j, i, position;
	struct stat st;
	char *buf;
	char data[4] = {'A', 'T', 'C', 'G'};
	if (argc != 4 && argc != 5) {
		printf("USAGE: >.out <source_file> <dest_file> <query_length> <thorivos>\n");
		return -1;
	}
	
	/*Open source and destination file*/
	source = fopen(argv[1], "r");
	if (source == NULL) {
		printf("Fopen error for source\n");
		return -1;
	}
	dest = fopen(argv[2], "w");
	if (dest == NULL) {
		printf("Fopen error for source\n");
		return -1;
	}
	
	length = atoi(argv[3]);
	buf = malloc(sizeof(char)*length);
	if (buf == NULL) {
		printf("malloc error \n");
		return -1;
	}
	
	/*Find the size of source file*/
	stat(argv[1], &st);
	size = st.st_size;
	
	srand(getpid());
	offset = rand() % (size-length) ;
	
	fseek(source, offset, SEEK_SET);
	fread(buf, sizeof(char), length, source);
	
	if (argc == 4)
		fwrite(buf, sizeof(char), length, dest);	
	else if (argc == 5) {
		thorivos = atoi(argv[4]);
		thorivos = (length*thorivos)/100 + 1;
		for (i=0 ; i < thorivos ; i++) {
			offset = rand() % (length) ;
			for(j=0; j<4; j++) {
				if(buf[offset] == data[j]) {
					position = j;
					break;
				}
			}
			if( position < 3 )
				position ++;				
			else
				position = 0;

			buf[offset] = data[position];
		}
		
		fwrite(buf, sizeof(char), length, dest);
	}
	
	
	free(buf);
	fclose(source);
	fclose(dest);
}