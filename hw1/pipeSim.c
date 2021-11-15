#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

// Iki tane file destriptor kullan, 3 tane process yarat

int main(int argc, char *argv[])
{
    printf("I’m SHELL process, with PID: %d - ", (int)getpid());
    printf("Main command is: man diff | grep \"\\--ignore-all-space\" --after-context=1\n");

    int fd[2];
    pipe(fd);

    int rc;
    if ((rc = fork()) < 0)
    {
        printf("fork error");
        exit(1);
    }
    else if (rc == 0)
    {
        printf("I’m MAN process, with PID: %d - ", (int)getpid());
        printf("My command is: man grep\n");

        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);

        char *man_args[3];
        man_args[0] = strdup("man");
        man_args[1] = strdup("diff");
        man_args[2] = NULL;
        execvp(man_args[0], man_args);

        return 0;
    }

    waitpid(rc, NULL, 0);

    if ((rc = fork()) < 0)
    {
        printf("fork error");
        exit(1);
    }
    else if (rc == 0)
    {
        printf("I’m GREP process, with PID: %d - ", (int)getpid());
        printf("My command is: grep \"\\--ignore-all-space\" --after-context=1\n");

        int new_file = open("./output.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);

        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);
        dup2(new_file, STDOUT_FILENO);

        char *grep_args[4];
        grep_args[0] = strdup("grep"); 
        grep_args[1] = strdup("\\--ignore-all-space");
        grep_args[2] = strdup("--after-context=1");
        grep_args[3] = NULL;          
        execvp(grep_args[0], grep_args);

        return 0;
    }
    else
    {
        close(fd[0]);
        close(fd[1]);
        
        wait(NULL);
        printf("I’m SHELL process, with PID: %d - ", (int)getpid());
        printf("execution is completed, you can find the results in output.txt\n");
    }

    return 0;
}