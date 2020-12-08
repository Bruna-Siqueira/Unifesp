class RunThE implements Runnable {
	private long N;
	private int MaxThreads;
	private int thid;
	// resultado da operação
	// construtor
	public RunThE(int aN, int aMaxThreads, int ThreadID) {
		N = aN;
		MaxThreads = aMaxThreads;
		thid=ThreadID;
	}
	public void run() {
		
	    int i, j, vizinhos,inicio, fim, divisao;
	    divisao = (int) ((N - 2) / MaxThreads);
	    inicio = (thid * divisao) - divisao + 1;
	    fim = thid * divisao;

	    for (i = inicio; i <= fim; i++)
	    {
	        for (j = 1; j < N - 1; j++)
	        {
	            vizinhos = JogoDaVida.getNeighbors(JogoDaVida.grid, i, j);

				if ((JogoDaVida.grid[i][j] == 1 && vizinhos == 2) || vizinhos == 3)
            	{
                	JogoDaVida.newGrid[i][j] = 1;
            	}
	        }
	    }
	    
	}
}
