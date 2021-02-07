#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define N 2048 // tabuleiro 2048x2048 + duas bordas adicionais para facilitar o cálculo da borda infinita
#define SRAND_VALUE 1985

int **alocarMatriz(int linhas)
{
    int i, j;
    int **matriz = (int **)malloc(linhas * sizeof(int *));
    for (i = 0; i < linhas; i++)
    {
        matriz[i] = (int *)malloc((N + 2) * sizeof(int));
        for (j = 0; j < N + 2; j++)
        {
            matriz[i][j] = 0;
        }
    }
    return matriz;
}

void imprimeMatriz(int **matriz, int linhas)
{
    int i, j;
    for (i = 0; i < linhas; i++)
    {
        printf("\n");
        for (j = 0; j < N + 2; j++)
        {
            printf("%d ", matriz[i][j]);
        }
    }
}

void inicializaFronteiras(int **matriz, int qtdProcessos)
{
    int j;

    if (qtdProcessos == 1)
    {
        for (j = 0; j < N + 2; j++)
        {
            // Inicializa bordas horizontais para 1 processo apenas
            matriz[0][j] = matriz[N][j];
            matriz[N + 1][j] = matriz[1][j];
        }
    }
}

void iniciaTabuleiro(int **matriz, int qtdProcessos, int rank, int linhas)
{
    int i, j, inicio, fim, divisao;

    for (i = 1; i < linhas - 1; i++)
    {
        for (j = 1; j <= N; j++)
        {
            matriz[i][j] = rand() % 2;
            //printf("%d", rand() % 2);
            //printf("%d", matriz[i][j]);
        }
        // Inicializa bordas verticais
        matriz[i][0] = matriz[i][N];
        matriz[i][N + 1] = matriz[i][1];
    }

    inicializaFronteiras(matriz, qtdProcessos);

    // else
    // { // Inicializa bordas horizontais para o processo 0
    //     // Não vai funcionar pq não sabemos a ordem em que os processos estão preenchendo a matriz
    //     if (rank == 0)
    //     {
    //         MPI_Send(&matriz[1][0], N, MPI_INT, (qtdProcessos - 1), 100, MPI_COMM_WORLD);
    //         MPI_Recv(&matriz[0][0], N, MPI_INT, (qtdProcessos - 1), 100, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //     }
    //     // Inicializa bordas horizontais para o último processo
    //     // else if (rank == (qtdProcessos - 1))
    //     // {
    //     //     MPI_Recv(&matriz[linhas - 1][0], N, MPI_INT, 0, 100, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //     //     MPI_Send(&matriz[linhas - 2][0], N, MPI_INT, 0, 100, MPI_COMM_WORLD);
    //     // }

    //     // if (rank != (qtdProcessos - 1))
    //     // {
    //     //     MPI_Send(&matriz[linhas - 2][0], N, MPI_INT, (rank + 1), 100, MPI_COMM_WORLD);
    //     //     MPI_Recv(&matriz[linhas - 1][0], N, MPI_INT, (rank + 1), 100, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //     // }
    //     // if (rank != 0)
    //     // {
    //     //     MPI_Recv(&matriz[inicio - 1][0], N, MPI_INT, (rank - 1), 100, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //     //     MPI_Send(&matriz[inicio][0], N, MPI_INT, (rank - 1), 100, MPI_COMM_WORLD);
    //     // }
    // }

    //inicializaBordas(matriz);
}

int getNeighbors(int **grid, int i, int j)
{
    int qtdVizinhos = grid[i - 1][j - 1] + grid[i - 1][j] + grid[i - 1][j + 1] + grid[i][j - 1] + grid[i][j + 1] + grid[i + 1][j - 1] + grid[i + 1][j] + grid[i + 1][j + 1];
    return qtdVizinhos;
}

