#ifndef SIMPLEX_H
#define SIMPLEX_H

#include <stdbool.h>

#include "../lib/types.h"

typedef enum RowType
{
	LESS,
	GREATER,
	EQUAL,
} RowType;

typedef struct TableauRow
{
	f32* values;
	u32 len;
	i32 id; //decision variable, or surplus etc...
} TableauRow;

typedef struct TableauCol
{
	f32* values;
	u32 len;
} TableauCol;

typedef struct Tableau
{
	bool optimal;
	TableauRow* rows;
	u32 row_len;
	u32 col_len;
} Tableau;

typedef struct ResultPair
{
	f32 value;
	i32 id;
} ResultPair;

typedef struct SimplexResult
{
	f32 optimal_value;
	ResultPair* decvar;
	u32 len;
} SimplexResult;

Tableau* tableau_new(u32 num_constraints, u32 num_variables);
SimplexResult tableau_run_simplex(Tableau* tab);
SimplexResult tableau_run_dual_simplex(Tableau* tab);
void tableau_print(Tableau* tab);

#endif
