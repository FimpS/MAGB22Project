#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "../include/simplex.h"

#define LINE_LEN 224
#define EXTRA_ROWS 3
#define DRAW_LINE() { for(i32 oooga = 0; oooga < LINE_LEN; oooga++) printf("-"); printf("\n"); }
#define ID_NONE -1

#if 1
#define COLOR_GREEN "\033[0;32m"
#define COLOR_RED "\033[0;31m"
#define COLOR_BLUE "\033[0;34m"
#define COLOR_RESET "\033[0m"
#endif
#if 0
#define COLOR_GREEN ""
#define COLOR_RED ""
#define COLOR_BLUE ""
#define COLOR_RESET ""
#endif



void simplexresult_destory(SimplexResult sresult)
{
	free(sresult.decvar);
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
	f32 min = values[0];
	i32 rindex = 0;
	for(i32 i = 1; i < len; i++)
	{
		//printf("%f\n", values[i-1]);
		if(min <= 0.0) 
		{
			min = values[i];
			rindex = i;
			continue;
		}
		if(min > values[i] && values[i] > 0.0) 
		{
			min = values[i];
			rindex = i;
		}
		//printf("%f\n", min);
	}
	if(min <= 0.0) { printf("TODO: INFEASABLE\n"); rindex = -1; }

	return rindex;
}

void tableau_adjust_non_pivot_row(TableauRow* row, const TableauRow pivot_row, f32 column_pivot_element)
{
	u32 len = row->len;
	for(i32 i = 0; i < len; i++)
	{
		row->values[i] = row->values[i] - column_pivot_element * pivot_row.values[i];
	}
}

void tableau_adjust_non_pivot_rows(Tableau* tab, const TableauRow pivot_row, u32 pivot_col_index, u32 pivot_row_index)
{
	u32 len = tab->col_len - EXTRA_ROWS;
	for(i32 i = 0; i < len; i++)
	{
		if(i == pivot_col_index) continue;
		f32 column_pivot = tab->rows[i].values[pivot_row_index];
		TableauRow* row = &tab->rows[i];
		tableau_adjust_non_pivot_row(row, pivot_row, column_pivot);
	}
}

void tableau_adjust_pivotrow(TableauRow* pivot_row, f32 pivot_element)
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
	u32 extra_rows = EXTRA_ROWS;
	f32* test_values = malloc(sizeof(f32) * tab->row_len - extra_rows);
	for(i32 i = 0; i < len - EXTRA_ROWS; i++)
	{
		f32 current_denom = tab->rows[i].values[pivot_col_index];
		if(current_denom == 0.0)
		{
			test_values[i] = 99999999.0;
			continue;
			//TODO bad divisor continue;
		}
		else
		{
			f32 test = tab->rows[i].values[last_index] / current_denom;
			test_values[i] = test;
		}

	}
	u32 rindex = tableaurow_min(test_values, tab->col_len - EXTRA_ROWS);
	free(test_values);
	return rindex;
}

Tableau* tableau_new(u32 num_constraints, u32 num_variables)
{
	Tableau* tab_new = malloc(sizeof(Tableau));
	tab_new->optimal = false;
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
		//test++;
	}

	return tab_new;
}

