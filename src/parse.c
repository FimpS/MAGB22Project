
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "../include/parse.h"

#define MAX_LEN 9999

Tableau* parse_parse_buffer(Chest* chest, char* buffer)
{
	i32 lexp = 0;
	i32 bufferp = 0;
	f32 rmat[MAX_LEN];
	u32 lenrmat = 0;

	u32 constraint_counter = 0;

	if(buffer[1] == 'a')
	{
		chest->maxproblem = true;	
	}
	buffer = buffer + 4;

	printf("%s\n", buffer);

	u32 len = strlen(buffer);
	char store[MAX_LEN];
	u32 storei = 0;
	for(i32 i = 0; i < len; i++)
	{
		if(buffer[i] == ';') constraint_counter ++;
		if(buffer[i] == '>') *(store + storei ++) = '>';
		if(buffer[i] == '<') *(store + storei ++) = '<';
	}

	constraint_counter --;
	printf("cons: %d\n", constraint_counter);
	u32 dec_variable_count = 0;
	char* lexbuffer = malloc(sizeof(char) * MAX_LEN);
	while(buffer[bufferp] != ';')
	{
		//printf("%d\n", buffer[bufferp]);
		bool neg = false;
		if(isdigit(buffer[bufferp]))
		{
			if(buffer[bufferp - 1] == '-') neg = true;
			lexbuffer[lexp ++] = buffer[bufferp ++];
			while(isdigit(buffer[bufferp]) || buffer[bufferp] == '.')
			{
				lexbuffer[lexp ++] = buffer[bufferp ++];
			}
			Pair p = {
				.id = buffer[bufferp + 1] - '0' - 1,
				.value = atof(lexbuffer),
			};
			if(neg) p.value *= -1.0;
			neg = false;
			chest->variable[chest->len ++] = p;
			bufferp += 1;
			memset(lexbuffer, 0, lexp);
			lexp = 0;
			dec_variable_count ++;
			printf("%d %f\n", p.id, p.value);
		}
		bufferp ++;
	}
	printf("Final dec_var = %d\n", dec_variable_count);

	u32 cons_index = 0;
	while(buffer[bufferp] != '$')
	{
		//printf("Pointer: %d\n", bufferp);
		if(buffer[bufferp] == '<' || buffer[bufferp] == '>')
		{
			for(i32 i = 0; i < constraint_counter; i++)
			{
				if(i == cons_index) 
				{
					rmat[lenrmat ++] = 1;
				} else
					rmat[lenrmat ++] = 0;
			}
			cons_index ++;
			bufferp += 2;
			if(isdigit(buffer[bufferp]))
			{
				printf("xd\n");
			}
		}
		if(buffer[bufferp] == '-')
		{
			bufferp ++;
			if(isdigit(buffer[bufferp]))
			{
				lexbuffer[lexp ++] = buffer[bufferp ++];
				while(isdigit(buffer[bufferp]) || buffer[bufferp] == '.')
				{
					lexbuffer[lexp ++] = buffer[bufferp ++];
				}
				rmat[lenrmat ++] = -1 * atof(lexbuffer);
				if(buffer[bufferp] == ';') 
				{
					printf("Hello: %c\n", buffer[bufferp]);
					bufferp ++;
				}
				else
					bufferp += 2;
				printf("next: %c\n", buffer[bufferp]);
				memset(lexbuffer, 0, lexp);
				lexp = 0;
				printf("%f\n", rmat[lenrmat - 1]);
			}
		}
		//printf("DOPNE HERE\n");
		if(isdigit(buffer[bufferp]))
		{
			lexbuffer[lexp ++] = buffer[bufferp ++];
			while(isdigit(buffer[bufferp]) || buffer[bufferp] == '.')
			{
				lexbuffer[lexp ++] = buffer[bufferp ++];
			}
			rmat[lenrmat ++] = atof(lexbuffer);
			bufferp += 1;
			memset(lexbuffer, 0, lexp);
			lexp = 0;
			printf("%f\n", rmat[lenrmat - 1]);
		}
		bufferp ++;
	}

	printf("RMAT:\n");
	for(i32 i = 0; i < lenrmat; i++) 
	{
		printf("%f ", rmat[i]);
	}
	printf("\n");
	const i32 extrarows = 3;
	Tableau* tab = tableau_new(constraint_counter, dec_variable_count + constraint_counter);
	u32 width = constraint_counter + dec_variable_count;
	for(i32 i = 0; i < constraint_counter; i++)
	{
		for(i32 j = 0; j < constraint_counter + dec_variable_count + 1; j++)
		{
			printf("%d\n", j + 1);
			tab->rows[i].values[j] = rmat[j + i * (width + 1)];
		}
	}
	for(i32 i = constraint_counter; i < constraint_counter + extrarows - 1; i ++)
	{
		for(i32 j = 0; j < constraint_counter + dec_variable_count; j++)
		{
			tab->rows[i].values[j] = 0.0;
		}
	}
	for(i32 i = 0; i < constraint_counter + dec_variable_count; i++)
	{
		if(i >= chest->len)
		{
			tab->rows[tab->col_len - 1].values[i] = 0.0;
		}
		else
			tab->rows[tab->col_len - 1].values[i] = chest->variable[i].value;
	}

	for(i32 i = 0; i < storei; i++)
	{
		if(store[i] == '>')
		{
			for(i32 j = 0; j < tab->row_len; j++)
			{
				if(j < dec_variable_count || j >= dec_variable_count + constraint_counter) 
				{
					tab->rows[i].values[j] *= -1.0;
				}
			}
		}
	}
	if(!chest->maxproblem)
	{
		for(i32 i = 0; i < tab->row_len; i++)
			tab->rows[tab->col_len - 1].values[i] *= -1.0;
	}

	free(lexbuffer);
	tableau_print(tab);
	tab->maxproblem = chest->maxproblem;
	return tab;
}

Tableau* parse_read_file(const char* file_path)
{
	FILE* fp = NULL;
	char* buffer = malloc(sizeof(char) * MAX_LEN);

	if((fp = fopen(file_path, "r")) == NULL)
	{
		printf("ULLE\n");
		exit(1);
	}
	fread(buffer, sizeof(char), MAX_LEN, fp);

	printf("%s\n", buffer);

	Chest chest = (Chest) {
		.variable = NULL,
			.maxproblem = false,
			.len = 0,
	};
	chest.variable = malloc(sizeof(Pair) * 1024);
	return parse_parse_buffer(&chest, buffer);


}
