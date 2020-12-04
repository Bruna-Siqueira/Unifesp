#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define N 2050
#define MAX_THREADS 8
#define SRAND_VALUE 1985

struct thread_data
{
    int **grid;
    int **newGrid;
    int thread_id;
};
struct thread_data thread_data_array[MAX_THREADS];

int **alocarMatriz()
{

    int i, j;

    int **matriz = (int **)malloc(N * sizeof(int *));

    for (i = 0; i < N; i++)
    {
        matriz[i] = (int *)malloc(N * sizeof(int));
        for (j = 0; j < N; j++)
        {
            matriz[i][j] = 0;
        }
    }
    return matriz;
}

void inicializaBordas(int **matriz)
{
    int i, j;

    //Inicializa canteiros
    matriz[0][0] = matriz[N - 2][N - 2];
    matriz[0][N - 1] = matriz[N - 2][1];
    matriz[N - 1][0] = matriz[1][N - 2];
    matriz[N - 1][N - 1] = matriz[1][1];

    //Inicializa bordas verticais
    for (i = 1; i < N - 1; i++)
    {
        matriz[i][0] = matriz[i][N - 2];
        matriz[i][N - 1] = matriz[i][1];
    }

    //Inicializa bordas horizontais
    for (j = 1; j < N - 1; j++)
    {
        matriz[0][j] = matriz[N - 2][j];
        matriz[N - 1][j] = matriz[1][j];
    }
}

void iniciaTabuleiro(int **matriz)
{

    int i, j;
    srand(SRAND_VALUE);

    for (i = 1; i < N - 1; i++)
    {
        for (j = 1; j < N - 1; j++)
        {
            matriz[i][j] = rand() % 2;
        }
    }
    inicializaBordas(matriz);
}

void iniciaMatriz(int **matriz)
{

    int i, j;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            matriz[i][j] = 0;
        }
    }
}

int getNeighbors(int **grid, int i, int j)
{

    int qtdVizinhos = grid[i - 1][j - 1] + grid[i - 1][j] + grid[i - 1][j + 1] + grid[i][j - 1] + grid[i][j + 1] + grid[i + 1][j - 1] + grid[i + 1][j] + grid[i + 1][j + 1];

    return qtdVizinhos;
}

void atualizaGrid(int **grid, int **newGrid)
{

    int i, j;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            grid[i][j] = newGrid[i][j];
        }
    }
    inicializaBordas(grid);

    iniciaMatriz(newGrid);
}

void *proximaGeracao(void *threadArg)
{

    int **grid;
    int **newGrid;
    int i, j, vizinhos, thid, inicio, fim, divisao;
    struct thread_data *my_data;

    my_data = (struct thread_data *)threadArg;
    thid = (int)my_data->thread_id;
    grid = my_data->grid;
    newGrid = my_data->newGrid;

    divisao = (N - 2) / MAX_THREADS;
    inicio = (thid * divisao) - divisao + 1;
    fim = thid * divisao;

    for (i = inicio; i <= fim; i++)
    {
        for (j = 1; j < N - 1; j++)
        {
            vizinhos = getNeighbors(grid, i, j);

            if (grid[i][j] == 1)
            {

                if (vizinhos < 2)
                {
                    newGrid[i][j] = 0;
                }
                else if (vizinhos > 3)
                {
                    newGrid[i][j] = 0;
                }
                else
                {
                    newGrid[i][j] = 1;
                }
            }

            else
            {
                if (vizinhos == 3)
                    newGrid[i][j] = 1;
            }
        }
    }
    pthread_exit(NULL);
}

int qtdCelulasVivas(int **matriz)
{
    int i, j, soma;
    soma = 0;

    for (i = 1; i < N - 1; i++)
    {
        for (j = 1; j < N - 1; j++)
        {
            soma += matriz[i][j];
        }
    }
    return soma;
}

void imprimeMatriz(int **matriz)
{
    int i, j;

    for (i = 0; i < N; i++)
    {
        printf("\n");
        for (j = 0; j < N; j++)
        {
            printf("%d ", matriz[i][j]);
        }
    }
}

int main(void)
{

    int **grid;
    int **newGrid;
    int i, j, qtd;
    pthread_t t[MAX_THREADS];
    void *status;

    grid = alocarMatriz();
    newGrid = alocarMatriz();

    iniciaTabuleiro(grid);

    qtd = qtdCelulasVivas(grid);
    printf("Condicao incial: %d\n", qtd);

    clock_t start, end;

    struct timeval inicio, final2;
    int tmili;

    start = clock();

    gettimeofday(&inicio, NULL);

    for (j = 0; j < 2000; j++)
    {

        for (i = 0; i < MAX_THREADS; i++)
        {
            thread_data_array[i].thread_id = i + 1;
            thread_data_array[i].grid = grid;
            thread_data_array[i].newGrid = newGrid;
            pthread_create(&t[i], NULL, &proximaGeracao, (void *)&thread_data_array[i]);
        }

        for (i = 0; i < MAX_THREADS; i++)
        {
            pthread_join(t[i], &status);
        }
        atualizaGrid(grid, newGrid);
    }

    gettimeofday(&final2, NULL);
    tmili = (int)(1000 * (final2.tv_sec - inicio.tv_sec) + (final2.tv_usec - inicio.tv_usec) / 1000);

    printf("tempo decorrido: %d milisegundos\n", tmili);

    printf("tempo decorrido tv_sec: %d\n", (int)(final2.tv_sec - inicio.tv_sec));

    printf("tempo decorrido tv_usec: %d\n", (int)(final2.tv_usec - inicio.tv_usec));

    printf("Time elapsed: %f\n", ((double)clock() - start) / CLOCKS_PER_SEC);

    qtd = qtdCelulasVivas(grid);
    printf("Geracao %d: %d\n", j, qtd);

    return 0;
}
