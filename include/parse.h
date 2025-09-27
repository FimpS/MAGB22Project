
#include "types.h"
#include "simplex.h"

typedef struct Pair
{
	f32 value;
	i32 id;
} Pair;

typedef struct Chest
{
	Pair* variable;
	u32 len;
	bool maxproblem;
} Chest;

Tableau* parse_read_file();
