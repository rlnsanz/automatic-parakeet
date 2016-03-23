/*
 * fork_stochastic.c
 *
 *  Created on: Dec 25, 2015
 *      Author: duolu
 */

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>




int main(int argc, char **argv)
{
	int i;
	pid_t pid;

	if(argc < 4) {
		printf("Usage: fork_stochastic seed max n\n");
		return 0;
	}

	int seed = strtol(argv[1], NULL, 10);
	int max = strtol(argv[2], NULL, 10);
	int n = strtol(argv[3], NULL, 10);

	int p = getpid();
	printf("%d\n", p);
	fflush(stdout);

	srand(seed);

	int r = rand();
	int count = r % max;

	char new_seed[4];
	char new_n[4];
	snprintf(new_n, sizeof(new_n), "%d", n - 1);	

	for(i = 0; i < count && n > 0; i++) {

		snprintf(new_seed, sizeof(new_seed), "%d", r + i);	

    		pid = fork();
    		if(pid == 0){
    			execl("./fork_stochastic", "./fork_stochastic", new_seed, argv[2], new_n, NULL);
    		}
	}
 
    	while(1) 
    		;

	return 0;
}

