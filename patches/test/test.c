#include <stdio.h>
#include <unistd.h>

extern int lib_function (void);

static int first;

void pgm_function(int a, int b, int c, int d, int e, int f, int g, int h)
{
	printf("\e[91m%s\e[0m\targs: [%d %d %d %d  %d %d %d %d]\n", __func__,
		a, b, c, d, e, f, g, h);

	if (!first) {
		first = 1;
		printf("\e[91m%s\e[0m\tinitializing...\n", __func__);
		sleep(10);
	}
}


int main()
{
	while(1) {
		pgm_function(10, 9, 8, 7, 6, 5, 4, 3);
		lib_function();
	}
	
        return 0;
}