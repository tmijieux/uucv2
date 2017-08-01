#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include <unistd.h>

int main(int argc, char *argv[], char *envp[])
{
    int filedes[2];
    pid_t p = 0;

    (void) argc;
    pipe(filedes);
    p = fork();
    if ( p == -1 ) {
        fprintf(stderr, "Cannot fork: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    } else if ( p == 0 ) {
        close(filedes[0]);
        dup2(filedes[1], STDOUT_FILENO);
        argv[0] = "cpp";
        execve("/usr/bin/cpp", argv, envp);
        fprintf(stderr, "Cannot exec cpp: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    } else {
        close(filedes[1]);
        dup2(filedes[0], STDIN_FILENO);
        char *argv[] = { "cc1", NULL };
        char *envp[] = { NULL };
        execve("./cc1", argv, envp );
        fprintf(stderr, "Cannot exec cc1: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // never reached
    return EXIT_SUCCESS;
}
