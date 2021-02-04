#include "get_next_line.h"
#include <stdio.h>

int main()
{
	int res;
	char *line;

	while ((res = get_next_line(&line)))
	{
		printf("%d -> %s\n", res, line);
		free(line);
	}
	printf("%d -> %s\n", res, line);
	free(line);
	// sleep(1000);
	return (0);
}
