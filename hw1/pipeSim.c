#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

// Iki tane file destriptor kullan, 3 tane process yarat

int main(int argc, char *argv[])
{
    int fd[2]; // file descriptor

    pipe(fd);

    int rc = fork();
    if (rc < 0)
    {
        fprintf(stderr, "fork failed\n");
        exit(1);
    }
    else if (rc == 0)
    {
        close(fd[0]); // close the output of fd

        close(STDOUT_FILENO); // close the stdout
        dup(fd[1]); // input of fd replaces stdout

        // Execute man grep
        char *man_args[3];
        man_args[0] = strdup("man"); 
        man_args[1] = strdup("grep");
        man_args[2] = NULL;          
        execvp(man_args[0], man_args);
    }
    else
    {
        int wc = wait(NULL); // wait for man command to exit

        close(fd[1]); // close the input of pipe

        close(STDOUT_FILENO); // close the stdout
        open("./output.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU); // output.txt is the output file

        close(STDIN_FILENO); // close the stdin so that we can use our pipe
        dup(fd[0]); // replace STDIN with pipe

        dup2(fd1, 0);
aa
        char *grep_args[4];
        grep_args[0] = strdup("grep"); 
        grep_args[1] = strdup("\\--after-context");
        grep_args[2] = strdup("--after-context=3");
        grep_args[3] = NULL;          
        execvp(grep_args[0], grep_args);
    }

    return 0;
}

