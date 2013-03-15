#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> /*deklaracje standardowych funkcji uniksowych (fork(), write() itd.)*/

void make_node(int depth) {
    pid_t pid;
	    printf("depth %d\n", depth);
	    switch(pid = fork()){
		case -1:
		    fprintf(stderr, "Blad w fork\n");
		    return EXIT_FAILURE;
		case 0: /*proces potomny*/
		    printf("Jestem procesem potomnym. PID = %d\n \ 
			Wartosc przekazana przez fork() = %d\n", getpid(), pid);
		    if(depth < 3)
		    	make_node(++depth);
			sleep(2000);
		    return EXIT_SUCCESS;
		default:
		    printf("Jestem procesem macierzystym. PID = %d\n \ 
			Wartosc przekazana przez fork() = %d\n", getpid(), pid);
             }
}

int main()
{
    printf("Moj PID = %d\n", getpid());
    int i;
    make_node(0);

}