void tableau_print(Tableau* tab)
{
	i32 decvar_count = (tab->row_len - 1) - (tab->col_len - EXTRA_ROWS);
	for(i32 i = 0; i < tab->row_len - 1; i++)
	{
		if(i < decvar_count)
		{
			printf("%s%9s_%d%s", COLOR_GREEN, "x", i + 1, COLOR_RESET);
		} else 
			printf("%s%9s_%d%s", COLOR_RED, "s", i + 1 - decvar_count, COLOR_RESET);
	}
	printf("%9s", "RHS");
	printf("\n");
	DRAW_LINE();
	for(i32 i = 0; i < tab->col_len; i++)
	{
		TableauRow* row = &tab->rows[i];

		printf("| ");
		for(i32 j = 0; j < tab->row_len; j++)
		{
			printf("%10.4f ", tab->rows[i].values[j]);
		}
		printf("%s", COLOR_RESET);
		printf("|");
		if(i < tab->col_len - 3)
		{
			if(row->id < decvar_count)
			{
				printf("%s x_%d %s",COLOR_GREEN, row->id + 1, COLOR_RESET);
			}
			else
			{
				printf("%s s_%d %s", COLOR_RED, row->id - decvar_count + 1, COLOR_RESET);
			}
		}
		if( i == tab->col_len - 3)	printf(" zj");
		if( i == tab->col_len - 2)  printf(" (cj - zj)%s", tab->simplex_version == DUAL ? "/aij" : "");
		if( i == tab->col_len - 1)	printf(" cj"); 
		printf("\n");
		if( i == tab->col_len - 3 - 1 ) DRAW_LINE();
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

u32 tableau_get_cjzj_diff(Tableau* tab)
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

void tableau_calculate_zjcj_diff(Tableau* tab)
{
	TableauRow* cj = &tab->rows[tableau_get_cj(tab)];
	TableauRow* zj = &tab->rows[tableau_get_zj(tab)];
	TableauRow* zcdiff = &tab->rows[tableau_get_cjzj_diff(tab)];
	for(i32 i = 0; i < tab->row_len; i++)
	{
		zcdiff->values[i] = cj->values[i] - zj->values[i];
	}
}

u32 tableau_find_max_zjcj(Tableau* tab)
{
	u32 resindex = 0;
	TableauRow cj_row = tab->rows[tableau_get_cjzj_diff(tab)];
	f32 max = cj_row.values[0];

	for(i32 i = 1; i < cj_row.len - 1; i++)
	{
		if(max <= cj_row.values[i]) 
		{
			max = cj_row.values[i];
			resindex = i;
		}
	}
	return resindex;
}

TableauRow* tableau_get_row_by_id(Tableau* tab, u32 id)
{
	for(i32 i = 0; i < tab->row_len - EXTRA_ROWS; i++)
	{
		if(tab->rows[i].id == id) return &tab->rows[i];
	}
	return NULL;
}

bool tableau_is_optimal(Tableau* tab)
{
	TableauRow zjcj_row = tab->rows[tableau_get_cjzj_diff(tab)];
	for(i32 i = 0; i < tab->row_len - 1; i++)
	{
		if(zjcj_row.values[i] > 0.0) return false;
	}
	return true;
}

SimplexResult simplexresult_new(Tableau* tab)
{
	u32 len = tab->col_len - EXTRA_ROWS;
	SimplexResult res = {
		.optimal_value = 0.0,
		.len = len,
		.decvar = NULL,
	};
	res.decvar = malloc(sizeof(ResultPair) * len);
	for(i32 i = 0; i < len; i++)
	{
		TableauRow* row = &tab->rows[i];
		f32 coef = row->values[tab->row_len - 1];
		i32 id = row->id;
		ResultPair pair = (ResultPair) { coef, id };
		res.decvar[i] = pair;
	}
	f32 optimal = 0.0;

	i32 decvar_count = (tab->row_len - 1) - (tab->col_len - EXTRA_ROWS);
	for(i32 i = 0 ; i < len; i++)
	{
		TableauRow* cj = &tab->rows[tableau_get_cj(tab)];
		f32 val = cj->values[res.decvar[i].id];
		optimal += val * res.decvar[i].value;
	}
	res.optimal_value = optimal;

	return res;
}

bool shadow_is_non_optimal(const SimplexResult r, i32 id)
{
	for(i32 i = 0; i < r.len; i++)
	{
		if(r.decvar[i].id == id) return false;
	}
	return true;
}

i32* shadow_analysis_get_non_optimal_slack(const SimplexResult result, Tableau* tab, i32* size)
{
	i32 decvar_count = (tab->row_len - 1) - (tab->col_len - 3);
	i32* res = malloc(sizeof(i32) * 32);
	i32 index = 0;
	for(i32 i = 0; i < tab->row_len - 1; i++)
	{
		if(shadow_is_non_optimal(result, i))
		{
			res[index ++] = i;
			(*size) ++;
			//printf("Nonoptimal: %d\n", res[index - 1]);
		}
	}
	return res;
}

f32 shadow_get_obj_upper_bound(const SimplexResult result, Tableau* tab, i32 size, i32* vals, TableauRow* curr_row)
{

	//TODO should be lowest of the highest i think so smallest pos value and largest neg value
	TableauRow* zj_row = &tab->rows[tableau_get_zj(tab)];
	f32 res = 0.0;
	for(i32 i = 0; i < size; i++)
	{
		res = (-1.0) * zj_row->values[vals[i]] / curr_row->values[vals[i]];
		if(curr_row->values[vals[i]] == 0.0) return 1.0/0.0;
		if(res >= 0.0) break;
	}
	for(i32 i = 0; i < size; i++)
	{
		//if(curr_row->values[vals[i]] == 0.0) return 0.0;
		f32 suggested = (-1.0) * zj_row->values[vals[i]] / curr_row->values[vals[i]];
		//printf("Enum: %f, Denom, %f ... Lower Suggested: %f\n", zj_row->values[vals[i]], curr_row->values[vals[i]], suggested);
		if(suggested <= res && suggested >= 0.0) 
		{
			//printf("Res: %f  Suggested: %f\n", res, suggested);
			res = suggested;
		}
	}
	return res;
}

f32 shadow_get_obj_lower_bound(const SimplexResult result, Tableau* tab, i32 size, i32* vals, TableauRow* curr_row)
{
	TableauRow* zj_row = &tab->rows[tableau_get_zj(tab)];
	f32 res = 0.0;
	for(i32 i = 0; i < size; i++)
	{
		res = (-1.0) * zj_row->values[vals[i]] / curr_row->values[vals[i]];
		if(curr_row->values[vals[i]] == 0.0) return - 1.0/0.0;
		if(res <= 0.0) break;
	}
	for(i32 i = 0; i < size; i++)
	{
		//if(curr_row->values[vals[i]] == 0.0) return 0.0;
		f32 suggested = (-1.0) * zj_row->values[vals[i]] / curr_row->values[vals[i]];
		//printf("Enum: %f, Denom, %f ... Lower Suggested: %f\n", zj_row->values[vals[i]], curr_row->values[vals[i]], suggested);
		if(suggested >= res && suggested <= 0.0) 
		{
			//printf("Res: %f  Suggested: %f\n", res, suggested);
			res = suggested;
		}
	}
	return res;
}

f32 shadow_get_cons_upper_bound(const SimplexResult result, Tableau* tab, i32* non_optimal_slack_indeces, u32 col_index)
{

	f32 res = 0.0;

	for(i32 i = 0; i < tab->col_len - EXTRA_ROWS; i++)
	{
		f32 b_factor = tab->rows[i].values[non_optimal_slack_indeces[col_index]];
		u32 last_index = tab->row_len - 1;
		f32 rhs_factor = tab->rows[i].values[last_index];
		res = (-1.0) * rhs_factor / b_factor;
		if(res >= 0.0) break;

	}
	for(i32 i = 0; i < tab->col_len - EXTRA_ROWS; i++)
	{
		f32 b_factor = tab->rows[i].values[non_optimal_slack_indeces[col_index]];
		u32 last_index = tab->row_len - 1;
		f32 rhs_factor = tab->rows[i].values[last_index];
		f32 suggestion = (-1.0) * rhs_factor / b_factor;
		if(suggestion <= res && suggestion >= 0.0)
		{
			res = suggestion;
		}
	}

	return res;
}

f32 shadow_get_cons_lower_bound(const SimplexResult result, Tableau* tab, i32* non_optimal_slack_indeces, u32 col_index)
{

	f32 res = 0.0;

	for(i32 i = 0; i < tab->col_len - EXTRA_ROWS; i++)
	{
		f32 b_factor = tab->rows[i].values[non_optimal_slack_indeces[col_index]];
		u32 last_index = tab->row_len - 1;
		f32 rhs_factor = tab->rows[i].values[last_index];
		res = (-1.0) * rhs_factor / b_factor;
		if(res <= 0.0) break;

	}
	for(i32 i = 0; i < tab->col_len - EXTRA_ROWS; i++)
	{
		f32 b_factor = tab->rows[i].values[non_optimal_slack_indeces[col_index]];
		u32 last_index = tab->row_len - 1;
		f32 rhs_factor = tab->rows[i].values[last_index];
		f32 suggestion = (-1.0) * rhs_factor / b_factor;
		if(suggestion >= res && suggestion <= 0.0)
		{
			res = suggestion;
		}
	}

	return res;
}

f32 shadow_dual_get_obj_lower_bound(const SimplexResult result, Tableau* tab, i32 size, i32* vals, TableauRow* curr_row)
{
	TableauRow* zj_row = &tab->rows[tableau_get_zj(tab)];
	f32 res = 0.0;
	for(i32 i = 0; i < size; i++)
	{
		//printf("vals[i] = %f\n", curr_row->values[vals[i]]);
		res = (-1.0) * zj_row->values[vals[i]] / curr_row->values[vals[i]];
		if(res <= 0.0) break;
	}
	for(i32 i = 0; i < size; i++)
	{
		f32 suggested = (-1.0) * zj_row->values[vals[i]] / curr_row->values[vals[i]];
		//printf("Enum: %f, Denom, %f ... Lower Suggested: %f\n", zj_row->values[vals[i]], curr_row->values[vals[i]], suggested);
		if(suggested >= res && suggested <= 0.0) 
		{
			//printf("Res: %f  Suggested: %f\n", res, suggested);
			res = suggested;
		}
	}
	return res;
}

void simplexshadowanalysis_print(const SimplexResult result, Tableau* tab)
{
	i32 slack_non_size = 0;
	i32 *non_optimal_slack_indeces = shadow_analysis_get_non_optimal_slack(result, tab, &slack_non_size);
	i32 decvar_count = (tab->row_len - 1) - (tab->col_len - 3);
	//for(i32 i = 0; i < slack_non_size; i++)
	printf("Range of Optimality: \n");
	for(i32 i = 0; i < decvar_count - 1; i++)
	{
		//printf("Slack indecis: %d\n", non_optimal_slack_indeces[i]);
		TableauRow* row = tableau_get_row_by_id(tab, i + 1);
		if(row != NULL)
		{
			//if(row->id <= decvar_count)
			{
				TableauRow* zj_row = &tab->rows[tableau_get_zj(tab)];
				f32 upper_offset = shadow_get_obj_upper_bound(result, tab, slack_non_size, non_optimal_slack_indeces, row);
				f32 lower_offset = shadow_dual_get_obj_lower_bound(result, tab, slack_non_size, non_optimal_slack_indeces, row);
				//printf("Upper: %f, Lower: %f\n", upper_offset, lower_offset);
				TableauRow* cj_row = &tab->rows[tableau_get_cj(tab)];
				printf("%10.4f < c%d < %10.4f\n", cj_row->values[row->id] + lower_offset, i + 1 + 1, cj_row->values[row->id] + upper_offset);
			}
		}
	}

	printf("Range of Feasability (offset):\n");
	u32 num_constraints = tab->col_len - EXTRA_ROWS;
	u32 last_index = tab->row_len - 1;
	for(i32 i = 0; i < num_constraints - 1; i ++)
	{
		tab->rows[i].values[last_index];
		f32 upper = shadow_get_cons_upper_bound(result, tab, non_optimal_slack_indeces, i);
		f32 lower = shadow_get_cons_lower_bound(result, tab, non_optimal_slack_indeces, i);
		printf("%10.4f < b%d_offset < %10.4f\n", lower, i + 1, upper);
	}
	DRAW_LINE();



	free(non_optimal_slack_indeces);
}

void simplexresult_print(const SimplexResult result, Tableau* tab)
{
	i32 decvar_count = (tab->row_len - 1) - (tab->col_len - 3);
	//printf("count %d %d %d\n", decvar_count, tab->col_len, tab->row_len);
	for(i32 i = 0; i < result.len; i++)
	{
		if(result.decvar[i].id < decvar_count)
		{
			printf("%sx_%d = %f%s\n", COLOR_GREEN, result.decvar[i].id + 1, result.decvar[i].value, COLOR_RESET);	
		}
		else
		{
			printf("%ss_%d = %f%s\n", COLOR_RED, result.decvar[i].id + 1 - decvar_count, result.decvar[i].value, COLOR_RESET);	
		}
	}
	printf("z =");
	for(i32 i = 0; i < tab->row_len - 1; i++)
	{
		TableauRow* cj = &tab->rows[tableau_get_cj(tab)];
		ResultPair pair = {0.0, 0};
		for(i32 j = 0; j < result.len; j++)
		{
			pair.id = i;
			if(result.decvar[j].id == i) 
			{
				pair = result.decvar[j];
				j = result.len;
			}
		}
		f32 val = cj->values[i];

		printf(" %f(%f) %s", val, pair.value, i == tab->row_len - 2 ? "" : "+");
	}
	printf("= %f\n\n", result.optimal_value);
	printf("Print Shadow analysis (y/n): ");
	char c = getc(stdin);
	if(c == 'y')
	{
	DRAW_LINE();
	printf("Sensitivity Analysis:\n");
	DRAW_LINE();
	simplexshadowanalysis_print(result, tab);
	}
}

void tableau_print_var_change(Tableau* tab, i32 in, i32 out)
{
	in ++;
	out ++;
	i32 decvar_count = (tab->row_len - 1) - (tab->col_len - EXTRA_ROWS);
	if(in < decvar_count) printf("Entering variable:%s%9s_%d%s\n", COLOR_GREEN, "x", in, COLOR_RESET);
	else printf("Entering variable: %s%9s_%d%s\n", COLOR_RED, "s", in - decvar_count, COLOR_RESET);
	if(out < decvar_count) printf("Exiting variable:  %s%9s_%d%s\n", COLOR_GREEN, "x", out, COLOR_RESET);
	else printf("Exiting variable: %s%9s_%d%s\n", COLOR_RED, "s", out - decvar_count, COLOR_RESET);

}

SimplexResult tableau_run_simplex(Tableau* tab)
{

	TableauCol col = tableau_get_col(tab, 1);
	DRAW_LINE();
	printf("INITIAL TALBEAU: \n");
	DRAW_LINE();
	tableau_print(tab);
	DRAW_LINE();
	i32 iteration = 1;
	for(;;)
	{
		printf("ITERATION: %d\n", iteration ++);
		tableau_calculate_zj(tab);
		tableau_calculate_zjcj_diff(tab);
		if(tableau_is_optimal(tab)) break;
		u32 variable_in_index = tableau_find_max_zjcj(tab);
		u32 variable_out_index = tableau_ratio_test(tab, variable_in_index);
		const f32 pivot_element = tab->rows[variable_out_index].values[variable_in_index];

		TableauRow* pivot_row = &tab->rows[variable_out_index];
		tableau_print_var_change(tab, variable_in_index, pivot_row->id);
		printf("Pivot Element: %f\n", pivot_element);
		//for(i32 i = 0; i < pivot_row->len; i++) printf("%f ", pivot_row->values[i]);
		//printf("\n");
		pivot_row->id = variable_in_index;

		//tableau_print(tab);
		tableau_adjust_pivotrow(pivot_row, pivot_element);
		tableau_adjust_non_pivot_rows(tab, *pivot_row, variable_out_index, variable_in_index);
		//tableau_adjust_non_pivot_rows TODO NEXT MAKE THIS FOR ALL ROWS NEEDED just loop through the rows and not pass in one row...
		//
		DRAW_LINE();
		tableau_print(tab);
		DRAW_LINE();
	}
	DRAW_LINE();
	printf("OPTIMAL TABLEAU\n");
	DRAW_LINE();
	tableau_print(tab);

	DRAW_LINE();

	SimplexResult result = simplexresult_new(tab);
	DRAW_LINE();
	simplexresult_print(result, tab);
	return result;
}

i32 tableau_find_min_RHS(Tableau* tab)
{
	i32 res = -1;
	u32 rhs_index = tab->row_len - 1;
	u32 len = tab->col_len - EXTRA_ROWS;

	f32 min = tab->rows[0].values[rhs_index];
	for(i32 i = 0; i < len; i++)
	{
		//printf("in min: %d\n", min);
		if(min >= tab->rows[i].values[rhs_index])
		{
			min = tab->rows[i].values[rhs_index];
			res = i;
		}
	}
	if(min >= 0.0) return -1;

	return res;
}

void tableau_calculate_zjcjaij(Tableau* tab, i32 aij_index)
{
	tableau_calculate_zjcj_diff(tab);
	TableauRow* zcdiff = &tab->rows[tableau_get_cjzj_diff(tab)];
	TableauRow* aij_row = &tab->rows[aij_index];
	for(i32 i = 0; i < zcdiff->len - 1; i++)
	{
		if(aij_row->values[i] < 0.0)
		{
			zcdiff->values[i] /= aij_row->values[i];
		}
		else zcdiff->values[i] = -1.0;
	}
}

i32 tableau_find_min_nonneg_zjcjaij(Tableau* tab)
{
	i32 res = -1;
	TableauRow* aij_row = &tab->rows[tableau_get_cjzj_diff(tab)];
	f32 min = aij_row->values[0];
	for(i32 i = 0; i < aij_row->len - 1; i++)
	{
		f32 value = aij_row->values[i];
		if((value >= 0.0 && min >= value) || min < 0.0)
		{
			min = value;
			res = i;
		}
	}
	if(min <= 0.0) return -1;
	return res;

}

bool tableau_is_dual_optimal(Tableau* tab)
{
	u32 rhs_index = tab->row_len - 1;
	u32 len = tab->col_len - EXTRA_ROWS;

	for(i32 i = 0; i < len; i++)
	{
		if(tab->rows[i].values[rhs_index] < 0.0) return false;
	}
	return true;
}

SimplexResult tableau_run_dual_simplex(Tableau* tab)
{
	i32 iteration = 1;
	DRAW_LINE();
	printf("Initial Tableau:\n");
	tableau_print(tab);
	DRAW_LINE();
	while(!tableau_is_dual_optimal(tab))
	{
		printf("ITERATION: %d\n", iteration ++);
		tableau_calculate_zj(tab);
		i32 variable_out_index = tableau_find_min_RHS(tab);
		tableau_calculate_zjcjaij(tab, variable_out_index);
		i32 variable_in_index = tableau_find_min_nonneg_zjcjaij(tab);
		tableau_print(tab);
		printf("In: %d\nOut: %d\n", variable_in_index, variable_out_index);
		//tableau_print(tab);

		const f32 pivot_element = tab->rows[variable_out_index].values[variable_in_index];
		TableauRow* pivot_row = &tab->rows[variable_out_index];
		printf("Pivot Element: %f\n", pivot_element);
		pivot_row->id = variable_in_index;

		tableau_adjust_pivotrow(pivot_row, pivot_element);
		tableau_adjust_non_pivot_rows(tab, *pivot_row, variable_out_index, variable_in_index);
		DRAW_LINE();
		tableau_print(tab);
		DRAW_LINE();
	}
	printf("OPTIMAL TABLEAU:\n");
	DRAW_LINE();
	tableau_print(tab);
	DRAW_LINE();


	SimplexResult result = simplexresult_new(tab);
	DRAW_LINE();
	simplexresult_print(result, tab);
}

SimplexResult tableau_run_twophase(Tableau* tab, u32 art_count)
{

	tableau_run_simplex(tab);
	tab->row_len -= art_count;
	tableau_run_simplex(tab);
}

MethodType tableau_decide_method(Tableau* tab)
{
	u32 rhs_index = tab->row_len - 1;
	u32 len = tab->col_len - EXTRA_ROWS;

	for(i32 i = 0; i < len; i++)
	{
		if(tab->rows[i].values[rhs_index] < 0.0) return DUAL;
	}
	return NORMAL;
}