void atualizaGrid(int **grid, int **newGrid, int linhas, int qtdProcessos)
{
    int i, j;

    for (i = 0; i < linhas; i++)
    {

        for (j = 0; j < N + 2; j++)
        {
            grid[i][j] = newGrid[i][j];
            newGrid[i][j] = 0;
        }
        // Inicializa bordas verticais
        grid[i][0] = grid[i][N];
        grid[i][N + 1] = grid[i][1];
    }
    inicializaFronteiras(grid, qtdProcessos);
}

void proximaGeracao(int **grid, int **newGrid, int linhas)
{
    int i, j, vizinhos;
    for (i = 1; i < linhas - 1; i++)
    {
        for (j = 1; j <= N; j++)
        {
            vizinhos = getNeighbors(grid, i, j);

            if ((grid[i][j] == 1 && vizinhos == 2) || vizinhos == 3)
            {
                newGrid[i][j] = 1;
            }
        }
    }
}

int qtdCelulasVivas(int **matriz, int linhas)
{
    int i, j, soma;
    soma = 0;
    for (i = 1; i < linhas - 1; i++)
    {
        for (j = 1; j <= N; j++)
        {
            soma += matriz[i][j];
        }
    }
    return soma;
}

int main(int argc, char *argv[])
{
    int rank;         /* rank dos processos */
    int qtdProcessos; /* Número de processos */
    int linhas;
    int **grid;
    int **newGrid;
    int **finalGrid;
    int i, qtd;
    //clock_t start, end;
    struct timeval inicio, final2;
    int tmili;

    srand(1985);

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &qtdProcessos);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    linhas = (N / qtdProcessos) + 2;

    grid = alocarMatriz(linhas);
    newGrid = alocarMatriz(linhas);

    // usado quando temos o MPI_Gather
    // if (rank == 0)
    // {
    //     finalGrid = (int **)malloc(6 * sizeof(int));
    //     finalGrid[0] = (int *)malloc(18 * sizeof(int));
    //     finalGrid[1] = (int *)malloc(18 * sizeof(int));
    //     //finalGrid = alocarMatriz(N);
    // }

    iniciaTabuleiro(grid, qtdProcessos, rank, linhas);

    //imprimeMatriz(grid, linhas);

    //MPI_Gather(grid[1], N, MPI_INT, finalGrid[0], N, MPI_INT, 0, MPI_COMM_WORLD);
    //MPI_Gather(grid[2], N, MPI_INT, finalGrid[1], N, MPI_INT, 0, MPI_COMM_WORLD);

    qtd = qtdCelulasVivas(grid, linhas);
    printf("Condicao incial: %d\n", qtd);

    // //start = clock();

    gettimeofday(&inicio, NULL);

    for (i = 0; i < 2000; i++)
    {
        proximaGeracao(grid, newGrid, linhas);
        atualizaGrid(grid, newGrid, linhas, qtdProcessos);

        // printf("\n newGrid");
        // imprimeMatriz(newGrid, linhas);
        // printf("\n grid");
        // imprimeMatriz(grid, linhas);

        // qtd = qtdCelulasVivas(grid, linhas);
        // printf("Geracao %d: %d\n", i + 1, qtd);
    }

    // gettimeofday(&final2, NULL);

    qtd = qtdCelulasVivas(grid, linhas);
    printf("Geracao %d: %d\n", i, qtd);

    // tmili = (int)(1000 * (final2.tv_sec - inicio.tv_sec) + (final2.tv_usec - inicio.tv_usec) / 1000);
    // printf("tempo decorrido: %d milisegundos\n", tmili);
    // // printf("tempo decorrido tv_sec: %d\n", (int)(final2.tv_sec - inicio.tv_sec));
    // // printf("tempo decorrido tv_usec: %d\n", (int)(final2.tv_usec - inicio.tv_usec));
    // // printf("Time elapsed: %f\n", ((double)clock() - start) / CLOCKS_PER_SEC);
    MPI_Finalize();
    return 0;
}
