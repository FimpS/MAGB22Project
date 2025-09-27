#include <stdio.h>

#include "../lib/types.h"
#include "../include/simplex.h"
#include "../include/parse.h"

i32 main(i32 argc, char* argv[]) 
{
	if(argc == 2)
	{
		Tableau* tab = parse_read_file(argv[1]);
		tableau_run_dual_simplex(tab);
		//tableau_run_simplex(tab);
	}
	else
	{
		Tableau* tab = parse_read_file("tests/test3.fspx");
		tableau_run_simplex(tab);
	}

#if 0
	tab->rows[0].values[0] = 2.0;
	tab->rows[0].values[1] = 5.0;
	tab->rows[0].values[2] = 1.0;
	tab->rows[0].values[3] = 0.0;
	tab->rows[0].values[4] = 90.0;

	tab->rows[1].values[0] = 3.0;
	tab->rows[1].values[1] = 4.0;
	tab->rows[1].values[2] = 0.0;
	tab->rows[1].values[3] = 1.0;
	tab->rows[1].values[4] = 100.0;

	tab->rows[2].values[0] = 0.0;
	tab->rows[2].values[1] = 0.0;
	tab->rows[2].values[2] = 0.0;
	tab->rows[2].values[3] = 0.0;
	tab->rows[2].values[4] = 0.0;

	tab->rows[3].values[0] = 0.0;
	tab->rows[3].values[1] = 0.0;
	tab->rows[3].values[2] = 0.0;
	tab->rows[3].values[3] = 0.0;
	tab->rows[3].values[4] = 0.0;

	tab->rows[4].values[0] = 30.0;
	tab->rows[4].values[1] = 40.0;
	tab->rows[4].values[2] = 0.0;
	tab->rows[4].values[3] = 0.0;
	tab->rows[4].values[4] = 0.0;
#endif
#if 0
	Tableau* tab = tableau_new(3, 5);
	tab->rows[0].values[0] = 1.0;
	tab->rows[0].values[1] = 1.0;
	tab->rows[0].values[2] = 1.0;
	tab->rows[0].values[3] = 0.0;
	tab->rows[0].values[4] = 0.0;
	tab->rows[0].values[5] = 4.0;

	tab->rows[1].values[0] = -1.0;
	tab->rows[1].values[1] = 1.0;
	tab->rows[1].values[2] = 0.0;
	tab->rows[1].values[3] = 1.0;
	tab->rows[1].values[4] = 0.0;
	tab->rows[1].values[5] = 1.0;

	tab->rows[2].values[0] = 2.0;
	tab->rows[2].values[1] = 4.0;
	tab->rows[2].values[2] = 0.0;
	tab->rows[2].values[3] = 0.0;
	tab->rows[2].values[4] = 1.0;
	tab->rows[2].values[5] = 10.0;

	tab->rows[3].values[0] = 0.0;
	tab->rows[3].values[1] = 0.0;
	tab->rows[3].values[2] = 0.0;
	tab->rows[3].values[3] = 0.0;
	tab->rows[3].values[4] = 0.0;
	tab->rows[3].values[5] = 0.0;

	tab->rows[4].values[0] = 0.0;
	tab->rows[4].values[1] = 0.0;
	tab->rows[4].values[2] = 0.0;
	tab->rows[4].values[3] = 0.0;
	tab->rows[4].values[4] = 0.0;
	tab->rows[4].values[5] = 0.0;

	tab->rows[5].values[0] = 20.0;
	tab->rows[5].values[1] = 30.0;
	tab->rows[5].values[2] = 0.0;
	tab->rows[5].values[3] = 0.0;
	tab->rows[5].values[4] = 0.0;
	tab->rows[5].values[5] = 0.0;
#endif

	//tableau_run_simplex(tab);

	return 0;
}
