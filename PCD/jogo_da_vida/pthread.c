#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define N 2050 // tabuleiro 2048x2048 + duas bordas adicionais para facilitar o cálculo da borda infinita
#define THREADS 8
#define SRAND_VALUE 1985

struct thread_data
{
    int **grid;
    int **newGrid;
    int thread_id;
};
struct thread_data thread_data_array[THREADS];

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
    int i;
    //Inicializa canteiros
    matriz[0][0] = matriz[N - 2][N - 2];
    matriz[0][N - 1] = matriz[N - 2][1];
    matriz[N - 1][0] = matriz[1][N - 2];
    matriz[N - 1][N - 1] = matriz[1][1];

    for (i = 1; i < N - 1; i++)
    {
        //Inicializa bordas verticais
        matriz[i][0] = matriz[i][N - 2];
        matriz[i][N - 1] = matriz[i][1];

        //Inicializa bordas horizontais
        matriz[0][i] = matriz[N - 2][i];
        matriz[N - 1][i] = matriz[1][i];
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

int getNeighbors(int **grid, int i, int j)
{
    int qtdVizinhos = grid[i - 1][j - 1] + grid[i - 1][j] + grid[i - 1][j + 1] + grid[i][j - 1] + grid[i][j + 1] + grid[i + 1][j - 1] + grid[i + 1][j] + grid[i + 1][j + 1];
    return qtdVizinhos;
}

void *atualizaGrid(void *threadArg)
{
    int **grid;
    int **newGrid;
    int i, j, vizinhos, thid, inicio, fim, divisao;
    struct thread_data *my_data;

    my_data = (struct thread_data *)threadArg;
    thid = (int)my_data->thread_id;
    grid = my_data->grid;
    newGrid = my_data->newGrid;

    divisao = (N - 2) / THREADS;
    inicio = (thid * divisao) - divisao + 1;
    fim = thid * divisao;

    for (i = inicio; i <= fim; i++)
    {
        for (j = 1; j < N - 1; j++)
        {
            grid[i][j] = newGrid[i][j];
            newGrid[i][j] = 0;
        }
    }
    pthread_exit(NULL);
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

    divisao = (N - 2) / THREADS;
    inicio = (thid * divisao) - divisao + 1;
    fim = thid * divisao;

    for (i = inicio; i <= fim; i++)
    {
        for (j = 1; j < N - 1; j++)
        {
            vizinhos = getNeighbors(grid, i, j);

            if ((grid[i][j] == 1 && vizinhos == 2) || vizinhos == 3)
            {
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
    pthread_t t[THREADS];
    void *status;
    //clock_t start, end;
    struct timeval inicio, final2;
    int tmili;

    grid = alocarMatriz();
    newGrid = alocarMatriz();

    iniciaTabuleiro(grid);

    qtd = qtdCelulasVivas(grid);
    printf("Condicao incial: %d\n", qtd);

    //start = clock();

    gettimeofday(&inicio, NULL);

    for (j = 0; j < 2000; j++)
    {
        for (i = 0; i < THREADS; i++)
        {
            thread_data_array[i].thread_id = i + 1;
            thread_data_array[i].grid = grid;
            thread_data_array[i].newGrid = newGrid;
            pthread_create(&t[i], NULL, &proximaGeracao, (void *)&thread_data_array[i]);
        }

        for (i = 0; i < THREADS; i++)
        {
            pthread_join(t[i], &status);
        }

        for (i = 0; i < THREADS; i++)
        {
            thread_data_array[i].thread_id = i + 1;
            thread_data_array[i].grid = grid;
            thread_data_array[i].newGrid = newGrid;
            pthread_create(&t[i], NULL, &atualizaGrid, (void *)&thread_data_array[i]);
        }

        for (i = 0; i < THREADS; i++)
        {
            pthread_join(t[i], &status);
        }
        inicializaBordas(grid);
    }

    gettimeofday(&final2, NULL);

    qtd = qtdCelulasVivas(grid);
    printf("Geracao %d: %d\n", j, qtd);

    tmili = (int)(1000 * (final2.tv_sec - inicio.tv_sec) + (final2.tv_usec - inicio.tv_usec) / 1000);
    printf("tempo decorrido: %d milisegundos\n", tmili);
    // printf("tempo decorrido tv_sec: %d\n", (int)(final2.tv_sec - inicio.tv_sec));
    // printf("tempo decorrido tv_usec: %d\n", (int)(final2.tv_usec - inicio.tv_usec));
    // printf("Time elapsed: %f\n", ((double)clock() - start) / CLOCKS_PER_SEC);

    return 0;
}
