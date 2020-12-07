import java.util.Random;

public class JogoDaVida { 
	static int N=2050;
	static int grid[][] = new int[N][N];
	static int newGrid[][] = new int[N][N];
	
	static void inicializaBordas(int matriz[][])
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
	
	static void iniciaTabuleiro(int matriz[][])
	{

	    int i, j;
	    Random gerador = new Random(1985);

	    for (i = 1; i < N - 1; i++)
	    {
	        for (j = 1; j < N - 1; j++)
	        {
	            matriz[i][j] = gerador.nextInt(2147483647) % 2;
	        }
	    }
	    inicializaBordas(matriz);
	}

	static void iniciaMatriz(int matriz[][])
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
	
	

	static int getNeighbors(int grid[][], int i, int j)
	{

	    int qtdVizinhos = grid[i - 1][j - 1] + grid[i - 1][j] + grid[i - 1][j + 1] + grid[i][j - 1] + grid[i][j + 1] + grid[i + 1][j - 1] + grid[i + 1][j] + grid[i + 1][j + 1];

	    return qtdVizinhos;
	}

	static void atualizaGrid(int grid[][], int newGrid[][])
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


	static int qtdCelulasVivas(int matriz[][])
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

	void imprimeMatriz(int matriz[][])
	{
	    int i, j;

	    for (i = 0; i < N; i++)
	    {
	        System.out.println("\n");
	        for (j = 0; j < N; j++)
	        {
	        	System.out.println( matriz[i][j]+" ");
	        }
	    }
	}
	
	public static void main(String args[]) throws InterruptedException{
		
		int MaxThreads = 1;
		int i, j, qtd;
		Thread[] th;
		RunThE[] rh;
		rh = new RunThE[MaxThreads];
		th = new Thread[MaxThreads];
		iniciaMatriz(grid);
		iniciaMatriz(newGrid);
		iniciaTabuleiro(grid);
		qtd = qtdCelulasVivas(grid);
		System.out.println("Condição Inicial:"+qtd+"\n");
		
		long startTime = System.currentTimeMillis();
		
		for(j=0; j<2; j++) {
			for(i=0; i<MaxThreads; i++) {
				rh[i] = new RunThE(N, MaxThreads, i+1);
				th[i] = new Thread(rh[i]);
				th[i].start();
			}
			
			for(i=0; i<MaxThreads; i++) {
				th[i].join();
			}
			
			atualizaGrid(grid, newGrid);
			qtd = qtdCelulasVivas(grid);
			System.out.println("Geração "+(j+1)+": "+qtd+"\n");
		}
		
		qtd = qtdCelulasVivas(grid);
		System.out.println("Condição Final:"+qtd+"\n");
		
		long calcTime = System.currentTimeMillis() - startTime;
		System.out.println("Em " + calcTime + "ms.\n");
		
		//pos-proc
       //Thread.sleep(1000);
		

	}
}
