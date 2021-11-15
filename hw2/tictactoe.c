#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

char **MATRIX;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
int turn = 1;
int game_over = 0;

int matrix_size = 3;

void initialize_matrix(int N);
void *play(void *arg0);
int is_game_over();
void print_matrix();

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Wrong command line arguments!\n");
        return 1;
    }
    matrix_size = atoi(argv[1]);

    initialize_matrix(matrix_size);
    srand(time(NULL));

    pthread_t player1, player2;
    int player1_num = 1;
    int player2_num = 2;

    pthread_create(&player1, NULL, play, &player1_num);
    pthread_create(&player2, NULL, play, &player2_num);

    pthread_join(player2, NULL);
    pthread_join(player1, NULL);

    printf("Game end\n");
    if (game_over == -1)
    {
        printf("Tie\n");
    }
    else
    {
        printf("Winner is %c\n", game_over == 1 ? 'X' : 'O');
    }

    print_matrix(matrix_size);

    return 0;
}

void *play(void *arg0)
{
    int *player = (int *)arg0;
    char sign = *player == 1 ? 'X' : 'O';

    int x = 0, y = 0;
    while (game_over == 0)
    {
        pthread_mutex_lock(&m);
        if ((game_over = is_game_over()) != 0)
        {
            pthread_mutex_unlock(&m);
            return NULL;
        }
        if (turn == *player)
        {
            do
            {
                x = rand() % matrix_size;
                y = rand() % matrix_size;
            } while (MATRIX[x][y] != ' ');

            MATRIX[x][y] = sign;
            printf("Player %c played on: (%d,%d)\n", sign, x, y);

            turn = *player == 1 ? 2 : 1;

            pthread_mutex_unlock(&m);
        }
        else
        {
            pthread_mutex_unlock(&m);
        }
    }
}

void initialize_matrix(int N)
{
    MATRIX = malloc(N * sizeof(char *));

    for (int i = 0; i < N; i++)
    {
        MATRIX[i] = malloc(N * sizeof(char));

        for (int j = 0; j < N; j++)
            MATRIX[i][j] = ' ';
    }
}

int is_game_over()
{
    for (int i = 0; i < matrix_size; i++)
    {
        int count = 0;
        char candidate = MATRIX[i][0];
        if (candidate == ' ')
            break;
        for (int j = 0; j < matrix_size; j++)
        {
            count = MATRIX[i][j] == candidate ? count + 1 : count;
        }

        if (count == matrix_size)
        {
            return candidate == 'X' ? 1 : 2;
        }
    }

    for (int i = 0; i < matrix_size; i++)
    {
        int count = 0;
        char candidate = MATRIX[0][i];
        if (candidate == ' ')
            break;
        for (int j = 0; j < matrix_size; j++)
        {
            count = MATRIX[j][i] == candidate ? count + 1 : count;
        }

        if (count == matrix_size)
        {
            return candidate == 'X' ? 1 : 2;
        }
    }

    for (int i = 0; i < 1; i++)
    {
        int count = 0;
        char candidate = MATRIX[0][0];
        if (candidate == ' ')
            break;
        for (int j = 0; j < matrix_size; j++)
        {
            count = MATRIX[j][j] == candidate ? count + 1 : count;
        }

        if (count == matrix_size)
        {
            return candidate == 'X' ? 1 : 2;
        }
    }

    for (int i = 0; i < 1; i++)
    {
        int count = 0;
        char candidate = MATRIX[0][matrix_size - 1];
        if (candidate == ' ')
            break;
        for (int j = 0; j < matrix_size; j++)
        {
            count = MATRIX[j][matrix_size - j - 1] == candidate ? count + 1 : count;
        }

        if (count == matrix_size)
        {
            return candidate == 'X' ? 1 : 2;
        }
    }

    int count = 0;
    for (int i = 0; i < matrix_size; i++)
    {
        for (int j = 0; j < matrix_size; j++)
        {
            count = MATRIX[j][i] == ' ' ? count + 1 : count;
        }
    }
    if (count == 0)
    {
        return -1;
    }

    return 0;
}

void print_matrix(int N)
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            printf("[%c]", (char)MATRIX[i][j]);
        }

        printf("\n");
    }
}