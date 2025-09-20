#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../include/simplex.h"


#define EXTRA_ROWS 3
#define DRAW_LINE(s) { for(i32 oooga = 0; oooga < s; oooga++) printf("-"); printf("\n"); }
#define ID_NONE -1

SimplexResult simplexresult_new(f32 optimal_value, f32* decvar_values, u32 len)
{
	SimplexResult res = {
		.optimal_value = optimal_value,
		.len = len,
		.decvar_values = NULL,
	};
	//MALLOC?
	memcpy(res.decvar_values, decvar_values, len * sizeof(f32));

	return res;
}

TableauCol tableaucol_new(f32* values, u32 len)
{
	TableauCol tabcol_new = {
		.len = len,
		.values = NULL,
	};

	tabcol_new.values = malloc(sizeof(f32) * len);
	memcpy(tabcol_new.values, values, sizeof(f32) * len);
	return tabcol_new;
}

void tableaucol_destroy(TableauCol tab)
{
	free(tab.values);
}

void tableaucol_print(TableauCol col)
{
	for(i32 i = 0; i < col.len; i++)
	{
		printf("%f\n", col.values[i]);
	}
}

//TODO REMOVE THIS
static i32 test = 0;
TableauRow tableaurow_new(u32 row_len, i32 id)
{
	TableauRow tabrow_new = {
		.values = malloc(sizeof(f32) * row_len),
		.len = row_len,
		.id = id,
	};

	for(int i = 0; i < row_len; i++)
	{
		tabrow_new.values[i] = (f32) test;
	}
	return tabrow_new;
}

void tableaurow_print(TableauRow* tab)
{
	for(i32 i = 0; i < tab->len; i++)
	{
		printf("%f ", tab->values[i]);
	}
	printf("\n");
}

i32 tableaurow_min(f32* values, u32 len)
{
	u32 min = values[0];
	i32 rindex = 0;
	for(i32 i = 1; i < len; i++)
	{
		if(values[i - 1] >= values[i] && values[i] >= 0.0) 
		{
			min = values[i];
			rindex = i;
		}
	}
	if(min <= 0.0) { printf("TODO: INFEASABLE\n"); rindex = -1; }

	return rindex;
}

void tableau_adjust_non_pivot_row(TableauRow* row, const TableauRow pivot_row, i32 column_pivot_element)
{
	u32 len = row->len;
	for(i32 i = 0; i < len; i++)
	{
		row->values[i] = row->values[i] - column_pivot_element * pivot_row.values[i];
	}
}

void tableau_adjust_pivotrow(TableauRow* pivot_row, u32 pivot_element)
{
	u32 len = pivot_row->len;
	for(i32 i = 0; i < len; i++)
	{
		pivot_row->values[i] /= pivot_element;
	}
}

u32 tableau_ratio_test(Tableau* tab, u32 pivot_col_index)
{
	u32 last_index = tab->row_len - 1;
	u32 len = tab->col_len;
	u32 extra_rows = 0; //TODO should be like 3
	f32* test_values = malloc(sizeof(f32) * tab->row_len - extra_rows);
	for(i32 i = 0; i < len; i++)
	{
		f32 current_denom = tab->rows[i].values[pivot_col_index];
		if(current_denom == 0.0)
		{
			//TODO bad divisor continue;
		}
		f32 test = tab->rows[i].values[last_index] / current_denom;
		test_values[i] = test;

	}
	tableaurow_min(test_values, tab->row_len - extra_rows);
	//free(test_values);
}

Tableau* tableau_new(u32 num_constraints, u32 num_variables)
{
	Tableau* tab_new = malloc(sizeof(Tableau));
	tab_new->col_len = num_constraints + EXTRA_ROWS;

	tab_new->rows = malloc(sizeof(TableauRow) * tab_new->col_len); //TODO + some c_j, z_j, c_j - z_j
	tab_new->row_len = num_variables + 1;
	for(i32 i = 0; i < tab_new->col_len; i++)
	{
		i32 id = ID_NONE;
		if(i < num_constraints)
		{
			id = i + num_variables - num_constraints;
		}
		tab_new->rows[i] = tableaurow_new(tab_new->row_len, id);
		test++;
	}

	return tab_new;
}

void tableau_print(Tableau* tab)
{
	for(i32 i = 0; i < tab->col_len; i++)
	{
		printf("| ");
		for(i32 j = 0; j < tab->row_len; j++)
		{
			printf("%f ", tab->rows[i].values[j]);
		}
		printf("|\n");
		if( i == tab->col_len - 3 - 1 ) DRAW_LINE(32);
	}
}

u32 tableau_get_cj(Tableau* tab)
{
	return tab->col_len - 1;
}

u32 tableau_get_zj(Tableau* tab)
{
	return tab->col_len - 3;
}

u32 tableau_get_cj_zj_diff(Tableau* tab)
{
	return tab->col_len - 2;
}

//f32 tableau_calculate_cj(Tableau* tab, TableauCol l  ) P rintf ullepump:wq

TableauCol tableau_get_col(Tableau* tab, u32 index)
{
	u32 len = tab->col_len;
	f32 values[len];
	i32 values_counter = 0;
	for(i32 i = 0; i < len; i++)
	{
		const f32 val = tab->rows[i].values[index];
		values[values_counter ++] = val;
	}
	return tableaucol_new(values, len);
}

f32 tableau_calc_objective_function(Tableau* tab, TableauRow cj, u32 j)
{
	const TableauCol col = tableau_get_col(tab, 0);
	f32 res = 0.0;
	u32 len = tab->col_len - EXTRA_ROWS;

	for(i32 i = 0 ; i < len; i++)
	{
		TableauRow current_row = tab->rows[i];
		printf("%f %d\n", cj.values[current_row.id], current_row.id);
		res += cj.values[current_row.id] * current_row.values[j];
	}

	return res;
}

void tableau_calculate_zj(Tableau* tab)
{
	u32 zj_index = tableau_get_zj(tab);
	u32 cj_index = tableau_get_cj(tab);
	TableauRow* zj_row = &tab->rows[zj_index];
	TableauRow* cj_row = &tab->rows[cj_index];

	u32 len = tab->row_len;
	for(i32 i = 0;  i < len; i++)
	{
		zj_row->values[i] = tableau_calc_objective_function(tab, *cj_row, i);
	}
}

SimplexResult tableau_run_simplex(Tableau* tab)
{

	TableauCol col = tableau_get_col(tab, 1);
	//tableau_print(tab);
	tableau_calculate_zj(tab);
	//tableaurow_print(zj_row);
	DRAW_LINE(32);
	tableau_print(tab);
	DRAW_LINE(32);

	SimplexResult result = simplexresult_new(0, 0, 0);
	return result;
}




