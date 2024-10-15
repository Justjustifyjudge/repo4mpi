// Rolf Rabenseifner implementation of MPI_Allreduce
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int num_nodes;
    MPI_Comm_size(MPI_COMM_WORLD, &num_nodes);

    int total_data_size = 1000000;

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    float *data=malloc(total_data_size*sizeof(float));
    //初始化需要计算的数据
    for(int i=0;i<total_data_size;i++){
        data[i]=rand()%20;
    }



    //结束和释放
    free(data);
    MPI_Finalize();
    return 0;
}