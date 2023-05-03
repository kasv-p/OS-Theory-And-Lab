#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
int main()
{
    char *s = "Hello World";
    for (int i = 0; i < 11; i++)
    {
        sleep(1 + rand() % 4);
        if (fork() == 0)
        {
            printf("%c %d\n", s[i], getpid());
        }
        else
        {
            wait(NULL);
            break;
        }
    }
    return 0;
}