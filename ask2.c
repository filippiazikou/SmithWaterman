#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h> 

#define TRUE 1
#define FALSE 2
#define GAP -1
#define MATCH 2
#define MISMATCH -1

struct data {
	int value;	//timi tou pinaka
	int x_prev;	//syntetagmenes apo opou proilthe i timi
	int y_prev;
	int x;		//syntetagmenes pou vrisketai ston pinaka to sygekrimeno keli
	int y;
	int match;	// TRUE - FALSE
};



int main (int argc, char ** argv) {
	FILE *data_fp, *fp1, *fp2;
	int col_num, line_num, db_size, q_size, i, j, point;
	int t1, t2, t3, max, t_max, t_max_i, t_max_j, max_i, max_j, current_i, current_j;
	struct data *column1;
	struct data *column2;
	struct data current;
	char *database;
	char *query, temp, tmp;
	struct stat st;
	
	if (argc < 3) {
		printf("Usage: >a.out database_filename query_filename!\n");
		return 0;
	}
	data_fp=fopen("data", "w+");
	
	/*Eyresi tou megethous twn arxeiwn database - query */
	stat(argv[1], &st);
	db_size = st.st_size;
	fp1=fopen(argv[1], "r");
	database = malloc(sizeof(char)*db_size);
	if (database == NULL) {
		printf("Malloc error!\n");
		return 0 ;
	}
	
	fread(database, sizeof(char), db_size, fp1);
	fclose(fp1);
	
	stat(argv[2], &st);
	q_size = st.st_size;

	fp2=fopen(argv[2], "r");
	query = malloc(sizeof(char)*q_size);
	if (query == NULL) {
		printf("Malloc error!\n");
		return 0 ;
	}
	
	column1 = malloc(sizeof(struct data)* (q_size+1));
	if (column1 == NULL) {
		printf("Malloc error!\n");
		return 0 ;
	}
	
	column2 = malloc(sizeof(struct data)*(q_size+1));
	if (column2 == NULL) {
		printf("Malloc error!\n");
		return 0 ;
	}
	fread(query, sizeof(char), q_size, fp2);
	fclose(fp2);
	
	max = 0;
	
	for(i=0; i<=q_size; i++) {
		column1[i].value = 0;
		column1[i].x = i;
		column1[i].y = 0;
		column1[i].x_prev = -1;
		column1[i].y_prev = -1;
		column1[i].match = FALSE;
	}
	
	fwrite( column1, sizeof(struct data), q_size+1, data_fp);
	for (j=1 ; j<=db_size ; j++) {
		column2[0].value = 0;
		column2[0].x = 0;
		column2[0].y = j;
		column2[0].x_prev = -1;
		column2[0].y_prev = j-1;
		column2[0].match = FALSE;
		for(i=1; i<=q_size; i++) {
			t_max = 0;
			column2[i].match = FALSE;
			t1 = column2[i-1].value + GAP;
			if (t1 > t_max) {
				t_max = t1;
				t_max_i = i-1;
				t_max_j = j;
			}
			
			t2 = column1[i].value + GAP;
			if (t2 > t_max) {
				t_max = t2;
				t_max_i = i;
				t_max_j = j-1;
			}
			
			if (database[j-1] == query [i-1]) 
				t3 = column1[i-1].value + MATCH;
			else 
				t3 = column1[i-1].value + MISMATCH;
			
			if (t3 > t_max) {
				if (database[j-1] == query [i-1]) 
					column2[i].match = TRUE;
				t_max = t3;
				t_max_i = i-1;
				t_max_j = j-1;
			}
		
			if (t_max> max) {
				max = t_max;
				max_i = i;
				max_j = j;
			}
			
			column2[i].value = t_max;
			column2[i].x=i;
			column2[i].y=j;
			column2[i].x_prev=t_max_i;
			column2[i].y_prev=t_max_j;
		
		}
		
		fwrite( column2, sizeof(struct data), q_size+1, data_fp);
		
		/*column1 -> column2*/
		memcpy( column1, column2, sizeof(struct data) * (q_size+1) ); 
		
	}
	
	fclose(data_fp);
	
	
	current_i = max_i;
	current_j = max_j;
	
	data_fp=fopen("data", "r");
	printf("---------------------------------------------------------\n");
	printf("max=%d me syntetagmenes %d %d\n", max, max_i, max_j);
	memset(database, 0, db_size);
	point = 0;
	for (i = 0 ; i< q_size ; i++)
		printf("%c", query[i]);
	printf("\n");
 	while (1){

		if( current_i == -1 || current_j == -1 ) {
			break;
		}
		
		fseek(data_fp, current_j*sizeof(struct data)*(q_size+1) + current_i*sizeof(struct data), SEEK_SET);
		fread(&current, sizeof(struct data), 1, data_fp);
		tmp = query[current_i - 1];
		if (current.match == TRUE)
			database[point] = tmp;
		else
			database[point] = '-' ;
		point++;
		
		current_i = current.x_prev;
		current_j = current.y_prev;

 	}
 	
 	for (i = point-1 ; i>=0 ; i--)
		printf("%c", database[i]);
 	printf("\n");
	fclose(data_fp);
	
}