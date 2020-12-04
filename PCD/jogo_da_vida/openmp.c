#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define N 2050
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
    int i, j;

    //Inicializa canteiros
    matriz[0][0] = matriz[N - 2][N - 2];
    matriz[0][N - 1] = matriz[N - 2][1];
    matriz[N - 1][0] = matriz[1][N - 2];
    matriz[N - 1][N - 1] = matriz[1][1];

#pragma omp parallel
    {
#pragma omp for
        //Inicializa bordas verticais
        for (i = 1; i < N - 1; i++)
        {
            matriz[i][0] = matriz[i][N - 2];
            matriz[i][N - 1] = matriz[i][1];
        }
#pragma omp for
        //Inicializa bordas horizontais
        for (j = 1; j < N - 1; j++)
        {
            matriz[0][j] = matriz[N - 2][j];
            matriz[N - 1][j] = matriz[1][j];
        }
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

#pragma omp parallel private(j)
#pragma omp for
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

#pragma omp parallel private(j)
#pragma omp for
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

void proximaGeracao(int **grid, int **newGrid)
{

    int i, j, vizinhos;

#pragma omp parallel private(j, vizinhos)
#pragma omp for
    for (i = 1; i < N - 1; i++)
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
    int i, qtd;
    double start, end;

    grid = alocarMatriz();
    newGrid = alocarMatriz();

    iniciaTabuleiro(grid);

    qtd = qtdCelulasVivas(grid);
    printf("Condicao incial: %d\n", qtd);

    start = omp_get_wtime();

    for (i = 0; i < 2000; i++)
    {
        proximaGeracao(grid, newGrid);
        atualizaGrid(grid, newGrid);
    }

    end = omp_get_wtime();
    printf(" took %f seconds.\n", end - start);

    qtd = qtdCelulasVivas(grid);
    printf("Geracao %d: %d\n", i, qtd);

    return 0;
}
