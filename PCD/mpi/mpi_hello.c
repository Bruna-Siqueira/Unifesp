// MPI Program to send data from 3 process to the fourth process
// Para compilar: mpicc -o hello mpi_hello.c
// Para executar: mpiexec -np 4 ./hello
// -np : Number of Processes

#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[])
{

    int processId;   /* rank dos processos */
    int noProcesses; /* Número de processos */
    int nameSize;    /* Tamanho do nome */
    char computerName[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &noProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    MPI_Get_processor_name(computerName, &nameSize);
    printf("Hello from process %d/%d on %s\n", processId,
           noProcesses, computerName);

    MPI_Finalize();
    return 0;
}
