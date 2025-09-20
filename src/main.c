#include <stdio.h>

#include "../lib/types.h"
#include "../include/simplex.h"

i32 main(i32 argc, char* argv[]) 
{

	Tableau* tab = tableau_new(2, 4);

	tab->rows[4].values[2] = 0.0;
	tab->rows[4].values[3] = 3.0;
	tableau_run_simplex(tab);

	return 0;
}
