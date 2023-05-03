
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
int main(int argc, char **argv)
{
    if (argc == 1)
    {
        exit(0);
    }
    int res = atoi(argv[argc - 1]);
    res = res / 2;
    printf("Half: Current process id: %u, Current result: %d\n", getpid(), res);

    sprintf(argv[argc - 1], "%d", res);

    for (int i = 0; i < argc; i++)
    {
        argv[i] = argv[i + 1];
    }

    argv[argc - 1] = NULL;
    execvp(argv[0], argv);

    return 0;
}