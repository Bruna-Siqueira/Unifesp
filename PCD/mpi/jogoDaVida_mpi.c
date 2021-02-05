#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define N 10 // tabuleiro 2048x2048 + duas bordas adicionais para facilitar o cálculo da borda infinita
#define SRAND_VALUE 1985

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

void iniciaTabuleiro(int **matriz, int qtdProcessos, int rank)
{
    int i, j, inicio, fim, divisao;
    srand(1985);

    divisao = (N - 2) / qtdProcessos;
    inicio = ((rank + 1) * divisao) - divisao + 1;
    fim = (rank + 1) * divisao;

    for (i = inicio; i <= fim; i++)
    {
        for (j = 1; j < N - 1; j++)
        {
            matriz[i][j] = rand() % 2;
            //printf("%d", rand() % 2);
            //printf("%d", matriz[i][j]);
        }
        matriz[i][0] = matriz[i][N - 2];
        matriz[i][N - 1] = matriz[i][1];
    }

    if (rank == 0)
    {
        MPI_Send(&matriz[1][0], (N - 1), MPI_INT, (qtdProcessos - 1), 100, MPI_COMM_WORLD);
        MPI_Recv(&matriz[0][0], (N - 1), MPI_INT, (qtdProcessos - 1), 100, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    else if (rank == (qtdProcessos - 1))
    {
        MPI_Recv(&matriz[N - 1][0], (N - 1), MPI_INT, 0, 100, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(&matriz[N - 1][0], (N - 1), MPI_INT, 0, 100, MPI_COMM_WORLD);
    }

    if (rank != (qtdProcessos - 1))
    {
        MPI_Send(&matriz[fim][0], (N - 1), MPI_INT, (rank + 1), 100, MPI_COMM_WORLD);
        MPI_Recv(&matriz[fim + 1][0], (N - 1), MPI_INT, (rank + 1), 100, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    if (rank != 0)
    {
        MPI_Recv(&matriz[inicio - 1][0], (N - 1), MPI_INT, (rank - 1), 100, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(&matriz[inicio][0], (N - 1), MPI_INT, (rank - 1), 100, MPI_COMM_WORLD);
    }

    //inicializaBordas(matriz);
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
            newGrid[i][j] = 0;
        }
    }
    inicializaBordas(grid);
}

void proximaGeracao(int **grid, int **newGrid, int processId)
{
    int i, j, vizinhos;
    for (i = 1; i < N - 1; i++)
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

int main(int argc, char *argv[])
{
    int rank;         /* rank dos processos */
    int qtdProcessos; /* Número de processos */

    int **grid;
    int **newGrid;
    int i, qtd;
    //clock_t start, end;
    struct timeval inicio, final2;
    int tmili;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &qtdProcessos);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    grid = alocarMatriz();
    newGrid = alocarMatriz();

    iniciaTabuleiro(grid, qtdProcessos, rank);

    imprimeMatriz(grid);

    MPI_Finalize();

    // qtd = qtdCelulasVivas(grid);
    // printf("Condicao incial: %d\n", qtd);

    // //start = clock();

    // gettimeofday(&inicio, NULL);

    // for (i = 0; i < 2000; i++)
    // {
    //     proximaGeracao(grid, newGrid, rank);
    //     atualizaGrid(grid, newGrid);
    // }

    // gettimeofday(&final2, NULL);

    // qtd = qtdCelulasVivas(grid);
    // printf("Geracao %d: %d\n", i, qtd);

    // tmili = (int)(1000 * (final2.tv_sec - inicio.tv_sec) + (final2.tv_usec - inicio.tv_usec) / 1000);
    // printf("tempo decorrido: %d milisegundos\n", tmili);
    // // printf("tempo decorrido tv_sec: %d\n", (int)(final2.tv_sec - inicio.tv_sec));
    // // printf("tempo decorrido tv_usec: %d\n", (int)(final2.tv_usec - inicio.tv_usec));
    // // printf("Time elapsed: %f\n", ((double)clock() - start) / CLOCKS_PER_SEC);

    return 0;
}
