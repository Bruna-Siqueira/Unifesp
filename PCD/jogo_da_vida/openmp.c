// Jogo da Vida em OpenMP
// Para compilar: gcc -fopenmp -o openmp openmp.c -O3
// Para executar: ./openmp
#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define N 2050 // tabuleiro 2048x2048 + duas bordas adicionais para facilitar o cálculo da borda infinita
#define SRAND_VALUE 1985
#define THREADS 8 // numero de threads

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

#pragma omp for private(i)
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

void atualizaGrid(int **grid, int **newGrid)
{
    int i, j;
#pragma omp for private(i, j)
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

void proximaGeracao(int **grid, int **newGrid)
{
    int i, j, vizinhos;
#pragma omp for private(i, j, vizinhos)
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

int main(void)
{
    int **grid;
    int **newGrid;
    int i, qtd, th_id, tmili;
    double start, end;

    grid = alocarMatriz();
    newGrid = alocarMatriz();

    iniciaTabuleiro(grid);

    qtd = qtdCelulasVivas(grid);
    printf("Condicao incial: %d\n", qtd);

    start = omp_get_wtime();

#pragma omp parallel private(i) shared(grid, newGrid) num_threads(THREADS)
    {
        for (i = 0; i < 2000; i++)
        {
            proximaGeracao(grid, newGrid);
            atualizaGrid(grid, newGrid);
        }
    }

    end = omp_get_wtime();

    qtd = qtdCelulasVivas(grid);
    printf("Geracao 2000: %d\n", qtd);

    tmili = (int)((end - start) * 1000);
    printf("tempo decorrido: %d milisegundos\n", tmili);

    return 0;
}
