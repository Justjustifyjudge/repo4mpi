#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]){
    MPI_Init(NULL, NULL);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    char process_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(process_name, &name_len);
    printf("Hello world from processor %s, rank %d out of %d processors\n", process_name, world_rank, world_size);

    MPI_Finalize();
    return 0;
}